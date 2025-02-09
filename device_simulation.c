#include "device_simulation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Initialize device manager
device_manager_t* device_manager_init(void) {
    device_manager_t *manager = (device_manager_t *)malloc(sizeof(device_manager_t));
    if (manager) {
        for (int i = 0; i < DEVICE_TYPE_MAX; i++) {
            manager->devices[i] = NULL;
        }
        manager->device_count = 0;
    }
    return manager;
}

// Create and register a new device
device_node_t* device_create(
    device_manager_t *mgr,
    device_type_t type,
    int id,
    void* config,
    size_t config_size,
    int num_regions,
    int (*read_func)(struct device_node*, uint32_t, uint8_t*, uint32_t),
    int (*write_func)(struct device_node*, uint32_t, const uint8_t*, uint32_t),
    int (*ioctl_func)(struct device_node*, uint32_t, void*)
) {
    if (!mgr || !config || type >= DEVICE_TYPE_MAX) {
        return NULL;
    }

    // Check if device already exists
    if (device_find(mgr, type, id)) {
        return NULL;
    }

    // Allocate new device node
    device_node_t *dev = (device_node_t *)malloc(sizeof(device_node_t));
    if (!dev) {
        return NULL;
    }

    // Initialize device node
    dev->type = type;
    dev->index = id;
    dev->num_regions = num_regions;
    dev->next = NULL;

    // Allocate memory regions
    dev->regions = (memory_region_t *)malloc(sizeof(memory_region_t) * num_regions);
    if (!dev->regions) {
        free(dev);
        return NULL;
    }

    // Initialize memory regions
    const device_region_t *regions = (const device_region_t*)config;
    for (uint32_t i = 0; i < num_regions; i++) {
        dev->regions[i].base_addr = regions[i].base_addr;
        dev->regions[i].length = regions[i].length;
        dev->regions[i].data = (uint8_t *)calloc(1, regions[i].length);
        if (!dev->regions[i].data) {
            // Cleanup on failure
            for (uint32_t j = 0; j < i; j++) {
                free(dev->regions[j].data);
            }
            free(dev->regions);
            free(dev);
            return NULL;
        }
    }

    // Set default operations
    dev->ops.read = read_func ? read_func : device_read;
    dev->ops.write = write_func ? write_func : device_write;
    dev->ops.ioctl = ioctl_func ? ioctl_func : device_ioctl;

    // Add to linked list
    dev->next = mgr->devices[type];
    mgr->devices[type] = dev;
    mgr->device_count++;

    return dev;
}

// Find device by type and index
device_node_t* device_find(
    device_manager_t *manager,
    device_type_t type,
    uint32_t index
) {
    if (!manager) {
        return NULL;
    }

    device_node_t *current = manager->devices[type];
    while (current) {
        if (current->index == index) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Helper function to find memory region for address
static memory_region_t* find_region(device_node_t *dev, uint32_t addr) {
    for (uint32_t i = 0; i < dev->num_regions; i++) {
        uint32_t start = dev->regions[i].base_addr;
        uint32_t end = start + dev->regions[i].length;
        if (addr >= start && addr < end) {
            return &dev->regions[i];
        }
    }
    return NULL;
}

// Generic read operation
static int device_read(
    device_node_t *dev,
    uint32_t addr,
    uint8_t *data,
    uint32_t len
) {
    if (!dev || !data) {
        return -1;
    }

    memory_region_t *region = find_region(dev, addr);
    if (!region) {
        return -1;
    }

    uint32_t offset = addr - region->base_addr;
    if (offset + len > region->length) {
        return -1;
    }

    memcpy(data, &region->data[offset], len);
    return 0;
}

// Generic write operation
int device_write(
    device_node_t *dev,
    uint32_t addr,
    const uint8_t *data,
    uint32_t len
) {
    printf("尝试写入地址 0x%x 长度 %u\n", addr, len);
    if (!dev || !data) {
        return -1;
    }

    memory_region_t *region = find_region(dev, addr);
    if (!region) {
        return -1;
    }

    uint32_t offset = addr - region->base_addr;
    if (offset + len > region->length) {
        return -1;
    }

    memcpy(&region->data[offset], data, len);
    return 0;
}

// Generic ioctl operation
int device_ioctl(
    device_node_t *dev,
    uint32_t cmd,
    void *arg
) {
    // Default implementation does nothing
    (void)dev;
    (void)cmd;
    (void)arg;
    return 0;
}

// Cleanup
void device_manager_cleanup(device_manager_t *manager) {
    if (!manager) {
        return;
    }

    for (int type = 0; type < DEVICE_TYPE_MAX; type++) {
        device_node_t *current = manager->devices[type];
        while (current) {
            device_node_t *next = current->next;
            
            // Free all memory regions
            for (uint32_t i = 0; i < current->num_regions; i++) {
                free(current->regions[i].data);
            }
            free(current->regions);
            free(current);
            
            current = next;
        }
    }
    free(manager);
}

// 新增公共访问接口
int device_node_read(device_node_t *dev, uint32_t addr, uint8_t *data, uint32_t len) {
    return dev->ops.read ? dev->ops.read(dev, addr, data, len) : -1;
}

// 新增公共访问接口
int device_node_write(device_node_t *dev, uint32_t addr, const uint8_t *data, uint32_t len) {
    return dev->ops.write ? dev->ops.write(dev, addr, data, len) : -1;
}

int device_node_ioctl(device_node_t *dev, uint32_t cmd, void *arg) {
    return dev->ops.ioctl ? dev->ops.ioctl(dev, cmd, arg) : -1;
}


