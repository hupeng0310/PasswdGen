#include "random.h"
#include <stdio.h>
#include <string.h>

// 平台检测
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
#else
    #error "Unsupported platform"
#endif

// 平台相关的全局变量
#ifdef PLATFORM_WINDOWS
static BCRYPT_ALG_HANDLE hAlgorithm = NULL;
#endif

#ifdef PLATFORM_LINUX
static int urandom_fd = -1;
#endif

// ==================== 初始化函数 ====================

int random_init(void) {
#ifdef PLATFORM_WINDOWS
    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &hAlgorithm,
        BCRYPT_RNG_ALGORITHM,
        NULL,
        0
    );
    
    if (!BCRYPT_SUCCESS(status)) {
        fprintf(stderr, "BCryptOpenAlgorithmProvider failed: 0x%08lx\n", status);
        return -1;
    }
    
    return 0;
    
#elif defined(PLATFORM_LINUX)
    // 首先尝试打开 /dev/urandom（推荐使用，不会阻塞）
    urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd < 0) {
        fprintf(stderr, "Failed to open /dev/urandom: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
#endif
}

// ==================== 生成随机字节 ====================

int random_bytes(uint8_t *buffer, size_t length) {
    if (buffer == NULL || length == 0) {
        return -1;
    }
    
#ifdef PLATFORM_WINDOWS
    if (hAlgorithm == NULL) {
        fprintf(stderr, "Random generator not initialized\n");
        return -1;
    }
    
    NTSTATUS status = BCryptGenRandom(
        hAlgorithm,
        buffer,
        (ULONG)length,
        0
    );
    
    if (!BCRYPT_SUCCESS(status)) {
        fprintf(stderr, "BCryptGenRandom failed: 0x%08lx\n", status);
        return -1;
    }
    
    return 0;
    
#elif defined(PLATFORM_LINUX)
    if (urandom_fd < 0) {
        fprintf(stderr, "Random generator not initialized\n");
        return -1;
    }
    
    size_t bytes_read = 0;
    while (bytes_read < length) {
        ssize_t result = read(urandom_fd, buffer + bytes_read, length - bytes_read);
        
        if (result < 0) {
            if (errno == EINTR) {
                // 被信号中断，重试
                continue;
            }
            fprintf(stderr, "Failed to read from /dev/urandom: %s\n", strerror(errno));
            return -1;
        }
        
        if (result == 0) {
            fprintf(stderr, "Unexpected EOF from /dev/urandom\n");
            return -1;
        }
        
        bytes_read += result;
    }
    
    return 0;
#endif
}

// ==================== 清理函数 ====================

void random_cleanup(void) {
#ifdef PLATFORM_WINDOWS
    if (hAlgorithm != NULL) {
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        hAlgorithm = NULL;
    }
    
#elif defined(PLATFORM_LINUX)
    if (urandom_fd >= 0) {
        close(urandom_fd);
        urandom_fd = -1;
    }
#endif
}

// ==================== 生成范围内的随机整数 ====================

uint32_t random_range(uint32_t min, uint32_t max) {
    if (min > max) {
        // 交换min和max
        uint32_t temp = min;
        min = max;
        max = temp;
    }
    
    if (min == max) {
        return min;
    }
    
    uint32_t range = max - min + 1;
    
    // 计算需要多少位来表示range-1
    uint32_t range_minus_1 = range - 1;
    uint32_t bits_needed = 0;
    uint32_t temp = range_minus_1;
    while (temp > 0) {
        bits_needed++;
        temp >>= 1;
    }
    
    // 创建掩码，只保留需要的位数
    uint32_t mask = (1U << bits_needed) - 1;
    
    // 使用拒绝采样避免模运算偏差
    uint32_t random_value;
    do {
        uint8_t random_bytes_buf[4];
        if (random_bytes(random_bytes_buf, sizeof(random_bytes_buf)) != 0) {
            // 如果生成随机数失败，返回最小值
            return min;
        }
        
        // 将字节转换为uint32_t（小端序）
        random_value = ((uint32_t)random_bytes_buf[0]) |
                      ((uint32_t)random_bytes_buf[1] << 8) |
                      ((uint32_t)random_bytes_buf[2] << 16) |
                      ((uint32_t)random_bytes_buf[3] << 24);
        
        random_value &= mask;
    } while (random_value >= range);
    
    return min + random_value;
}
