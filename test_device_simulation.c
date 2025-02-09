#include "device_simulation.h"
#include <stdio.h>
#include <string.h>

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
        regions,
        sizeof(regions),
        2
    );
    if (!dev) {
        printf("创建设备失败！\n");
        device_manager_cleanup(mgr);
        return 1;
    }

    // 测试读写操作
    uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_data[4] = {0};
    int test_result = 0;
    
    printf("\n测试写入内存区域1...\n");
    if (device_write(dev, 0x1000, write_data, 4) != 0) {
        printf("[失败] 写入操作失败\n");
        test_result = 1;
        goto cleanup;
    }

    printf("\n测试读取内存区域1...\n");
    if (device_read(dev, 0x1000, read_data, 4) != 0) {
        printf("[失败] 读取操作失败\n");
        test_result = 1;
        goto cleanup;
    }

    // 自动化数据验证
    if (memcmp(write_data, read_data, sizeof(write_data)) != 0) {
        printf("[失败] 数据校验不匹配\n");
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
        printf("[成功] 数据校验通过\n");
    }

    // 在cleanup前添加类型检查
    printf("\n验证设备类型分组...\n");
    device_node_t *mem_dev = device_find(mgr, DEVICE_TYPE_MEMORY, 1);
    if (!mem_dev) {
        printf("[失败] 内存设备未正确注册\n");
        test_result = 1;
    }

    device_node_t *invalid_dev = device_find(mgr, DEVICE_TYPE_IO, 1);
    if (invalid_dev) {
        printf("[失败] 错误类型设备存在\n");
        test_result = 1;
    }

cleanup:
    // 清理资源
    device_manager_cleanup(mgr);
    printf("\n测试完成！\n");
    return test_result;
} 