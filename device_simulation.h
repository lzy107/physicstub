#include <stddef.h>

#ifndef DEVICE_SIMULATION_H
#define DEVICE_SIMULATION_H

#include <stdint.h>
#include <stdbool.h>

// Device types
typedef enum {
    DEVICE_TYPE_MEMORY = 0,  // 内存设备
    DEVICE_TYPE_IO,          // IO设备
    DEVICE_TYPE_SENSOR,      // 传感器设备
    DEVICE_TYPE_FLASH,
    DEVICE_TYPE_TEMPERATURE_SENSOR,
    DEVICE_TYPE_I2C,
    DEVICE_TYPE_SPI,
    DEVICE_TYPE_MAX
} device_type_t;

// Memory region structure
typedef struct {
    uint32_t base_addr;
    uint32_t length;
    uint8_t *data;
} memory_region_t;

// Device structure
typedef struct device_node {
    device_type_t type;
    uint32_t index;
    uint32_t num_regions;
    memory_region_t *regions;
    struct device_node *next;
    
    // Function pointers for device operations
    int (*read)(struct device_node *dev, uint32_t addr, uint8_t *data, uint32_t len);
    int (*write)(struct device_node *dev, uint32_t addr, const uint8_t *data, uint32_t len);
    int (*ioctl)(struct device_node *dev, uint32_t cmd, void *arg);
} device_node_t;

// Device manager structure
typedef struct {
    device_node_t *head;
    uint32_t device_count;
} device_manager_t;

// Initialize device manager
device_manager_t* device_manager_init(void);

// Create and register a new device
device_node_t* device_create(
    device_manager_t* mgr,
    device_type_t type,
    int id,
    void* config,
    size_t config_size,
    int num_regions
);

// Find device by type and index
device_node_t* device_find(
    device_manager_t *manager,
    device_type_t type,
    uint32_t index
);

// Device operations
int device_read(
    device_node_t *dev,
    uint32_t addr,
    uint8_t *data,
    uint32_t len
);

int device_write(
    device_node_t *dev,
    uint32_t addr,
    const uint8_t *data,
    uint32_t len
);

int device_ioctl(
    device_node_t *dev,
    uint32_t cmd,
    void *arg
);

// Cleanup
void device_manager_cleanup(device_manager_t *manager);

#endif // DEVICE_SIMULATION_H
