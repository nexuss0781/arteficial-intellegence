#pragma once

#include "constants.h"
#include <vector>
#include <cstdint>
#include <cstddef>
#include <deque>

namespace genesis {

// -----------------------------------------------------------------------------
// Phase III: Engine State Machine
// -----------------------------------------------------------------------------
enum class EngineState {
    AWAKE, // Normal sensory processing and learning
    SLEEP  // Sensory gating, spontaneous replay, consolidation
};

// -----------------------------------------------------------------------------
// 1. Data-Oriented Design: Struct-of-Arrays (SoA)
// -----------------------------------------------------------------------------
struct alignas(64) NeuronBlock {
    // Phase I: Physics State
    std::vector<float>   membrane_potential;   // V (mV)
    std::vector<float>   recovery_variable;    // u
    std::vector<float>   atp_level;            // Energy
    
    // Phase I: Discrete State
    std::vector<int32_t> refractory_timer;
    
    // Phase I: Event State
    std::vector<bool>    has_fired;
    std::vector<uint64_t> last_spike_time;
    
    // Phase I: Homeostasis
    std::vector<float>   avg_firing_rate;

    // Phase II: Space & Structure
    std::vector<float>   pos_x;
    std::vector<float>   pos_y;
    std::vector<uint8_t> layer_id;

    // --- PHASE III EXTENSIONS: MEMORY ---
    // Plasticity Scalar: How "fast" does this neuron learn?
    // Hippocampus neurons will have high values (e.g., 10.0) for one-shot learning.
    // Cortex neurons will have low values (e.g., 1.0) for slow statistical integration.
    std::vector<float>   plasticity_scale;

    void resize(size_t n) {
        membrane_potential.resize(n);
        recovery_variable.resize(n);
        atp_level.resize(n);
        refractory_timer.resize(n);
        has_fired.resize(n);
        last_spike_time.resize(n);
        avg_firing_rate.resize(n);
        
        pos_x.resize(n);
        pos_y.resize(n);
        layer_id.resize(n);
        
        // Phase III
        plasticity_scale.resize(n);
    }
};

struct alignas(64) SynapseBlock {
    // Connectivity
    std::vector<uint32_t> pre_indices;
    std::vector<uint32_t> post_indices;
    
    // Dynamics
    std::vector<float>    weights;
    std::vector<float>    eligibility_traces;
    std::vector<bool>     is_inhibitory;

    // Phase II: Time
    std::vector<uint8_t>  delays;

    void resize(size_t n) {
        pre_indices.resize(n);
        post_indices.resize(n);
        weights.resize(n);
        eligibility_traces.resize(n);
        is_inhibitory.resize(n);
        delays.resize(n);
    }
};

// -----------------------------------------------------------------------------
// 2. Topology Acceleration (CSR)
// -----------------------------------------------------------------------------
struct TopologyIndex {
    std::vector<uint32_t> outgoing_start;
    std::vector<uint32_t> outgoing_count;

    void resize(size_t n) {
        outgoing_start.resize(n);
        outgoing_count.resize(n);
    }
};

// -----------------------------------------------------------------------------
// 3. Temporal Integration Buffer
// -----------------------------------------------------------------------------
struct PendingSpike {
    uint32_t target_neuron_id;
    float    signal_strength; 
    uint8_t  ticks_remaining;
};

using SpikeDelayQueue = std::deque<PendingSpike>;

// -----------------------------------------------------------------------------
// 4. Global Context
// -----------------------------------------------------------------------------
struct Context {
    uint64_t current_tick = 0;
    uint64_t rng_seed = 0;

    float dopamine      = 0.0f;
    float acetylcholine = 1.0f;
    
    // Phase III: Current Brain State
    EngineState state = EngineState::AWAKE;
    
    uint64_t total_spikes_this_tick = 0;
};

} // namespace genesis