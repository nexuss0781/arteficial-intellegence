#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace genesis {

// -----------------------------------------------------------------------------
// 1. Data-Oriented Design: Struct-of-Arrays (SoA)
// -----------------------------------------------------------------------------
// We use SoA layout to maximize CPU cache utilization. 
// When the engine iterates over neurons, it streams contiguous floats, 
// allowing the CPU prefetcher to keep the pipeline full.

// Aligning to 64 bytes ensures arrays start on cache line boundaries,
// vital for SIMD vectorization (AVX2/AVX-512).
struct alignas(64) NeuronBlock {
    // Physics State
    std::vector<float>   membrane_potential;   // V (mV)
    std::vector<float>   recovery_variable;    // u (for adaptation/dynamics)
    std::vector<float>   atp_level;            // Energy (0.0 - 1.0)
    
    // Discrete State
    std::vector<int32_t> refractory_timer;     // Ticks remaining until ready
    
    // Event State
    std::vector<bool>    has_fired;            // Did it fire this tick?
    std::vector<uint64_t> last_spike_time;     // Tick count of last spike
    
    // Homeostasis
    std::vector<float>   avg_firing_rate;      // Moving average for scaling

    // Pre-allocate memory to avoid reallocations during runtime
    void resize(size_t n) {
        membrane_potential.resize(n);
        recovery_variable.resize(n);
        atp_level.resize(n);
        refractory_timer.resize(n);
        has_fired.resize(n);
        last_spike_time.resize(n);
        avg_firing_rate.resize(n);
    }
};

struct alignas(64) SynapseBlock {
    // Connectivity (Adjacency)
    std::vector<uint32_t> pre_indices;         // Source Neuron ID
    std::vector<uint32_t> post_indices;        // Destination Neuron ID
    
    // Dynamics
    std::vector<float>    weights;             // W (Conductance strength)
    std::vector<float>    eligibility_traces;  // E (For delayed reward)
    
    // Properties
    std::vector<bool>     is_inhibitory;       // True = GABA, False = Glutamate

    void resize(size_t n) {
        pre_indices.resize(n);
        post_indices.resize(n);
        weights.resize(n);
        eligibility_traces.resize(n);
        is_inhibitory.resize(n);
    }
};

// -----------------------------------------------------------------------------
// 2. Topology Acceleration: Compressed Sparse Row (CSR) Style
// -----------------------------------------------------------------------------
// Instead of searching the synapse array for a neuron's connections,
// we pre-calculate the start index and count. 
// Iteration becomes: for(i = start[n]; i < start[n] + count[n]; i++)
struct TopologyIndex {
    std::vector<uint32_t> outgoing_start;      // Index in SynapseBlock where connections begin
    std::vector<uint32_t> outgoing_count;      // How many connections this neuron has

    void resize(size_t n) {
        outgoing_start.resize(n);
        outgoing_count.resize(n);
    }
};

// -----------------------------------------------------------------------------
// 3. Global Context (Neuromodulators & Time)
// -----------------------------------------------------------------------------
struct Context {
    uint64_t current_tick = 0;
    uint64_t rng_seed = 0;

    // The Chemical Soup
    float dopamine      = 0.0f;  // Reward signal (Gates plasticity)
    float acetylcholine = 1.0f;  // Attention/Gain (Default 1.0)
    
    // Statistics
    uint64_t total_spikes_this_tick = 0;
};

} // namespace genesis