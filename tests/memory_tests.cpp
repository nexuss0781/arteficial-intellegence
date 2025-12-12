#include <gtest/gtest.h>
#include "bio_engine.h"
#include <vector>

using namespace genesis;

class MemorySystemTest : public ::testing::Test {
protected:
    BioEngine engine;
    
    void SetUp() override {
        // Initialize basic structure
        engine.init_structured(100, 1000, 42); 
    }
};

// -----------------------------------------------------------------------------
// Test 1: Differential Plasticity (Fast vs Slow Learning)
// -----------------------------------------------------------------------------
TEST_F(MemorySystemTest, DifferentialPlasticity) {
    // 1. Setup Architecture
    // Input Layer connected to two downstream layers
    uint32_t input_start = engine.create_layer("Input", 1, LAYER_ID_INPUT);
    
    // "Slow" Cortex (Scale = 1.0)
    uint32_t slow_start = engine.create_layer("Cortex", 1, LAYER_ID_CORTEX, PLASTICITY_SCALE_CORTEX);
    
    // "Fast" Hippocampus (Scale = 10.0)
    uint32_t fast_start = engine.create_layer("Hippocampus", 1, LAYER_ID_HIPPOCAMPUS, PLASTICITY_SCALE_HIPPOCAMPUS);

    // Connect Input -> Slow
    engine.connect_full("Input", "Cortex", 0.1f, 1);
    // Connect Input -> Fast
    engine.connect_full("Input", "Hippocampus", 0.1f, 1);
    
    engine.bake_topology();

    // 2. Training Protocol
    // We want to verify that for the SAME input pattern, the Fast layer learns MORE.
    engine.set_dopamine(1.0f); // Turn on learning
    
    for(int i=0; i<20; ++i) {
        // Fire Pre (Input)
        engine.inject_stimulus(input_start, 50.0f);
        engine.tick();
        
        // Force Fire Post (Target layers) to induce LTP
        engine.inject_stimulus(slow_start, 50.0f);
        engine.inject_stimulus(fast_start, 50.0f);
        engine.tick();
    }

    // 3. Inspect Results
    const auto& synapses = engine.get_synapses();
    float w_slow = 0.0f;
    float w_fast = 0.0f;
    
    // Find the weights (linear scan is fine for test)
    for(size_t i=0; i<synapses.weights.size(); ++i) {
        if (synapses.post_indices[i] == slow_start) w_slow = synapses.weights[i];
        if (synapses.post_indices[i] == fast_start) w_fast = synapses.weights[i];
    }

    // 4. Assertions
    // The Hippocampal weight should have grown significantly larger than the Cortical weight
    // due to the plasticity_scale multiplier.
    EXPECT_GT(w_fast, w_slow) << "Hippocampus (Fast) synapse should strengthen more than Cortex (Slow)";
    
    // Check for significant growth (started at ~0.05, added ~0.1 * 10 * 20 times roughly)
    EXPECT_GT(w_fast, 0.2f) << "Fast synapse should show significant growth from baseline";
}

// -----------------------------------------------------------------------------
// Test 2: Pattern Completion (Attractor Dynamics)
// -----------------------------------------------------------------------------
TEST_F(MemorySystemTest, PatternCompletion) {
    // 1. Setup Recurrent Layer
    // Two neurons connected to each other: N0 <-> N1
    uint32_t mem_start = engine.create_layer("Memory", 2, LAYER_ID_HIPPOCAMPUS, 10.0f);
    
    // Fully connected recurrent layer (density 1.0)
    engine.connect_recurrent("Memory", 0.5f, 1, 1.0f); 
    engine.bake_topology();

    // 2. Training Phase: "Bind" the association
    // Force both to fire together in the presence of Dopamine
    engine.set_dopamine(1.0f);
    for(int i=0; i<50; ++i) {
        engine.inject_stimulus(mem_start, 50.0f);     // Fire N0
        engine.inject_stimulus(mem_start + 1, 50.0f); // Fire N1
        engine.tick();
    }

    // 3. Testing Phase: Recall
    engine.set_dopamine(0.0f); // Stop learning
    
    // Let network settle
    for(int i=0; i<10; ++i) engine.tick();

    // Stimulate ONLY N0 (The "Partial Cue")
    engine.inject_stimulus(mem_start, 50.0f);
    engine.tick(); 
    
    // Tick forward to allow signal propagation N0 -> N1
    engine.tick(); 

    // 4. Assertions
    const auto& neurons = engine.get_neurons();
    
    // N1 should fire even though it received no external stimulus,
    // driven solely by the strengthened connection from N0.
    EXPECT_TRUE(neurons.has_fired[mem_start + 1]) 
        << "N1 should fire via pattern completion from N0";
}