cmake_minimum_required(VERSION 3.10)
project(DeviceSimulation)

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(DeviceSimulationTest
    device_simulation.c
)

# 启用所有警告
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(DeviceSimulationTest PRIVATE -Wall -Wextra)
endif()

# 可选：调试配置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(DeviceSimulationTest PRIVATE -DDEBUG)
endif() 