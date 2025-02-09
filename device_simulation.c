#include "device_simulation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Define the device_region_t structure
typedef struct {
    uint32_t base_addr;
    uint32_t length;
} device_region_t;

// Initialize device manager
device_manager_t* device_manager_init(void) {
    device_manager_t *manager = (device_manager_t *)malloc(sizeof(device_manager_t));
    if (manager) {
        manager->head = NULL;
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
    int num_regions
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
    dev->read = device_read;
    dev->write = device_write;
    dev->ioctl = device_ioctl;

    // Add to linked list
    if (!mgr->head) {
        mgr->head = dev;
    } else {
        device_node_t *current = mgr->head;
        while (current->next) {
            current = current->next;
        }
        current->next = dev;
    }
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

    device_node_t *current = manager->head;
    while (current) {
        if (current->type == type && current->index == index) {
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
int device_read(
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

    device_node_t *current = manager->head;
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
    free(manager);
}

// 新增测试主函数
int main() {
    printf("开始设备模拟测试...\n");
    
    // 初始化设备管理器
    device_manager_t *mgr = device_manager_init();
    if (!mgr) {
        printf("初始化失败！\n");
        return 1;
    }

    // 创建测试设备
    device_region_t regions[] = {
        {0x1000, 1024},  // 内存区域1
        {0x2000, 512}    // 内存区域2
    };
    
    device_node_t *dev = device_create(
        mgr, 
        DEVICE_TYPE_MEMORY, 
        1, 
        regions,                // 作为config参数传递
        sizeof(regions),         // 添加config_size参数
        2                       // num_regions参数
    );
    if (!dev) {
        printf("创建设备失败！\n");
        device_manager_cleanup(mgr);
        return 1;
    }

    // 测试读写操作
    uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_data[4] = {0};
    
    printf("\n测试写入内存区域1...\n");
    if (device_write(dev, 0x1000, write_data, 4) == 0) {
        printf("写入成功\n");
    }

    printf("\n测试读取内存区域1...\n");
    if (device_read(dev, 0x1000, read_data, 4) == 0) {
        printf("读取数据: ");
        for (int i = 0; i < 4; i++) {
            printf("%02X ", read_data[i]);
        }
        printf("\n");
    }

    // 清理资源
    device_manager_cleanup(mgr);
    printf("\n测试完成！\n");
    return 0;
}


