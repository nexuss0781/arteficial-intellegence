# Project Genesis: Phase I - Test Results

This document records the output of the `build_and_run.sh` script.

## 1. Compilation

The compilation was successful.

```
 [0;32m  > Compilation Successful. [0m
```

## 2. Physics Tests

The physics tests were executed, but 2 out of 4 tests failed.

```
 [1m[Genesis] Executing Physics Tests... [0m
Running main() from /app/build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from BioPhysicsTest
[ RUN      ] BioPhysicsTest.SingleNeuronIntegrity
[       OK ] BioPhysicsTest.SingleNeuronIntegrity (0 ms)
[ RUN      ] BioPhysicsTest.STDP_CausalLink
/app/tests/physics_tests.cpp:80: Failure
Expected: (trace_after_pre) > (0.0f), actual: -0.0199999958 vs 0
Eligibility trace should increase after Pre spike

/app/tests/physics_tests.cpp:94: Failure
Expected: (w_final) > (w_initial), actual: 0.0001 vs 0.0179732423
Weight should increase with Dopamine + LTP Trace

[  FAILED  ] BioPhysicsTest.STDP_CausalLink (0 ms)
[ RUN      ] BioPhysicsTest.DopamineGatingAndLTD
[       OK ] BioPhysicsTest.DopamineGatingAndLTD (0 ms)
[ RUN      ] BioPhysicsTest.MetabolicCrash
/app/tests/physics_tests.cpp:161: Failure
Expected: (avg_atp) < (0.5f), actual: 0.959999919 vs 0.5
Network should suffer ATP drain under heavy load

[  FAILED  ] BioPhysicsTest.MetabolicCrash (0 ms)
[----------] 4 tests from BioPhysicsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] BioPhysicsTest.STDP_CausalLink
[  FAILED  ] BioPhysicsTest.MetabolicCrash

 2 FAILED TESTS
 [0;31m  > Critical Physics Failure. Aborting Simulation. [0m
```

## 3. Main Simulation

The main simulation was aborted due to the critical physics failures and did not run.

## 4. Performance Report

No performance report was generated.

## Conclusion

The project did not build and run as expected. Two physics tests failed, preventing the main simulation from launching. The failing tests are:
- `BioPhysicsTest.STDP_CausalLink`
- `BioPhysicsTest.MetabolicCrash`
