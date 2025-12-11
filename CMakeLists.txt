cmake_minimum_required(VERSION 3.14)
project(project_genesis_phase_1 CXX)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization Flags: Critical for High-Performance Physics Simulation
# -O3: Max optimization
# -march=native: Use instruction sets available on host CPU (AVX, etc.)
if(MSVC)
    add_compile_options(/O2 /arch:AVX2 /W4)
else()
    add_compile_options(-O3 -march=native -Wall -Wextra -Wpedantic)
endif()

# Module Definition: Core Logic Library
# Separating the engine into a library allows reuse in both Main and Tests
add_library(bio_core
    src/bio_engine.cpp
)
target_include_directories(bio_core PUBLIC src)

# Main Executable
add_executable(genesis_sim src/main.cpp)
target_link_libraries(genesis_sim PRIVATE bio_core)

# ------------------------------------------------------------------------------
# Testing Infrastructure (GoogleTest)
# ------------------------------------------------------------------------------
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent GoogleTest from overriding compiler flags on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test Executable
add_executable(physics_tests tests/physics_tests.cpp)
target_link_libraries(physics_tests PRIVATE bio_core GTest::gtest_main)

# Integration
include(GoogleTest)
gtest_discover_tests(physics_tests)