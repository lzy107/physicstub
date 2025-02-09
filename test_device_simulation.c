#include "device_simulation.h"
#include <stdio.h>
#include <string.h>

int main() {
    int test_result = 0;
    const char* test_case = "内存设备基础测试";
    
    // 初始化设备管理器
    device_manager_t *mgr = device_manager_init();
    if (!mgr) {
        printf("[%s] 失败：初始化失败\n", test_case);
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
        regions,
        sizeof(regions),
        2
    );
    if (!dev) {
        printf("[%s] 失败：设备创建失败\n", test_case);
        device_manager_cleanup(mgr);
        return 1;
    }

    // 测试读写操作
    uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_data[4] = {0};
    
    if (device_write(dev, 0x1000, write_data, 4) != 0) {
        printf("[%s] 失败：写入操作\n", test_case);
        test_result = 1;
        goto cleanup;
    }

    if (device_read(dev, 0x1000, read_data, 4) != 0) {
        printf("[%s] 失败：读取操作\n", test_case);
        test_result = 1;
        goto cleanup;
    }

    // 自动化数据验证
    if (memcmp(write_data, read_data, sizeof(write_data)) != 0) {
        printf("[%s] 失败：数据校验\n", test_case);
        printf("预期数据: ");
        for (int i = 0; i < 4; i++) {
            printf("%02X ", write_data[i]);
        }
        printf("\n实际数据: ");
        for (int i = 0; i < 4; i++) {
            printf("%02X ", read_data[i]);
        }
        printf("\n");
        test_result = 1;
    } else {
        printf("[%s] 成功\n", test_case);
    }

    // 在cleanup前添加类型检查
    device_node_t *mem_dev = device_find(mgr, DEVICE_TYPE_MEMORY, 1);
    if (!mem_dev) {
        printf("[%s] 失败：设备注册\n", test_case);
        test_result = 1;
    }

    device_node_t *invalid_dev = device_find(mgr, DEVICE_TYPE_IO, 1);
    if (invalid_dev) {
        printf("[%s] 失败：设备类型冲突\n", test_case);
        test_result = 1;
    }

cleanup:
    // 清理资源
    device_manager_cleanup(mgr);
    return test_result;
} 