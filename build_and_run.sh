#!/bin/bash

# ------------------------------------------------------------------------------
# Project Genesis: Phase I Build & Automation Script
# ------------------------------------------------------------------------------
# Purpose:
# 1. Sets up the build environment (CMake).
# 2. Compiles the engine with maximum optimizations (-O3).
# 3. Runs the GoogleTest validation suite to prove physics integrity.
# 4. Executes the main simulation if tests pass.
# ------------------------------------------------------------------------------

# Exit immediately if any command fails
set -e

# Visual formatting
BOLD='\033[1m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BOLD}[Genesis] Initializing Build Pipeline...${NC}"

# 1. Directory Setup
if [ ! -d "build" ]; then
    echo -e "  > Creating build directory..."
    mkdir build
else
    echo -e "  > Build directory exists. Cleaning..."
    rm -rf build/*
fi

cd build

# 2. CMake Configuration
# We explicitly request Release mode to ensure the -O3 flags defined in CMakeLists.txt are active.
echo -e "${BOLD}[Genesis] Configuring Build System...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release ..

# 3. Compilation
# Uses $(nproc) to utilize all available CPU cores for parallel compilation.
echo -e "${BOLD}[Genesis] Compiling Bio-Digital Substrate...${NC}"
if cmake --build . --config Release -- -j$(nproc); then
    echo -e "${GREEN}  > Compilation Successful.${NC}"
else
    echo -e "${RED}  > Compilation Failed.${NC}"
    exit 1
fi

# 4. Physics Validation (Unit Tests)
echo -e "${BOLD}[Genesis] Executing Physics Tests...${NC}"
if ./physics_tests; then
    echo -e "${GREEN}  > All Physics Invariants Verified.${NC}"
else
    echo -e "${RED}  > Critical Physics Failure. Aborting Simulation.${NC}"
    exit 1
fi

# 5. Main Simulation
echo -e "${BOLD}[Genesis] Launching Main Simulation...${NC}"
echo "----------------------------------------------------------------"
./genesis_sim
echo "----------------------------------------------------------------"

echo -e "${GREEN}[Genesis] Workflow Completed Successfully.${NC}"