#include "device_simulation.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_CASE(name) printf("\n=== 测试用例: %s ===\n", name)
#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        printf("[失败] %s (文件: %s 行号: %d)\n", msg, __FILE__, __LINE__); \
        return 1; \
    } \
    printf("[通过] %s\n", msg); \
} while(0)

int test_basic_rw() {
    TEST_CASE("基础读写测试");
    int result = 0;
    
    device_manager_t *mgr = device_manager_init();
    CHECK(mgr != NULL, "初始化设备管理器");

    device_region_t regions[] = {{0x1000, 1024}, {0x2000, 512}};
    device_node_t *dev = device_create(
        mgr, 
        DEVICE_TYPE_MEMORY, 
        1, 
        regions,
        sizeof(regions),
        2,
        NULL,  // 使用默认read函数
        NULL,  // 使用默认write函数
        NULL   // 使用默认ioctl函数
    );
    CHECK(dev != NULL, "创建设备节点");

    const uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_data[4] = {0};
    
    // 测试写入
    result = device_node_write(dev, 0x1000, write_data, sizeof(write_data));
    CHECK(result == 0, "写入操作");

    // 测试读取
    result = device_node_read(dev, 0x1000, read_data, sizeof(read_data));
    CHECK(result == 0, "读取操作");

    // 数据校验
    CHECK(memcmp(write_data, read_data, sizeof(write_data)) == 0, "数据一致性校验");

cleanup:
    device_manager_cleanup(mgr);
    return 0;
}

int main() {
    printf("开始设备模拟测试...\n");
    
    int total_failures = 0;
    total_failures += test_basic_rw();
    
    printf("\n测试完成，失败用例数: %d\n", total_failures);
    return total_failures > 0 ? 1 : 0;
} 