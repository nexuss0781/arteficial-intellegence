#pragma once

#include "constants.h"
#include <vector>
#include <cstdint>
#include <cstddef>
#include <deque>

namespace genesis {

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

    // --- PHASE II EXTENSIONS: SPACE & STRUCTURE ---
    // Spatial Topology (for Retinotopy and Lateral Inhibition)
    std::vector<float>   pos_x;                // Normalized [0.0, 1.0] or Grid Index
    std::vector<float>   pos_y;
    
    // Layer Identification (0=Input, 1=Thalamus, 2=Cortex, etc.)
    // Allows specific logic per region (e.g., Input layer doesn't integrate, it transduces)
    std::vector<uint8_t> layer_id;

    void resize(size_t n) {
        membrane_potential.resize(n);
        recovery_variable.resize(n);
        atp_level.resize(n);
        refractory_timer.resize(n);
        has_fired.resize(n);
        last_spike_time.resize(n);
        avg_firing_rate.resize(n);
        
        // Phase II
        pos_x.resize(n);
        pos_y.resize(n);
        layer_id.resize(n);
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

    // --- PHASE II EXTENSIONS: TIME ---
    // Axonal Delay: How many ticks before the signal hits the post-neuron?
    // Essential for sequence detection (A->B vs B->A).
    std::vector<uint8_t>  delays;

    void resize(size_t n) {
        pre_indices.resize(n);
        post_indices.resize(n);
        weights.resize(n);
        eligibility_traces.resize(n);
        is_inhibitory.resize(n);
        
        // Phase II
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
// 3. Temporal Integration Buffer (Phase II NEW)
// -----------------------------------------------------------------------------
// Represents a "spike in flight".
struct PendingSpike {
    uint32_t target_neuron_id;
    float    signal_strength; // Weight * Gain (+/-)
    uint8_t  ticks_remaining; // Countdown
};

// A ring buffer or bucket queue could optimize this, but a deque is sufficient 
// for Phase II prototyping.
using SpikeDelayQueue = std::deque<PendingSpike>;

// -----------------------------------------------------------------------------
// 4. Global Context
// -----------------------------------------------------------------------------
struct Context {
    uint64_t current_tick = 0;
    uint64_t rng_seed = 0;

    float dopamine      = 0.0f;
    float acetylcholine = 1.0f;
    
    uint64_t total_spikes_this_tick = 0;
};

} // namespace genesis