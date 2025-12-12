 [1m[Genesis] Initializing Build Pipeline... [0m
  > Build directory exists. Cleaning...
 [1m[Genesis] Configuring Build System... [0m
-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- The C compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Python3: /home/jules/.pyenv/shims/python3 (found version "3.12.12") found components: Interpreter
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Configuring done (3.9s)
-- Generating done (0.0s)
-- Build files have been written to: /app/build
 [1m[Genesis] Compiling Bio-Digital Substrate... [0m
[  7%] Building CXX object CMakeFiles/bio_core.dir/src/bio_engine.cpp.o
[ 14%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
[ 21%] Linking CXX static library libbio_core.a
[ 21%] Built target bio_core
[ 28%] Building CXX object CMakeFiles/genesis_sim.dir/src/main.cpp.o
[ 35%] Linking CXX executable genesis_sim
[ 35%] Built target genesis_sim
[ 42%] Linking CXX static library ../../../lib/libgtest.a
[ 42%] Built target gtest
[ 50%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
[ 57%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
[ 64%] Linking CXX static library ../../../lib/libgtest_main.a
[ 64%] Built target gtest_main
[ 71%] Building CXX object CMakeFiles/physics_tests.dir/tests/physics_tests.cpp.o
[ 78%] Linking CXX executable physics_tests
[ 78%] Built target physics_tests
[ 85%] Linking CXX static library ../../../lib/libgmock.a
[ 85%] Built target gmock
[ 92%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
[100%] Linking CXX static library ../../../lib/libgmock_main.a
[100%] Built target gmock_main
 [0;32m  > Compilation Successful. [0m
 [1m[Genesis] Executing Physics Tests... [0m
Running main() from /app/build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from BioPhysicsTest
[ RUN      ] BioPhysicsTest.SingleNeuronIntegrity
[       OK ] BioPhysicsTest.SingleNeuronIntegrity (0 ms)
[ RUN      ] BioPhysicsTest.STDP_CausalLink
[       OK ] BioPhysicsTest.STDP_CausalLink (0 ms)
[ RUN      ] BioPhysicsTest.DopamineGatingAndLTD
[       OK ] BioPhysicsTest.DopamineGatingAndLTD (0 ms)
[ RUN      ] BioPhysicsTest.MetabolicCrash
[       OK ] BioPhysicsTest.MetabolicCrash (0 ms)
[----------] 4 tests from BioPhysicsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
 [0;32m  > All Physics Invariants Verified. [0m
 [1m[Genesis] Launching Main Simulation... [0m
----------------------------------------------------------------
[Genesis] Phase II: Sensory Cortex Simulation (Corrected Protocol)
  > Creating Layers...
  > Wiring Retina to Cortex...
  > 17021 synapses created.
[Genesis] Network ready. Starting Simulation...
Tick:      0 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:    100 | Input Spikes:   1 | Cortex Spikes:   6 | DA: 0.00 | ACh: 1.00
Tick:    200 | Input Spikes:   0 | Cortex Spikes:  13 | DA: 0.00 | ACh: 1.00
Tick:    300 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:    400 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:    500 | Input Spikes:   5 | Cortex Spikes:   8 | DA: 0.00 | ACh: 1.00
Tick:    600 | Input Spikes:   1 | Cortex Spikes:   8 | DA: 0.00 | ACh: 1.00
Tick:    700 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:    800 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:    900 | Input Spikes:   1 | Cortex Spikes:   4 | DA: 0.00 | ACh: 1.00
Tick:   1000 | Input Spikes:   0 | Cortex Spikes:   6 | DA: 0.00 | ACh: 1.00
Tick:   1100 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   1200 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   1300 | Input Spikes:   0 | Cortex Spikes:   7 | DA: 0.00 | ACh: 1.00
Tick:   1400 | Input Spikes:   0 | Cortex Spikes:   6 | DA: 0.00 | ACh: 1.00
Tick:   1500 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   1600 | Input Spikes:   3 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   1700 | Input Spikes:   1 | Cortex Spikes:   6 | DA: 0.00 | ACh: 1.00
Tick:   1800 | Input Spikes:   1 | Cortex Spikes:  15 | DA: 0.00 | ACh: 1.00
Tick:   1900 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   2000 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 1.00 | ACh: 1.00
Tick:   2100 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.60 | ACh: 1.00
Tick:   2200 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.37 | ACh: 1.00
Tick:   2300 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.22 | ACh: 1.00
Tick:   2400 | Input Spikes:   2 | Cortex Spikes:   0 | DA: 0.13 | ACh: 1.00
Tick:   2500 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.08 | ACh: 1.00
Tick:   2600 | Input Spikes:   0 | Cortex Spikes:   1 | DA: 0.05 | ACh: 1.00
Tick:   2700 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.03 | ACh: 1.00
Tick:   2800 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.02 | ACh: 1.00
Tick:   2900 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.01 | ACh: 1.00
Tick:   3000 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.01 | ACh: 1.00
Tick:   3100 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3200 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3300 | Input Spikes:   2 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3400 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3500 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3600 | Input Spikes:   2 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3700 | Input Spikes:   3 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3800 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   3900 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4000 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4100 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4200 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4300 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4400 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4500 | Input Spikes:   1 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4600 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4700 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4800 | Input Spikes:   4 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
Tick:   4900 | Input Spikes:   0 | Cortex Spikes:   0 | DA: 0.00 | ACh: 1.00
[Genesis] Simulation Complete.
  Real Time: 0.11s
  Bio Time:  5.00s
  Speedup:   44.77x faster than real-time
  Ticks/Sec: 44774.71
----------------------------------------------------------------
 [0;32m[Genesis] Workflow Completed Successfully. [0m
 CMake Warning (dev) at /usr/share/cmake-3.28/Modules/FetchContent.cmake:1331 (message):
  The DOWNLOAD_EXTRACT_TIMESTAMP option was not given and policy CMP0135 is
  not set.  The policy's OLD behavior will be used.  When using a URL
  download, the timestamps of extracted files should preferably be that of
  the time of extraction, otherwise code that depends on the extracted
  contents might not be rebuilt if the URL changes.  The OLD behavior
  preserves the timestamps from the archive instead, but this is usually not
  what you want.  Update your project to the NEW behavior or specify the
  DOWNLOAD_EXTRACT_TIMESTAMP option with a value of true to avoid this
  robustness issue.
Call Stack (most recent call first):
  CMakeLists.txt:34 (FetchContent_Declare)
This warning is for project developers.  Use -Wno-dev to suppress it.
