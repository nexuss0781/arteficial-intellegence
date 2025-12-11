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
    ctx_.current_tick = 0;

    // 1. Resize Neuron Arrays (SoA)
    neurons_.resize(neuron_count);

    // Initialize Neurons to Resting State
    for (size_t i = 0; i < neuron_count; ++i) {
        neurons_.membrane_potential[i] = V_REST_MV;
        neurons_.recovery_variable[i] = 0.0f;
        neurons_.atp_level[i] = ATP_MAX; // Start fully charged
        neurons_.refractory_timer[i] = 0;
        neurons_.has_fired[i] = false;
        neurons_.last_spike_time[i] = 0; // 0 implies never fired (or long ago)
        neurons_.avg_firing_rate[i] = 0.0f;
    }

    // 2. Generate Topology (Random Connectivity)
    // We strictly limit density to ensure memory predictability.
    size_t est_synapse_count = neuron_count * synapse_density_per_neuron;
    if (est_synapse_count > MAX_SYNAPSES) {
        est_synapse_count = MAX_SYNAPSES;
    }

    synapses_.resize(est_synapse_count);
    topology_.resize(neuron_count);

    size_t syn_idx = 0;
    
    // Create connections
    for (size_t pre_id = 0; pre_id < neuron_count; ++pre_id) {
        topology_.outgoing_start[pre_id] = static_cast<uint32_t>(syn_idx);
        uint32_t count = 0;

        // Determine number of connections for this neuron
        // Simple uniform random distribution for now
        size_t n_connections = synapse_density_per_neuron;
        
        // Safety clamp against global max
        if (syn_idx + n_connections > est_synapse_count) {
            n_connections = est_synapse_count - syn_idx;
        }

        for (size_t c = 0; c < n_connections; ++c) {
            // Pick a random post_neuron (exclude self)
            uint32_t post_id = rng_->next_int(0, static_cast<int>(neuron_count - 1));
            if (post_id == pre_id) post_id = (post_id + 1) % neuron_count;

            synapses_.pre_indices[syn_idx]  = static_cast<uint32_t>(pre_id);
            synapses_.post_indices[syn_idx] = post_id;
            
            // Random initialization
            // Weights biased towards lower end to prevent initial explosion
            synapses_.weights[syn_idx]            = rng_->next_float() * 0.1f + 0.01f; 
            synapses_.eligibility_traces[syn_idx] = 0.0f;
            
            // 20% Inhibitory neurons (Dale's Law approximation at connection level)
            // In a real biological model, a neuron is strictly E or I. 
            // Here we allow mixed outputs for Phase 1 simplicity, or we could enforce it.
            // Let's rely on random probability for Phase 1.
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
    // 0. Advance Time & Decay Chemicals
    ctx_.current_tick++;
    ctx_.total_spikes_this_tick = 0;
    
    // Decay global dopamine
    ctx_.dopamine *= DOPAMINE_DECAY_FACTOR;
    if (ctx_.dopamine < 0.001f) ctx_.dopamine = 0.0f;

    // 1. Metabolic Regulation (Refill Energy)
    update_metabolism();

    // 2. Synaptic Dynamics (Trace Decay + Weight Update)
    update_synapses();

    // 3. Membrane Integration (Voltage Update)
    auto firing_indices = integrate_and_fire();
    ctx_.total_spikes_this_tick = firing_indices.size();

    // 4. Spike Propagation (Transmission & Plasticity Events)
    propagate_spikes(firing_indices);
}

// -----------------------------------------------------------------------------
// Physics Modules
// -----------------------------------------------------------------------------

void BioEngine::update_metabolism() {
    // Vectorized refill
    // Could be optimized with SIMD intrinsics in future
    const size_t cnt = neuron_count_;
    for (size_t i = 0; i < cnt; ++i) {
        float& atp = neurons_.atp_level[i];
        atp += ATP_REFILL_RATE;
        // Hard clamp to 1.0
        if (atp > 1.0f) atp = 1.0f;
    }
}

void BioEngine::update_synapses() {
    const size_t syn_count = synapses_.weights.size();
    const float da = ctx_.dopamine;
    const bool has_dopamine = (da > 0.0001f);

    for (size_t k = 0; k < syn_count; ++k) {
        // 1. Trace Decay
        float& trace = synapses_.eligibility_traces[k];
        trace *= TRACE_DECAY_FACTOR;

        // 2. Dopamine Gating (Weight Update)
        // W += eta * D * E
        // Only run if significant dopamine is present to save cycles
        if (has_dopamine) {
            float& w = synapses_.weights[k];
            // eta is implicitly 1.0 here relative to the Trace magnitude
            float delta_w = da * trace; 
            
            // Apply update
            w += delta_w;

            // 3. Hard Clamping
            // std::clamp style check
            if (w > W_MAX) w = W_MAX;
            else if (w < W_MIN) w = W_MIN;
        }
    }
}

std::vector<uint32_t> BioEngine::integrate_and_fire() {
    std::vector<uint32_t> fired;
    fired.reserve(neuron_count_ / 10); // Heuristic reservation

    const size_t cnt = neuron_count_;
    
    for (size_t i = 0; i < cnt; ++i) {
        // Reset fired flag from previous tick
        neurons_.has_fired[i] = false;

        // 1. Refractory Check
        if (neurons_.refractory_timer[i] > 0) {
            neurons_.refractory_timer[i]--;
            // Even if refractory, voltage decays towards rest
            // neurons_.membrane_potential[i] *= VOLTAGE_DECAY_FACTOR; // Optional biological detail
            continue; 
        }

        // 2. Voltage Decay (Leak)
        // V(t+1) = V(t) * Decay
        // Input summation happens in propagate_spikes (forward buffer logic) 
        // OR effectively here if we assume inputs arrived instantaneously from prev tick.
        // In this architecture, inputs were added directly to V in the previous tick's propagate step.
        float& v = neurons_.membrane_potential[i];
        
        // Decay relative to Resting Potential (0-centered for math simplicity, then offset?)
        // Standard equation: V_new = V_old + (V_rest - V_old)/Tau
        // Using precomputed multiplicative decay:
        // V_dist_from_rest = (v - V_REST_MV);
        // V_dist_new = V_dist_from_rest * Decay;
        // v = V_REST_MV + V_dist_new;
        
        float v_diff = v - V_REST_MV;
        v_diff *= VOLTAGE_DECAY_FACTOR;
        v = V_REST_MV + v_diff;

        // 3. Threshold Check
        if (v >= V_THRESH_MV) {
            // 4. Energy Gate
            if (neurons_.atp_level[i] >= ATP_MIN_TO_FIRE) {
                // Fire!
                fired.push_back(static_cast<uint32_t>(i));
                neurons_.has_fired[i] = true;
            } else {
                // Failed to fire due to fatigue (Silence)
                // Voltage remains high but no spike is sent
            }
        }
    }
    return fired;
}

void BioEngine::propagate_spikes(const std::vector<uint32_t>& firing_indices) {
    const float ach_gain = ctx_.acetylcholine;
    
    for (uint32_t pre_id : firing_indices) {
        // 1. Neuron Reset
        neurons_.membrane_potential[pre_id] = V_RESET_MV;
        neurons_.refractory_timer[pre_id]   = REFRACTORY_TICKS;
        neurons_.atp_level[pre_id]          -= SPIKE_COST;
        neurons_.last_spike_time[pre_id]    = ctx_.current_tick;

        // 2. Synaptic Transmission
        uint32_t start = topology_.outgoing_start[pre_id];
        uint32_t count = topology_.outgoing_count[pre_id];

        for (uint32_t k = start; k < start + count; ++k) {
            uint32_t post_id = synapses_.post_indices[k];
            
            // A. Signal Propagation (Voltage Transfer)
            float w = synapses_.weights[k];
            bool inhib = synapses_.is_inhibitory[k];
            
            // ACh modulates gain
            float signal = w * ach_gain;
            if (inhib) signal = -signal;

            // Add directly to Post-Neuron Voltage (Immediate transmission assumption for Phase 1)
            neurons_.membrane_potential[post_id] += signal;

            // B. STDP Event (Pre-Synaptic Spike)
            // Rule: Pre fired, so we create a "tag" for potential LTP.
            // Eligibility += A_PLUS
            synapses_.eligibility_traces[k] += A_PLUS;

            // C. STDP Cross-Check (LTD: Post before Pre)
            // If the target (Post) fired recently, this Pre spike arrived "too late".
            // That implies an anti-causal relationship (or noise).
            // We penalize the trace (LTD).
            uint64_t post_fire_time = neurons_.last_spike_time[post_id];
            uint64_t diff = ctx_.current_tick - post_fire_time;

            if (diff < static_cast<uint64_t>(STDP_WINDOW_MS)) {
                // "Post fired just before Pre arrived" -> Depression
                // We modify the trace downwards immediately
                synapses_.eligibility_traces[k] -= A_MINUS;
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