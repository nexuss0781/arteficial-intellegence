# Analysis of Visual Cortex Simulation Issue

## 1. The Problem: Zero Cortex Activity

The simulation was showing a series of "0" for the visual cortex output, despite the input layer showing minimal but present activity. This indicated a failure in the propagation of signals from the input layer to the cortex.

## 2. Root Cause Analysis

The "zero cortex activity" issue was caused by two distinct but compounding bugs:

### Bug A: Premature Signal Decay in the Input Layer

The `integrate_and_fire` function in `src/bio_engine.cpp` applied a voltage decay to all neurons, including the input layer. This meant that even when a stimulus pushed an input neuron's potential above the firing threshold, the subsequent decay would pull it back down before the spike was registered. This effectively muted the input signal.

### Bug B: Synapse Creation Failure

The `BioEngine`'s internal `synapse_cursor_` was not being updated after the synapses were created in `main.cpp`. The engine was therefore unaware of the new connections, preventing any signal from propagating from the input layer to the cortex.

## 3. The Solution

The fix was implemented in the `integrate_and_fire` function. A specific logic block was added for the input layer:

```cpp
if (neurons_.layer_id[i] == 0) {
    if (neurons_.membrane_potential[i] > V_THRESH_MV) {
        neurons_.membrane_potential[i] = V_THRESH_MV + 1.0f; // Force fire
    }
}
```

This change ensures that if an input neuron's potential is above the threshold, it is clamped to a value that guarantees a spike on the current tick. This treats the input layer as a pure transducer, which is its intended role, rather than a dynamic neuron that integrates and leaks over time.

To address the synapse creation failure, a getter and setter for the `synapse_cursor_` were added to the `BioEngine`. This allows `main.cpp` to correctly update the engine's internal state after creating the synapses.

## 4. Expected Outcome

With these fixes, the input layer now reliably fires when stimulated, and the signal correctly propagates to the cortex, resulting in a simulation that shows meaningful activity in both layers.
