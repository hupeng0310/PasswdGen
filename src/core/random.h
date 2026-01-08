#ifndef PASSWDGEN_RANDOM_H
#define PASSWDGEN_RANDOM_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 初始化随机数生成器
 * 
 * 该函数会根据不同平台进行相应的初始化操作
 * - Windows: 初始化BCryptOpenAlgorithmProvider
 * - Linux: 打开/dev/urandom文件描述符
 * 
 * @return 0表示成功，-1表示失败
 */
int random_init(void);

/**
 * @brief 生成指定长度的安全随机字节
 * 
 * 使用系统提供的安全随机数生成器填充缓冲区
 * - Windows: 使用BCryptGenRandom API
 * - Linux: 从/dev/urandom读取随机数据
 * 
 * @param buffer 用于存储随机字节的缓冲区
 * @param length 需要生成的随机字节数量
 * @return 0表示成功，-1表示失败
 */
int random_bytes(uint8_t *buffer, size_t length);

/**
 * @brief 清理随机数生成器资源
 * 
 * 释放随机数生成器占用的系统资源
 * - Windows: 关闭BCrypt算法提供者句柄
 * - Linux: 关闭/dev/urandom文件描述符
 */
void random_cleanup(void);

/**
 * @brief 生成指定范围内的随机整数
 * 
 * 生成一个[min, max]范围内的随机整数（包含边界）
 * 使用拒绝采样方法避免模运算偏差
 * 
 * @param min 最小值（包含）
 * @param max 最大值（包含）
 * @return 范围内的随机整数，失败返回min
 */
uint32_t random_range(uint32_t min, uint32_t max);

#endif // PASSWDGEN_RANDOM_H
