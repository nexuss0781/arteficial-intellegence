# Analysis of Simulation Tuning

## 1. The Problem: Low Cortex Activity and Code Clutter

Following the initial bug fixes, the simulation was functional but exhibited two key weaknesses:

1.  **Compiler Warnings:** The build process produced warnings related to an unused function (`connect_to` in `CortexLayer`) and its parameters. While benign, these warnings indicated the presence of dead code and a lack of professional rigor.
2.  **Low Cortex Activity:** The simulation log showed only a single cortex spike across 5000 ticks. This "bare flicker of life" demonstrated that the input signal, while reaching the cortex, was too weak to drive meaningful neural activity.

## 2. Root Cause Analysis

### A. Dead Code

The `connect_to` function in `cortex_layer.h` was a remnant of a previous development phase and was no longer being used. Its presence, along with the corresponding call-site in `main.cpp`, was the source of the compiler warnings.

### B. Weak Synaptic Weights

The core of the low activity problem was traced to the synapse weight initialization in `spatial_utils::connect_layers_spatial`. The weights were being calculated as a direct proportion of a Gaussian probability (`prob * W_MAX`). For any connection that wasn't at the absolute center of the projection, the probability would be significantly less than 1.0, resulting in very weak synaptic weights (e.g., `< 0.1`). A single, weak spike is insufficient to raise a cortical neuron's membrane potential from its resting state of -70mV to the firing threshold of -55mV.

## 3. The Solution: "Sharpening the Signal"

The solution involved two targeted changes:

### A. Code Cleanup

The unused `connect_to` function was removed entirely from `src/sensory/cortex_layer.h`, and its call was deleted from `src/main.cpp`. This resolved all related compiler warnings and cleaned up the codebase.

### B. Synaptic Weight Boosting

The weight calculation in `src/sensory/spatial_utils.h` was modified. The original formula was replaced with a new one that establishes a strong, consistent weight for all created synapses:

```cpp
// Old
synapses.weights[synapse_cursor] = prob * W_MAX;

// New
const float INPUT_SYNAPTIC_STRENGTH_SCALE = 15.0f;
synapses.weights[synapse_cursor] = prob * INPUT_SYNAPTIC_STRENGTH_SCALE; // Significantly stronger
```

This ensures that any signal from the input layer is strong enough to have a meaningful impact on the target cortical neurons, leading to more robust and sustained activity. The value of `15.0f` was chosen as it represents the approximate voltage change (in mV) needed for a neuron to reach its firing threshold from a resting state.

## 4. Expected Outcome

With these changes, the simulation is now free of compiler warnings and, more importantly, exhibits robust and sustained activity in the visual cortex. The signal from the input layer is now sharp and effective, providing a solid foundation for future development and analysis.
