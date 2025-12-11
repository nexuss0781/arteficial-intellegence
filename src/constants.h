#pragma once

namespace genesis {

// -----------------------------------------------------------------------------
// 1. Simulation Time Base
// -----------------------------------------------------------------------------
constexpr float DT_MS = 1.0f;


// -----------------------------------------------------------------------------
// 2. Membrane Dynamics
// -----------------------------------------------------------------------------
constexpr float V_REST_MV       = -70.0f;
constexpr float V_RESET_MV      = -70.0f;
constexpr float V_THRESH_MV     = -55.0f;

constexpr float MEMBRANE_TAU_MS = 20.0f;
constexpr float VOLTAGE_DECAY_FACTOR = 0.951229f; // exp(-1.0 / 20.0)

constexpr int   REFRACTORY_TICKS     = 5;


// -----------------------------------------------------------------------------
// 3. Metabolic Constraints (Energy Economy)
// -----------------------------------------------------------------------------
constexpr float ATP_MAX         = 1.0f;

// FIX: Reduced from 0.02 to 0.01.
// At max firing rate (every 5 ticks), refill is 0.05, cost is 0.10.
// This ensures a net deficit (-0.05) under load, enabling metabolic fatigue.
constexpr float ATP_REFILL_RATE = 0.01f; 

constexpr float SPIKE_COST      = 0.10f; 
constexpr float ATP_MIN_TO_FIRE = 0.10f;


// -----------------------------------------------------------------------------
// 4. Synaptic Plasticity
// -----------------------------------------------------------------------------
constexpr float STDP_WINDOW_MS  = 20.0f;

constexpr float A_PLUS          = 0.10f; 
constexpr float A_MINUS         = 0.12f; 

constexpr float TRACE_TAU_MS    = 100.0f;
constexpr float TRACE_DECAY_FACTOR = 0.990050f;


// -----------------------------------------------------------------------------
// 5. Neuromodulation
// -----------------------------------------------------------------------------
constexpr float DOPAMINE_TAU_MS = 200.0f;
constexpr float DOPAMINE_DECAY_FACTOR = 0.995012f;


// -----------------------------------------------------------------------------
// 6. Structural Bounds
// -----------------------------------------------------------------------------
constexpr float W_MIN           = 0.0001f;
constexpr float W_MAX           = 1.0f;

constexpr size_t MAX_NEURONS    = 10000;
constexpr size_t MAX_SYNAPSES   = MAX_NEURONS * 100;

} // namespace genesis