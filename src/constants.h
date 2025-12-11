#pragma once

#include <cstddef> // for size_t

namespace genesis {

// -----------------------------------------------------------------------------
// 1. Simulation Time Base
// -----------------------------------------------------------------------------
constexpr float DT_MS = 1.0f;


// -----------------------------------------------------------------------------
// 2. Membrane Dynamics (Phase I Retained)
// -----------------------------------------------------------------------------
constexpr float V_REST_MV       = -70.0f;
constexpr float V_RESET_MV      = -70.0f;
constexpr float V_THRESH_MV     = -55.0f;

constexpr float MEMBRANE_TAU_MS = 20.0f;
constexpr float VOLTAGE_DECAY_FACTOR = 0.951229f; // exp(-1.0 / 20.0)

constexpr int   REFRACTORY_TICKS     = 5;


// -----------------------------------------------------------------------------
// 3. Metabolic Constraints (Phase I Retained)
// -----------------------------------------------------------------------------
constexpr float ATP_MAX         = 1.0f;
constexpr float ATP_REFILL_RATE = 0.01f; 
constexpr float SPIKE_COST      = 0.10f; 
constexpr float ATP_MIN_TO_FIRE = 0.10f;


// -----------------------------------------------------------------------------
// 4. Synaptic Plasticity (Phase I Retained)
// -----------------------------------------------------------------------------
constexpr float STDP_WINDOW_MS  = 20.0f;
constexpr float A_PLUS          = 0.10f; 
constexpr float A_MINUS         = 0.12f; 

constexpr float TRACE_TAU_MS    = 100.0f;
constexpr float TRACE_DECAY_FACTOR = 0.990050f;


// -----------------------------------------------------------------------------
// 5. Neuromodulation (Phase I Retained)
// -----------------------------------------------------------------------------
constexpr float DOPAMINE_TAU_MS = 200.0f;
constexpr float DOPAMINE_DECAY_FACTOR = 0.995012f;


// -----------------------------------------------------------------------------
// 6. Structural Bounds (Phase I Retained)
// -----------------------------------------------------------------------------
constexpr float W_MIN           = 0.0001f;
constexpr float W_MAX           = 1.0f;

// Expanded for Phase II Sensory Layers
constexpr size_t MAX_NEURONS    = 20000; // Increased to accommodate Input Layers
constexpr size_t MAX_SYNAPSES   = MAX_NEURONS * 100;


// -----------------------------------------------------------------------------
// 7. Phase II: Sensory & Spatial Constants (NEW)
// -----------------------------------------------------------------------------

// Temporal Integration
// The ring buffer size for delayed spikes. 
// Allows neurons to detect sequences "A then B" up to 20ms apart.
constexpr size_t MAX_DELAY_TICKS = 20;

// Spatial Topology (Visual Patch / Text Grid)
// Standard 28x28 grid matches simple visual datasets (e.g., MNIST/EMNIST characters)
constexpr size_t INPUT_WIDTH  = 28;
constexpr size_t INPUT_HEIGHT = 28;
constexpr size_t INPUT_SIZE   = INPUT_WIDTH * INPUT_HEIGHT;

// Lateral Inhibition (Mexican Hat)
// Radius in grid units where an active neuron suppresses neighbors.
constexpr float INHIBITION_RADIUS   = 2.0f; 
constexpr float INHIBITION_STRENGTH = 2.0f; // Strong inhibition forces "Winner-Take-All"

} // namespace genesis