#include "bio_engine.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <numeric>

using namespace genesis;

// -----------------------------------------------------------------------------
// Simulation Configuration
// -----------------------------------------------------------------------------
constexpr size_t NEURON_COUNT     = 1000;
constexpr size_t DENSITY          = 100; // Synapses per neuron
constexpr uint64_t RNG_SEED       = 123456789;
constexpr int    SIM_TICKS        = 10000; // 10 seconds of biological time
constexpr int    LOG_INTERVAL     = 100;   // Print status every 100 ticks

int main() {
    std::cout << "[Genesis] Initializing Bio-Digital Substrate..." << std::endl;
    std::cout << "  Neurons: " << NEURON_COUNT << std::endl;
    std::cout << "  Synapses: ~" << NEURON_COUNT * DENSITY << std::endl;

    // 1. Initialize Engine
    BioEngine engine;
    engine.init(NEURON_COUNT, DENSITY, RNG_SEED);

    std::cout << "[Genesis] Network ready. Starting Simulation (" << SIM_TICKS << " ms)..." << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Simulation Loop
    for (int t = 0; t < SIM_TICKS; ++t) {
        
        // --- A. Dynamic Stimulus Protocol ---
        
        // Input: Stimulate Neuron 0-9 every 200ms (5Hz pulse)
        if (t % 200 == 0) {
            for (uint32_t i = 0; i < 10; ++i) {
                engine.inject_stimulus(i, 20.0f); // Strong input
            }
        }

        // Reward: Inject Dopamine bursts between tick 2000 and 4000
        // Simulating a "learning event" where the input pattern is rewarded
        if (t == 2000) {
            std::cout << "  >>> Dopamine Injection STARTED <<<" << std::endl;
            engine.set_dopamine(1.0f); // High reward
        } else if (t == 4000) {
            std::cout << "  >>> Dopamine Injection STOPPED <<<" << std::endl;
            engine.set_dopamine(0.0f); // Baseline
        }

        // --- B. Physics Step ---
        engine.tick();

        // --- C. Telemetry & Logging ---
        
        if (t % LOG_INTERVAL == 0) {
            const auto& ctx = engine.get_context();
            const auto& neurons = engine.get_neurons();

            // Calculate Global Average ATP for health check
            // (Note: In a huge sim, we might estimate this rather than sum all)
            double total_atp = 0.0;
            for (float atp : neurons.atp_level) total_atp += atp;
            double avg_atp = total_atp / neurons.atp_level.size();

            std::cout << "Tick: " << std::setw(6) << t 
                      << " | Spikes: " << std::setw(4) << ctx.total_spikes_this_tick
                      << " | DA: " << std::fixed << std::setprecision(2) << ctx.dopamine
                      << " | Avg ATP: " << avg_atp
                      << std::endl;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // 3. Performance Report
    double real_time_sec = elapsed.count();
    double biological_time_sec = SIM_TICKS / 1000.0;
    double speedup = biological_time_sec / real_time_sec;

    std::cout << "[Genesis] Simulation Complete." << std::endl;
    std::cout << "  Real Time: " << real_time_sec << "s" << std::endl;
    std::cout << "  Bio Time:  " << biological_time_sec << "s" << std::endl;
    std::cout << "  Speedup:   " << speedup << "x faster than real-time" << std::endl;
    std::cout << "  Ticks/Sec: " << (SIM_TICKS / real_time_sec) << std::endl;

    return 0;
}