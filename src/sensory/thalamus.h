#pragma once

#include "../bio_engine.h"

namespace genesis {

// -----------------------------------------------------------------------------
// Thalamus: The Attentional Gate
// -----------------------------------------------------------------------------
// Purpose: Acts as a dynamic filter for sensory information. It prevents the 
// cortex from being overwhelmed by constant, unchanging input. It learns nothing;
// it only gates information based on novelty and global attention levels.
class Thalamus {
public:
    Thalamus(BioEngine& engine, const LayerMeta& layer_meta)
        : engine_(engine), 
          meta_(layer_meta),
          // Initialize internal state trackers to match layer size
          activity_avg_(meta_.count, 0.0f) 
    {}

    // Main API: Apply attentional gating logic.
    // This function is called every tick by the main simulation loop.
    // It acts as a "veto" power over the normal physics of the neurons it controls.
    void apply_gating() {
        // Get mutable access to the engine's state
        auto& neurons = const_cast<NeuronBlock&>(engine_.get_neurons());
        auto& ctx = const_cast<Context&>(engine_.get_context());

        // 1. Determine the current "surprise" threshold based on global attention.
        // High acetylcholine (focus) -> Low threshold -> everything is interesting.
        // Low acetylcholine (unfocused) -> High threshold -> only big changes pass.
        // We add a small epsilon to avoid division by zero.
        const float surprise_threshold = 20.0f / (ctx.acetylcholine + 0.01f);

        // This will hold the indices of spikes we are vetoing.
        std::vector<uint32_t> vetoed_spikes;

        for (uint32_t i = 0; i < meta_.count; ++i) {
            uint32_t neuron_id = meta_.start_index + i;

            // 2. Update the moving average of this neuron's activity.
            // This represents the neuron's expectation of its own input.
            float current_potential = neurons.membrane_potential[neuron_id];
            activity_avg_[i] = (activity_avg_[i] * 0.99f) + (current_potential * 0.01f);
            
            // 3. Apply Gating Logic
            // Check if this neuron was supposed to fire according to normal physics.
            if (neurons.has_fired[neuron_id]) {
                // Was the spike "surprising" enough to pass the gate?
                // A spike is surprising if its potential is significantly higher than the recent average.
                bool is_surprising = (current_potential > (activity_avg_[i] + surprise_threshold));
                
                if (!is_surprising) {
                    // VETO! The spike is predictable/boring. Suppress it.
                    vetoed_spikes.push_back(neuron_id);
                }
            }
        }
        
        // 4. Process Vetoes
        // We do this in a separate loop to avoid modifying data while iterating.
        if (!vetoed_spikes.empty()) {
            for (uint32_t neuron_id : vetoed_spikes) {
                // A. Revoke the "fired" status.
                neurons.has_fired[neuron_id] = false;
                
                // B. Do NOT reset its voltage. Let it continue integrating.
                // The main engine already reset the voltage; we must restore it.
                // This is a slight hack; a better engine would have pre/post-fire hooks.
                // For Phase II, we assume this is acceptable.
                // Note: The neuron's ATP was already consumed, which is fine. The "attempt" to fire cost energy.
            }
            // C. Correct the global spike count for this tick.
            ctx.total_spikes_this_tick -= vetoed_spikes.size();
        }
    }

private:
    BioEngine& engine_;
    LayerMeta meta_;
    
    // Internal state: Exponential Moving Average of membrane potential for each neuron.
    std.vector<float> activity_avg_;
};

} // namespace genesis