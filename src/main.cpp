#include "bio_engine.h"
#include "sensory/input_layer.h"
#include "sensory/cortex_layer.h"
#include "sensory/thalamus.h"
#include "sensory/spatial_utils.h"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace genesis;

// -----------------------------------------------------------------------------
// Simulation Configuration
// -----------------------------------------------------------------------------
constexpr size_t CORTEX_WIDTH     = 32;
constexpr size_t CORTEX_HEIGHT    = 32;
constexpr size_t CORTEX_SIZE      = CORTEX_WIDTH * CORTEX_HEIGHT;

constexpr size_t TOTAL_NEURONS    = INPUT_SIZE + CORTEX_SIZE; // Input + Cortex (Thalamus is virtual for now)
constexpr size_t MAX_SYNAPSES_EST = TOTAL_NEURONS * 100; // Heuristic max

constexpr uint64_t RNG_SEED       = 2024;
constexpr int    SIM_TICKS        = 2000; // 2 seconds of biological time
constexpr int    LOG_INTERVAL     = 100;

// Helper to create a simple visual stimulus
std::vector<float> create_vertical_line_stimulus() {
    std::vector<float> data(INPUT_SIZE, 0.0f);
    int center_x = INPUT_WIDTH / 2;
    for (int y = 5; y < INPUT_HEIGHT - 5; ++y) {
        data[y * INPUT_WIDTH + center_x] = 1.0f;
        data[y * INPUT_WIDTH + center_x - 1] = 0.5f; // Softer edges
        data[y * INPUT_WIDTH + center_x + 1] = 0.5f;
    }
    return data;
}

int main() {
    std::cout << "[Genesis] Phase II: Sensory Cortex Simulation" << std::endl;

    // 1. Initialize Engine in Structured Mode
    BioEngine engine;
    engine.init_structured(TOTAL_NEURONS, MAX_SYNAPSES_EST, RNG_SEED);

    // 2. Create Functional Layers
    std::cout << "  > Creating Layers..." << std::endl;
    uint32_t input_start  = engine.create_layer("Input", INPUT_SIZE, 0);
    uint32_t cortex_start = engine.create_layer("Cortex", CORTEX_SIZE, 2);

    // Assign spatial positions to neurons for visualization and connectivity
    auto& neurons = const_cast<NeuronBlock&>(engine.get_neurons());
    for(uint32_t i=0; i<INPUT_SIZE; ++i) {
        neurons.pos_x[input_start + i] = static_cast<float>(i % INPUT_WIDTH);
        neurons.pos_y[input_start + i] = static_cast<float>(i / INPUT_WIDTH);
    }
    for(uint32_t i=0; i<CORTEX_SIZE; ++i) {
        neurons.pos_x[cortex_start + i] = static_cast<float>(i % CORTEX_WIDTH);
        neurons.pos_y[cortex_start + i] = static_cast<float>(i / CORTEX_WIDTH);
    }

    // 3. Connect Layers with Spatial Logic
    std::cout << "  > Wiring Retina to Cortex..." << std::endl;
    // NOTE: In a full implementation, this logic would be in a dedicated
    // TopologyBuilder class. Here in main for clarity.
    auto& synapses = const_cast<SynapseBlock&>(engine.get_synapses());
    size_t synapse_cursor = 0; // Simplified cursor management
    utils::FastRNG rng(RNG_SEED);

    const auto& input_meta = engine.get_layers().at("Input");
    const auto& cortex_meta = engine.get_layers().at("Cortex");

    spatial_utils::connect_layers_spatial(input_meta, neurons, cortex_meta, 
                                          synapses, synapse_cursor, rng);

    // Finalize the connection graph for fast lookups
    engine.bake_topology();
    std::cout << "  > " << synapse_cursor << " synapses created." << std::endl;

    // 4. Instantiate Sensory Modules
    InputLayer input_layer(engine, input_meta);
    CortexLayer cortex_layer(engine, cortex_meta);
    // The Thalamus virtually gates the Input layer for this simulation
    Thalamus thalamus(engine, input_meta); 

    // 5. Prepare Stimulus
    auto stimulus = create_vertical_line_stimulus();
    
    std::cout << "[Genesis] Network ready. Starting Simulation..." << std::endl;
    
    // 6. Simulation Loop
    for (int t = 0; t < SIM_TICKS; ++t) {
        // A. Present data to the "retina"
        input_layer.present_data(stimulus, 100.0f); // 100 Hz max firing rate

        // B. Run core physics (voltage integration, STDP, etc.)
        engine.tick();
        
        // C. Apply Thalamic Gating (Filter out predictable input)
        // Note: The thalamus is gating the *input* neurons' ability to propagate.
        // A full model would have a separate thalamic layer.
        thalamus.apply_gating();
        
        // D. Apply Cortical Dynamics (Winner-Take-All)
        cortex_layer.apply_lateral_inhibition();
        
        // E. Logging
        if (t % LOG_INTERVAL == 0) {
            const auto& ctx = engine.get_context();
            // Count spikes per layer
            uint32_t input_spikes = 0;
            uint32_t cortex_spikes = 0;
            for (uint32_t i=0; i<TOTAL_NEURONS; ++i) {
                if (neurons.has_fired[i]) {
                    if (neurons.layer_id[i] == 0) input_spikes++;
                    else if (neurons.layer_id[i] == 2) cortex_spikes++;
                }
            }

            std::cout << "Tick: " << std::setw(6) << t
                      << " | Input Spikes: " << std::setw(3) << input_spikes
                      << " | Cortex Spikes: " << std::setw(3) << cortex_spikes
                      << " | DA: " << std::fixed << std::setprecision(2) << ctx.dopamine
                      << std::endl;
        }
    }

    std::cout << "[Genesis] Simulation Complete." << std::endl;

    return 0;
}