# PhysicStub

A device simulation framework for embedded systems that provides memory-mapped device simulation capabilities. This framework allows developers to simulate various types of devices such as memory devices, I/O devices, sensors, flash memory, and communication buses (I2C, SPI).

## Features

- Device type classification (Memory, IO, Sensor, Flash, etc.)
- Memory region simulation with base address mapping
- Generic read/write operations with bounds checking
- Linked list based device management
- Extensible device operations through function pointers
- Complete memory management with proper cleanup

## Usage

```c
// Initialize device manager
device_manager_t *mgr = device_manager_init();

// Configure device regions
device_region_t regions[] = {
    {0x1000, 1024},  // Memory region 1
    {0x2000, 512}    // Memory region 2
};

// Create a memory device
device_node_t *dev = device_create(
    mgr,
    DEVICE_TYPE_MEMORY,
    1,
    regions,
    sizeof(regions),
    2
);

// Perform read/write operations
uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
device_write(dev, 0x1000, write_data, 4);
```
