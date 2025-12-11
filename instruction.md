Here are the instructions to assemble and run **Project Genesis: Phase I**.

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
│   └── main.cpp
└── tests/
    └── physics_tests.cpp
```

### **2. Execution**

Open your terminal in the `genesis_project` folder and run the automation script. This will compile the engine, validate the physics with GoogleTest, and launch the simulation.

```bash
# Make the script executable
chmod +x build_and_run.sh

# Run the pipeline
./build_and_run.sh
```

### **3. Expected Output**

If successful, you will see the following cascade:

1.  **[Genesis] Compiling...** (Builds the C++ binaries)
2.  **[Genesis] Executing Physics Tests...** (Runs `physics_tests`)
    *   *Look for:* `[  PASSED  ] 4 tests.`
3.  **[Genesis] Launching Main Simulation...**
    *   You will see the log output showing `Tick`, `Spikes`, `DA` (Dopamine), and `Avg ATP`.
    *   At **Tick 2000**, Dopamine will jump to `1.0`.
    *   At **Tick 4000**, Dopamine will drop to `0.0`.
4.  **Performance Report**:
    *   *Look for:* `Speedup: Xx faster than real-time`. This confirms the engine is efficient enough for massive scaling.

**Project Genesis Phase I is now ready for deployment.**	
