#pragma once

#include "constants.h"
#include "types.h"
#include "utils.h"

#include <vector>
#include <memory>

namespace genesis {

class BioEngine {
public:
    BioEngine() = default;
    ~BioEngine() = default;

    // -------------------------------------------------------------------------
    // API: Lifecycle & Control
    // -------------------------------------------------------------------------

    // Initialize the engine with a specific network size and connectivity density.
    // Pre-allocates all memory vectors to prevent resizing during simulation.
    // seed: For deterministic RNG.
    void init(size_t neuron_count, size_t synapse_density_per_neuron, uint64_t seed);

    // The Main Physics Loop.
    // Executes exactly one millisecond of biological time.
    // 1. Decays chemicals
    // 2. Refills energy (ATP)
    // 3. Integrates inputs (Voltage)
    // 4. Fires Neurons (Spikes)
    // 5. Updates Plasticity (STDP + Dopamine)
    void tick();

    // -------------------------------------------------------------------------
    // API: Interaction & Inspection
    // -------------------------------------------------------------------------

    // Inject current into a specific neuron (simulating sensory input).
    // current_mv: Amount of voltage to add directly to membrane potential.
    void inject_stimulus(uint32_t neuron_id, float current_mv);

    // Set the global dopamine level (Reward signal).
    void set_dopamine(float level);

    // Read-only access to internal state (for Logging/Visualization)
    const NeuronBlock& get_neurons() const { return neurons_; }
    const SynapseBlock& get_synapses() const { return synapses_; }
    const Context& get_context() const { return ctx_; }

private:
    // -------------------------------------------------------------------------
    // Internal Physics Modules
    // -------------------------------------------------------------------------

    // Step 1: Metabolic Regulator
    // Refills ATP and clamps to max.
    void update_metabolism();

    // Step 2: Synaptic Dynamics (Plasticity)
    // Decays eligibility traces. Applies Dopamine-gated weight updates.
    void update_synapses();

    // Step 3: Membrane Integration
    // Sums inputs from firing synapses into post-synaptic neurons.
    // Handles Refractory gating.
    // Returns a list of neurons that fired this tick.
    std::vector<uint32_t> integrate_and_fire();

    // Step 4: Spike Propagation & STDP Event
    // Processes the firing events:
    // - Deducts ATP cost
    // - Updates Pre/Post STDP traces
    // - Resets Membrane Potential
    void propagate_spikes(const std::vector<uint32_t>& firing_indices);

    // -------------------------------------------------------------------------
    // Data
    // -------------------------------------------------------------------------
    NeuronBlock   neurons_;
    SynapseBlock  synapses_;
    TopologyIndex topology_;
    Context       ctx_;
    
    // Internal RNG for connectivity and noise
    std::unique_ptr<utils::FastRNG> rng_;
    
    size_t neuron_count_ = 0;
};

} // namespace genesis