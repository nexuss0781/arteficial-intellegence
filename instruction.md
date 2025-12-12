Here are the instructions to assemble and run **Project Genesis: Phase III**.

### **1. Directory Structure**
Ensure your files are arranged exactly as follows to match the `CMakeLists.txt`:

```text
genesis_project/
├── CMakeLists.txt
├── build_and_run.sh
├── src/
│   ├── constants.h
│   ├── types.h
│   ├── utils.h
│   ├── bio_engine.h
│   ├── bio_engine.cpp
│   ├── main.cpp
│   ├── sensory/
│   │   ├── input_layer.h
│   │   ├── cortex_layer.h
│   │   ├── thalamus.h
│   │   └── spatial_utils.h
│   └── learning/
│       └── hippocampus.h
└── tests/
    ├── physics_tests.cpp
    ├── sensory_tests.cpp
    └── memory_tests.cpp
```

### **2. Execution**

Open your terminal in the `genesis_project` folder and run the automation script. This will compile the engine, run all three validation suites, and launch the Pavlovian Learning experiment.

```bash
# Make the script executable
chmod +x build_and_run.sh

# Run the pipeline
./build_and_run.sh
```

### **3. Expected Output**

If successful, you will see the following cascade:

1.  **Validation Suites**:
    *   `Physics Integrity (Phase I)... [PASS]`
    *   `Sensory Dynamics (Phase II)... [PASS]`
    *   `Memory & Plasticity (Phase III)... [PASS]` (Verifies One-Shot Learning & Pattern Completion)
2.  **Main Simulation**:
    *   **T: 500-1500 (Learning)**: `VisIn`, `AudIn`, `VisCtx`, `AudCtx`, and `HIP` should all show high activity. `DA` (Dopamine) should be `1.0`.
    *   **T: 3000-4000 (Recall)**: **Crucial Step.** You should see `VisIn` activity (The Bell), **NO** `AudIn` activity, but `AudCtx` should show activity (The Hallucination of Food). This proves associative recall.
    *   **T: 4500+ (Sleep)**: The state changes to `SLEEP`. `VisIn` and `AudIn` go silent (Gating). `HIP` should show random bursts (Replay), driving weak activity in the Cortex.

**Project Genesis Phase III is now ready for deployment.**

