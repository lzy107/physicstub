#!/bin/bash
echo "Building with GCC..."
gcc device_simulation.c test_device_simulation.c -o device_simulation_test -lpthread -std=c11
echo "Running..."
./device_simulation_test