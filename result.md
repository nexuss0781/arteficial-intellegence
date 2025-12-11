# Project Genesis: Phase I Test Results

The `build_and_run.sh` script failed to execute successfully. The compilation process encountered an error, preventing the tests and the main simulation from running.

## Error Log

```
 [1m[Genesis] Initializing Build Pipeline... [0m
  > Creating build directory...
 [1m[Genesis] Configuring Build System... [0m
-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
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
-- Configuring done (7.8s)
-- Generating done (0.0s)
-- Build files have been written to: /app/build
 [1m[Genesis] Compiling Bio-Digital Substrate... [0m
[  7%] Building CXX object CMakeFiles/bio_core.dir/src/bio_engine.cpp.o
[ 14%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
[ 21%] Linking CXX static library libbio_core.a
[ 21%] Built target bio_core
[ 28%] Building CXX object CMakeFiles/genesis_sim.dir/src/main.cpp.o
In file included from /app/src/main.cpp:3:
/app/src/sensory/cortex_layer.h: In member function ‘void genesis::CortexLayer::connect_to(const genesis::LayerMeta&, float, float)’:
/app/src/sensory/cortex_layer.h:21:15: warning: unused variable ‘neurons’ [-Wunused-variable]
   21 |         auto& neurons = const_cast<NeuronBlock&>(engine_.get_neurons()); // Unsafe, but needed for pos
      |               ^~~~~~~
/app/src/sensory/cortex_layer.h:20:38: warning: unused parameter ‘input_layer’ [-Wunused-parameter]
   20 |     void connect_to(const LayerMeta& input_layer, float sigma, float weight_scale) {
      |                     ~~~~~~~~~~~~~~~~~^~~~~~~~~~~
/app/src/sensory/cortex_layer.h:20:57: warning: unused parameter ‘sigma’ [-Wunused-parameter]
   20 |     void connect_to(const LayerMeta& input_layer, float sigma, float weight_scale) {
      |                                                   ~~~~~~^~~~~
/app/src/sensory/cortex_layer.h:20:70: warning: unused parameter ‘weight_scale’ [-Wunused-parameter]
   20 |     void connect_to(const LayerMeta& input_layer, float sigma, float weight_scale) {
      |                                                                ~~~~~~^~~~~~~~~~~~
In file included from /app/src/main.cpp:4:
/app/src/sensory/thalamus.h: At global scope:
/app/src/sensory/thalamus.h:84:5: error: ‘std’ does not name a type
   84 |     std.vector<float> activity_avg_;
      |     ^~~
/app/src/sensory/thalamus.h: In constructor ‘genesis::Thalamus::Thalamus(genesis::BioEngine&, const genesis::LayerMeta&)’:
/app/src/sensory/thalamus.h:19:11: error: class ‘genesis::Thalamus’ does not have any field named ‘activity_avg_’
   19 |           activity_avg_(meta_.count, 0.0f)
      |           ^~~~~~~~~~~~~
/app/src/sensory/thalamus.h: In member function ‘void genesis::Thalamus::apply_gating()’:
/app/src/sensory/thalamus.h:45:13: error: ‘activity_avg_’ was not declared in this scope
   45 |             activity_avg_[i] = (activity_avg_[i] * 0.99f) + (current_potential * 0.01f);
      |             ^~~~~~~~~~~~~
/app/src/main.cpp: In function ‘std::vector<float> create_vertical_line_stimulus()’:
/app/src/main.cpp:47:23: warning: comparison of integer expressions of different signedness: ‘int’ and ‘long unsigned int’ [-Wsign-compare]
   47 |     for (int y = 5; y < INPUT_HEIGHT - 5; ++y) {
      |                     ~~^~~~~~~~~~~~~~~~~~
gmake[2]: *** [CMakeFiles/genesis_sim.dir/build.make:76: CMakeFiles/genesis_sim.dir/src/main.cpp.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:166: CMakeFiles/genesis_sim.dir/all] Error 2
gmake[1]: *** Waiting for unfinished jobs....
[ 35%] Linking CXX static library ../../../lib/libgtest.a
[ 35%] Built target gtest
gmake: *** [Makefile:146: all] Error 2
 [0;31m  > Compilation Failed. [0m
```
