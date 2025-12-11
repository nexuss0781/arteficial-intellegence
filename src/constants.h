#pragma once

namespace genesis {

// -----------------------------------------------------------------------------
// 1. Simulation Time Base
// -----------------------------------------------------------------------------
// The fundamental tick of the universe. All physics are calibrated to 1.0 ms.
constexpr float DT_MS = 1.0f;


// -----------------------------------------------------------------------------
// 2. Membrane Dynamics (Leaky Integrate-and-Fire)
// -----------------------------------------------------------------------------
constexpr float V_REST_MV       = -70.0f;
constexpr float V_RESET_MV      = -70.0f;
constexpr float V_THRESH_MV     = -55.0f;

constexpr float MEMBRANE_TAU_MS = 20.0f;

// Optimization: Precomputed Decay Factor
// Formula: exp(-DT / MEMBRANE_TAU) = exp(-1.0 / 20.0)
// Doing this avoiding exp() calls in the hot loop.
constexpr float VOLTAGE_DECAY_FACTOR = 0.951229f; 

constexpr int   REFRACTORY_TICKS     = 5; // Absolute dead time (5ms)


// -----------------------------------------------------------------------------
// 3. Metabolic Constraints (Energy Economy)
// -----------------------------------------------------------------------------
// Refill 2% per tick -> Full recharge in 50ms (approx 20Hz sustained rate)
// Spike cost 10% -> Max immediate burst length = 10 spikes before silence
constexpr float ATP_MAX         = 1.0f;
constexpr float ATP_REFILL_RATE = 0.02f; 
constexpr float SPIKE_COST      = 0.10f; 
constexpr float ATP_MIN_TO_FIRE = 0.10f; // Hard gate: No energy, no fire.


// -----------------------------------------------------------------------------
// 4. Synaptic Plasticity (STDP + Eligibility Traces)
// -----------------------------------------------------------------------------
constexpr float STDP_WINDOW_MS  = 20.0f;

constexpr float A_PLUS          = 0.10f; // LTP magnitude (Potentiation)
constexpr float A_MINUS         = 0.12f; // LTD magnitude (Depression) - Higher for stability

constexpr float TRACE_TAU_MS    = 100.0f;

// Optimization: Precomputed Trace Decay
// Formula: exp(-DT / TRACE_TAU) = exp(-1.0 / 100.0)
constexpr float TRACE_DECAY_FACTOR = 0.990050f;


// -----------------------------------------------------------------------------
// 5. Neuromodulation (Dopamine Global Signal)
// -----------------------------------------------------------------------------
constexpr float DOPAMINE_TAU_MS = 200.0f;

// Optimization: Precomputed Dopamine Decay
// Formula: exp(-DT / DOPAMINE_TAU) = exp(-1.0 / 200.0)
constexpr float DOPAMINE_DECAY_FACTOR = 0.995012f;


// -----------------------------------------------------------------------------
// 6. Structural Bounds & Safety Limits
// -----------------------------------------------------------------------------
constexpr float W_MIN           = 0.0001f; // Minimum weight (Effective pruning threshold)
constexpr float W_MAX           = 1.0f;    // Weight saturation point

// Memory Arena Sizing
// Defined compile-time for stack/static allocation optimization if needed
constexpr size_t MAX_NEURONS    = 10000;
constexpr size_t MAX_SYNAPSES   = MAX_NEURONS * 100; // 10% Density Assumption

} // namespace genesis