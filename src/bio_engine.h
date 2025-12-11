#pragma once

#include "constants.h"
#include "types.h"
#include "utils.h"

#include <vector>
#include <memory>
#include <string>
#include <map>

namespace genesis {

// Helper struct to track functional regions in the flat neuron array
struct LayerMeta {
    uint32_t start_index;
    uint32_t count;
    uint8_t  layer_id;
};

class BioEngine {
public:
    BioEngine() = default;
    ~BioEngine() = default;

    // -------------------------------------------------------------------------
    // API: Lifecycle & Construction
    // -------------------------------------------------------------------------

    // Phase I (Legacy/Test): Initialize with random topology.
    // Keeps the old tests working.
    void init(size_t neuron_count, size_t synapse_density, uint64_t seed);

    // Phase II (Structured): Initialize empty memory arena.
    // Call create_layer() and connect_layers() afterwards.
    void init_structured(size_t max_neurons, size_t max_synapses, uint64_t seed);

    // Phase II: Define a functional region (e.g., "Retina", "V1").
    // Returns the starting index of the layer in the global neuron array.
    uint32_t create_layer(const std::string& name, size_t count, uint8_t layer_type);

    // Phase II: Connect two layers.
    // This allows specific projection logic (Random, Gaussian, etc.) to be implemented
    // in the source file or via helper builders.
    // For now, exposes a generic connector for the engine to manage indexing.
    // Returns number of synapses created.
    size_t connect_full(const std::string& pre_name, const std::string& post_name, 
                        float weight_scale, uint8_t delay_ticks);

    // Finalize topology acceleration structures after manually adding connections.
    void bake_topology();

    // -------------------------------------------------------------------------
    // API: Simulation
    // -------------------------------------------------------------------------

    // The Main Physics Loop.
    // Now includes:
    // 5. Process Delayed Spikes (Axonal transmission)
    void tick();

    // -------------------------------------------------------------------------
    // API: Interaction
    // -------------------------------------------------------------------------
    void inject_stimulus(uint32_t neuron_id, float current_mv);
    void set_dopamine(float level);

    const NeuronBlock& get_neurons() const { return neurons_; }
    const SynapseBlock& get_synapses() const { return synapses_; }
    const Context& get_context() const { return ctx_; }
    
    // Access layer info for visualization
    const std::map<std::string, LayerMeta>& get_layers() const { return layers_; }

private:
    // -------------------------------------------------------------------------
    // Physics Modules
    // -------------------------------------------------------------------------
    void update_metabolism();
    void update_synapses();
    std::vector<uint32_t> integrate_and_fire();
    
    // Phase II Update: Pushes events to DelayQueue instead of immediate write
    void propagate_spikes(const std::vector<uint32_t>& firing_indices);
    
    // Phase II New: Consumes DelayQueue and updates Post-Neuron Voltage
    void process_delayed_spikes();

    // -------------------------------------------------------------------------
    // Data
    // -------------------------------------------------------------------------
    NeuronBlock   neurons_;
    SynapseBlock  synapses_;
    TopologyIndex topology_;
    Context       ctx_;
    
    // Phase II: Temporal Buffer (Axonal Transit)
    SpikeDelayQueue delay_queue_;

    // Phase II: Structural Metadata
    std::map<std::string, LayerMeta> layers_;
    size_t neuron_cursor_ = 0;  // Tracks allocation in flat array
    size_t synapse_cursor_ = 0;
    
    std::unique_ptr<utils::FastRNG> rng_;
    size_t neuron_count_ = 0;
};

} // namespace genesis