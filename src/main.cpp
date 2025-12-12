#include "bio_engine.h"
#include "sensory/input_layer.h"
#include "sensory/cortex_layer.h"
#include "sensory/thalamus.h"
#include "sensory/spatial_utils.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono> // For performance report

// --- NOTE: Assumes a simple setter for acetylcholine exists in BioEngine ---
/*
In bio_engine.h:
public:
    void set_acetylcholine(float level);

In bio_engine.cpp:
void BioEngine::set_acetylcholine(float level) {
    ctx_.acetylcholine = level;
}
*/

using namespace genesis;

// -----------------------------------------------------------------------------
// Simulation Configuration
// -----------------------------------------------------------------------------
constexpr size_t CORTEX_WIDTH     = 32;
constexpr size_t CORTEX_HEIGHT    = 32;
constexpr size_t CORTEX_SIZE      = CORTEX_WIDTH * CORTEX_HEIGHT;

constexpr size_t TOTAL_NEURONS    = INPUT_SIZE + CORTEX_SIZE;
constexpr size_t MAX_SYNAPSES_EST = TOTAL_NEURONS * 100;

constexpr uint64_t RNG_SEED       = 2024;
constexpr int    SIM_TICKS        = 5000;
constexpr int    LOG_INTERVAL     = 100;

constexpr int PULSE_ON_DURATION_MS  = 200;
constexpr int PULSE_OFF_DURATION_MS = 200;
constexpr int PULSE_CYCLE_MS        = PULSE_ON_DURATION_MS + PULSE_OFF_DURATION_MS;

std::vector<float> create_vertical_line_stimulus() {
    std::vector<float> data(INPUT_SIZE, 0.0f);
    int center_x = INPUT_WIDTH / 2;
    for (size_t y = 5; y < INPUT_HEIGHT - 5; ++y) {
        data[y * INPUT_WIDTH + center_x] = 1.0f;
    }
    return data;
}

int main() {
    std::cout << "[Genesis] Phase II: Sensory Cortex Simulation (Corrected Protocol)" << std::endl;

    BioEngine engine;
    engine.init_structured(TOTAL_NEURONS, MAX_SYNAPSES_EST, RNG_SEED);

    std::cout << "  > Creating Layers..." << std::endl;
    uint32_t input_start  = engine.create_layer("Input", INPUT_SIZE, 0);
    uint32_t cortex_start = engine.create_layer("Cortex", CORTEX_SIZE, 2);

    auto& neurons = const_cast<NeuronBlock&>(engine.get_neurons());
    for(uint32_t i=0; i<INPUT_SIZE; ++i) {
        neurons.pos_x[input_start + i] = static_cast<float>(i % INPUT_WIDTH);
        neurons.pos_y[input_start + i] = static_cast<float>(i / INPUT_WIDTH);
    }
    for(uint32_t i=0; i<CORTEX_SIZE; ++i) {
        neurons.pos_x[cortex_start + i] = static_cast<float>(i % CORTEX_WIDTH);
        neurons.pos_y[cortex_start + i] = static_cast<float>(i / CORTEX_WIDTH);
    }

    std::cout << "  > Wiring Retina to Cortex..." << std::endl;
    auto& synapses = const_cast<SynapseBlock&>(engine.get_synapses());
    size_t synapse_cursor = 0;
    utils::FastRNG rng(RNG_SEED);
    const auto& input_meta = engine.get_layers().at("Input");
    const auto& cortex_meta = engine.get_layers().at("Cortex");
    spatial_utils::connect_layers_spatial(input_meta, neurons, cortex_meta, synapses, synapse_cursor, rng)
    engine.set_synapse_cursor(synapse_cursor);
    engine.bake_topology();
    std::cout << "  > " << engine.get_synapse_cursor() << " synapses created." << std::endl;

    InputLayer input_layer(engine, input_meta);
    CortexLayer cortex_layer(engine, cortex_meta);
    Thalamus thalamus(engine, input_meta);

    auto stimulus = create_vertical_line_stimulus();
    
    std::cout << "[Genesis] Network ready. Starting Simulation..." << std::endl;
    
    // --- FIX: Add Performance Timers ---
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < SIM_TICKS; ++t) {
        
        bool is_stimulus_on = (t % PULSE_CYCLE_MS) < PULSE_ON_DURATION_MS;

        if (is_stimulus_on) {
            input_layer.present_data(stimulus, 100.0f);
            // engine.set_acetylcholine(2.0f);
        } else {
            // engine.set_acetylcholine(1.0f);
        }
        
        if (t == 2000) engine.set_dopamine(1.0f);
        if (t == 4000) engine.set_dopamine(0.0f);

        // --- FIX: Correct Order of Operations ---
        // 1. Thalamus gates the potential *before* the physics tick.
        //    (Requires thalamus.h logic to check voltage, not has_fired flag)
        thalamus.apply_gating(); 
        
        // 2. The main physics engine integrates inputs and fires neurons.
        engine.tick(); 
        
        // 3. Cortex applies feedback inhibition *after* neurons have fired.
        cortex_layer.apply_lateral_inhibition();
        
        if (t % LOG_INTERVAL == 0) {
            const auto& ctx = engine.get_context();
            uint32_t input_spikes = 0;
            uint32_t cortex_spikes = 0;
            size_t total_neurons_in_use = engine.get_layers().at("Input").count + engine.get_layers().at("Cortex").count;
            for (uint32_t i=0; i < total_neurons_in_use; ++i) {
                if(i >= TOTAL_NEURONS) continue; // Safety
                if (neurons.has_fired[i]) {
                    if (neurons.layer_id[i] == 0) input_spikes++;
                    else if (neurons.layer_id[i] == 2) cortex_spikes++;
                }
            }
            std::cout << "Tick: " << std::setw(6) << t
                      << " | Input Spikes: " << std::setw(3) << input_spikes
                      << " | Cortex Spikes: " << std::setw(3) << cortex_spikes
                      << " | DA: " << std::fixed << std::setprecision(2) << ctx.dopamine
                      << " | ACh: " << ctx.acetylcholine
                      << std::endl;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "[Genesis] Simulation Complete." << std::endl;

    // --- FIX: Re-add Performance Report ---
    std::chrono::duration<double> elapsed = end_time - start_time;
    double real_time_sec = elapsed.count();
    double biological_time_sec = SIM_TICKS / 1000.0;
    double speedup = biological_time_sec / real_time_sec;

    std::cout << "  Real Time: " << real_time_sec << "s" << std::endl;
    std::cout << "  Bio Time:  " << biological_time_sec << "s" << std::endl;
    std::cout << "  Speedup:   " << speedup << "x faster than real-time" << std::endl;
    std::cout << "  Ticks/Sec: " << (SIM_TICKS / real_time_sec) << std::endl;

    return 0;
}