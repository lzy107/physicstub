#!/bin/bash
echo "Building with GCC..."
gcc device_simulation.c -o device_simulation -lpthread -std=c11
echo "Running..."
./device_simulation