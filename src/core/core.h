#ifndef PASSWDGEN_CORE_H
#define PASSWDGEN_CORE_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 字符类型定义
 */
#define CHARSET_DIGITS      "0123456789"
#define CHARSET_LOWERCASE   "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_UPPERCASE   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_SYMBOLS     "!@#$%^&*()-_=+[]{}|;:,.<>?/~`"

/**
 * @brief 根据权重生成随机密码
 * 
 * 根据给定的总长度和各字符类型的权重，生成由纯ASCII字符组成的随机字符串。
 * 权重值决定了每种字符类型在最终密码中的大致比例。
 * 
 * @param output        输出缓冲区（需要预先分配足够空间，至少length+1字节）
 * @param length        生成密码的总长度
 * @param digit_weight  数字字符的权重（0-100）
 * @param lower_weight  小写字母的权重（0-100）
 * @param upper_weight  大写字母的权重（0-100）
 * @param symbol_weight 符号字符的权重（0-100）
 * 
 * @return 0表示成功，-1表示失败
 * 
 * @note 权重不需要加起来等于100，函数会自动计算比例
 * @note 如果所有权重都为0，函数将返回错误
 * @note 生成的密码会被随机打乱，不会按权重顺序排列
 */
int generate_password_by_weight(
    char *output,
    size_t length,
    uint32_t digit_weight,
    uint32_t lower_weight,
    uint32_t upper_weight,
    uint32_t symbol_weight
);

/**
 * @brief 根据指定长度生成随机密码
 * 
 * 根据给定的各字符类型的具体数量，生成由纯ASCII字符组成的随机字符串。
 * 
 * @param output        输出缓冲区（需要预先分配足够空间，至少(digit_count+lower_count+upper_count+symbol_count+1)字节）
 * @param digit_count   数字字符的数量
 * @param lower_count   小写字母的数量
 * @param upper_count   大写字母的数量
 * @param symbol_count  符号字符的数量
 * 
 * @return 0表示成功，-1表示失败（实际生成的总长度，失败返回-1）
 * 
 * @note 生成的密码总长度 = digit_count + lower_count + upper_count + symbol_count
 * @note 如果所有计数都为0，函数将返回错误
 * @note 生成的密码会被随机打乱，不会按字符类型顺序排列
 */
int generate_password_by_length(
    char *output,
    size_t digit_count,
    size_t lower_count,
    size_t upper_count,
    size_t symbol_count
);

#endif // PASSWDGEN_CORE_H
