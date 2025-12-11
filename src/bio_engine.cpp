#include "bio_engine.h"
#include <cmath>
#include <iostream>

namespace genesis {

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------
void BioEngine::init(size_t neuron_count, size_t synapse_density_per_neuron, uint64_t seed) {
    neuron_count_ = neuron_count;
    rng_ = std::make_unique<utils::FastRNG>(seed);
    ctx_.rng_seed = seed;
    
    // FIX: Start tick at 100 to prevents STDP artifacts during first few ms.
    // If tick starts at 0, diff = 0 - 0 = 0, triggering immediate LTD.
    ctx_.current_tick = 100; 

    // 1. Resize Neuron Arrays (SoA)
    neurons_.resize(neuron_count);

    // Initialize Neurons to Resting State
    for (size_t i = 0; i < neuron_count; ++i) {
        neurons_.membrane_potential[i] = V_REST_MV;
        neurons_.recovery_variable[i] = 0.0f;
        neurons_.atp_level[i] = ATP_MAX;
        neurons_.refractory_timer[i] = 0;
        neurons_.has_fired[i] = false;
        neurons_.last_spike_time[i] = 0; // 0 represents "never/long ago" relative to tick 100
        neurons_.avg_firing_rate[i] = 0.0f;
    }

    // 2. Generate Topology (Random Connectivity)
    size_t est_synapse_count = neuron_count * synapse_density_per_neuron;
    if (est_synapse_count > MAX_SYNAPSES) {
        est_synapse_count = MAX_SYNAPSES;
    }

    synapses_.resize(est_synapse_count);
    topology_.resize(neuron_count);

    size_t syn_idx = 0;
    
    for (size_t pre_id = 0; pre_id < neuron_count; ++pre_id) {
        topology_.outgoing_start[pre_id] = static_cast<uint32_t>(syn_idx);
        uint32_t count = 0;

        size_t n_connections = synapse_density_per_neuron;
        if (syn_idx + n_connections > est_synapse_count) {
            n_connections = est_synapse_count - syn_idx;
        }

        for (size_t c = 0; c < n_connections; ++c) {
            uint32_t post_id = rng_->next_int(0, static_cast<int>(neuron_count - 1));
            if (post_id == pre_id) post_id = (post_id + 1) % neuron_count;

            synapses_.pre_indices[syn_idx]  = static_cast<uint32_t>(pre_id);
            synapses_.post_indices[syn_idx] = post_id;
            
            synapses_.weights[syn_idx]            = rng_->next_float() * 0.1f + 0.01f; 
            synapses_.eligibility_traces[syn_idx] = 0.0f;
            synapses_.is_inhibitory[syn_idx]      = (rng_->next_float() < 0.2f);

            syn_idx++;
            count++;
        }
        topology_.outgoing_count[pre_id] = count;
        
        if (syn_idx >= est_synapse_count) break;
    }
}

// -----------------------------------------------------------------------------
// Core Physics Loop
// -----------------------------------------------------------------------------
void BioEngine::tick() {
    ctx_.current_tick++;
    ctx_.total_spikes_this_tick = 0;
    
    // Decay global dopamine
    ctx_.dopamine *= DOPAMINE_DECAY_FACTOR;
    if (ctx_.dopamine < 0.001f) ctx_.dopamine = 0.0f;

    update_metabolism();
    update_synapses();
    
    auto firing_indices = integrate_and_fire();
    ctx_.total_spikes_this_tick = firing_indices.size();

    propagate_spikes(firing_indices);
}

// -----------------------------------------------------------------------------
// Physics Modules
// -----------------------------------------------------------------------------

void BioEngine::update_metabolism() {
    const size_t cnt = neuron_count_;
    for (size_t i = 0; i < cnt; ++i) {
        float& atp = neurons_.atp_level[i];
        atp += ATP_REFILL_RATE;
        if (atp > 1.0f) atp = 1.0f;
    }
}

void BioEngine::update_synapses() {
    const size_t syn_count = synapses_.weights.size();
    const float da = ctx_.dopamine;
    const bool has_dopamine = (da > 0.0001f);

    for (size_t k = 0; k < syn_count; ++k) {
        float& trace = synapses_.eligibility_traces[k];
        trace *= TRACE_DECAY_FACTOR;

        if (has_dopamine) {
            float& w = synapses_.weights[k];
            float delta_w = da * trace; 
            w += delta_w;

            if (w > W_MAX) w = W_MAX;
            else if (w < W_MIN) w = W_MIN;
        }
    }
}

std::vector<uint32_t> BioEngine::integrate_and_fire() {
    std::vector<uint32_t> fired;
    fired.reserve(neuron_count_ / 10); 

    const size_t cnt = neuron_count_;
    
    for (size_t i = 0; i < cnt; ++i) {
        neurons_.has_fired[i] = false;

        if (neurons_.refractory_timer[i] > 0) {
            neurons_.refractory_timer[i]--;
            continue; 
        }

        float& v = neurons_.membrane_potential[i];
        
        // Voltage Decay (Leak)
        float v_diff = v - V_REST_MV;
        v_diff *= VOLTAGE_DECAY_FACTOR;
        v = V_REST_MV + v_diff;

        // Threshold & Energy Gate
        if (v >= V_THRESH_MV) {
            if (neurons_.atp_level[i] >= ATP_MIN_TO_FIRE) {
                fired.push_back(static_cast<uint32_t>(i));
                neurons_.has_fired[i] = true;
            }
        }
    }
    return fired;
}

void BioEngine::propagate_spikes(const std::vector<uint32_t>& firing_indices) {
    const float ach_gain = ctx_.acetylcholine;
    
    for (uint32_t pre_id : firing_indices) {
        neurons_.membrane_potential[pre_id] = V_RESET_MV;
        neurons_.refractory_timer[pre_id]   = REFRACTORY_TICKS;
        neurons_.atp_level[pre_id]          -= SPIKE_COST;
        neurons_.last_spike_time[pre_id]    = ctx_.current_tick;

        uint32_t start = topology_.outgoing_start[pre_id];
        uint32_t count = topology_.outgoing_count[pre_id];

        for (uint32_t k = start; k < start + count; ++k) {
            uint32_t post_id = synapses_.post_indices[k];
            
            float w = synapses_.weights[k];
            bool inhib = synapses_.is_inhibitory[k];
            
            float signal = w * ach_gain;
            if (inhib) signal = -signal;

            neurons_.membrane_potential[post_id] += signal;

            // B. STDP: Pre fired -> Increase Trace (LTP tag)
            synapses_.eligibility_traces[k] += A_PLUS;

            // C. STDP: Cross-Check (LTD)
            // Did Post fire recently?
            uint64_t post_fire_time = neurons_.last_spike_time[post_id];
            
            // FIX: Only check if Post has actually fired since initialization (time > 0)
            // This prevents "never fired" (0) from triggering LTD against current tick (100+)
            if (post_fire_time > 0) {
                uint64_t diff = ctx_.current_tick - post_fire_time;
                if (diff < static_cast<uint64_t>(STDP_WINDOW_MS)) {
                    // Post fired JUST BEFORE Pre arrived.
                    // This is Anti-Causal. Penalize trace.
                    synapses_.eligibility_traces[k] -= A_MINUS;
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Interaction
// -----------------------------------------------------------------------------
void BioEngine::inject_stimulus(uint32_t neuron_id, float current_mv) {
    if (neuron_id < neuron_count_) {
        neurons_.membrane_potential[neuron_id] += current_mv;
    }
}

void BioEngine::set_dopamine(float level) {
    ctx_.dopamine = level;
}

} // namespace genesis