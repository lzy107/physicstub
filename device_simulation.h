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

// Device region config structure (已在头文件中正确定义)
typedef struct {
    uint32_t base_addr;
    uint32_t length;
} device_region_t;

// Device structure
typedef struct device_node {
    device_type_t type;
    uint32_t index;
    uint32_t num_regions;
    memory_region_t *regions;
    struct device_node *next;
    
    // 设备操作函数结构体
    struct {
        int (*read)(struct device_node *dev, uint32_t addr, uint8_t *data, uint32_t len);
        int (*write)(struct device_node *dev, uint32_t addr, const uint8_t *data, uint32_t len);
        int (*ioctl)(struct device_node *dev, uint32_t cmd, void *arg);
    } ops;
} device_node_t;

// Device manager structure
typedef struct {
    device_node_t *devices[DEVICE_TYPE_MAX]; // 按类型分组的链表头
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
    int num_regions,
    int (*read_func)(struct device_node*, uint32_t, uint8_t*, uint32_t),
    int (*write_func)(struct device_node*, uint32_t, const uint8_t*, uint32_t),
    int (*ioctl_func)(struct device_node*, uint32_t, void*)
);

// Find device by type and index
device_node_t* device_find(
    device_manager_t *manager,
    device_type_t type,
    uint32_t index
);

// Device operations
static int device_read(
    device_node_t *dev,
    uint32_t addr,
    uint8_t *data,
    uint32_t len
);

static int device_write(
    device_node_t *dev,
    uint32_t addr,
    const uint8_t *data,
    uint32_t len
);

static int device_ioctl(
    device_node_t *dev,
    uint32_t cmd,
    void *arg
);

// Cleanup
void device_manager_cleanup(device_manager_t *manager);

// 新增公共接口声明
int device_node_read(device_node_t *dev, uint32_t addr, uint8_t *data, uint32_t len);
int device_node_write(device_node_t *dev, uint32_t addr, const uint8_t *data, uint32_t len);
int device_node_ioctl(device_node_t *dev, uint32_t cmd, void *arg);

#endif // DEVICE_SIMULATION_H
