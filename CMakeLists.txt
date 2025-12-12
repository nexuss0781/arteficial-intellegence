cmake_minimum_required(VERSION 3.14)
project(project_genesis_phase_3 CXX)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization Flags
if(MSVC)
    add_compile_options(/O2 /arch:AVX2 /W4)
else()
    add_compile_options(-O3 -march=native -Wall -Wextra -Wpedantic)
endif()

# Core Logic Library
add_library(bio_core
    src/bio_engine.cpp
)
target_include_directories(bio_core PUBLIC src)

# Main Executable
add_executable(genesis_sim src/main.cpp)
target_link_libraries(genesis_sim PRIVATE bio_core)

# ------------------------------------------------------------------------------
# Testing Infrastructure
# ------------------------------------------------------------------------------
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test 1: Physics (Phase I)
add_executable(physics_tests tests/physics_tests.cpp)
target_link_libraries(physics_tests PRIVATE bio_core GTest::gtest_main)

# Test 2: Sensory (Phase II)
add_executable(sensory_tests tests/sensory_tests.cpp)
target_link_libraries(sensory_tests PRIVATE bio_core GTest::gtest_main)

# Test 3: Memory (Phase III - NEW)
add_executable(memory_tests tests/memory_tests.cpp)
target_link_libraries(memory_tests PRIVATE bio_core GTest::gtest_main)

# Integration
include(GoogleTest)
gtest_discover_tests(physics_tests)
gtest_discover_tests(sensory_tests)
gtest_discover_tests(memory_tests)