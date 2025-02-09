#!/bin/bash
# 清理旧构建
echo "清理旧构建文件..."
rm -rf build && mkdir -p build

echo "Building with GCC..."
gcc -std=c11 \
    -I. \
    device_simulation.c \
    tests/device_simulation_test.c \
    -o build/device_simulation_test \
    -lpthread

echo "Running..."
./build/device_simulation_test