#include "core.h"
#include "random.h"
#include <string.h>
#include <stdio.h>

// 辅助函数：从字符集中随机选择一个字符
static char random_char_from_set(const char *charset) {
    size_t charset_len = strlen(charset);
    if (charset_len == 0) {
        return '\0';
    }
    uint32_t index = random_range(0, (uint32_t) (charset_len - 1));
    return charset[index];
}

// 辅助函数：Fisher-Yates洗牌算法，随机打乱字符串
static void shuffle_string(char *str, size_t length) {
    if (length <= 1) {
        return;
    }

    for (size_t i = length - 1; i > 0; i--) {
        uint32_t j = random_range(0, (uint32_t) i);
        // 交换str[i]和str[j]
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

// ==================== 根据权重生成密码 ====================

int generate_password_by_weight(
        char *output,
        size_t length,
        uint32_t digit_weight,
        uint32_t lower_weight,
        uint32_t upper_weight,
        uint32_t symbol_weight
) {
    if (output == NULL || length == 0) {
        fprintf(stderr, "错误: 无效的输出缓冲区或长度\n");
        return -1;
    }

    // 计算总权重
    uint32_t total_weight = digit_weight + lower_weight + upper_weight + symbol_weight;
    if (total_weight == 0) {
        fprintf(stderr, "错误: 所有权重不能都为0\n");
        return -1;
    }

    // 根据权重计算每种字符类型的数量
    size_t digit_count = 0;
    size_t lower_count = 0;
    size_t upper_count = 0;
    size_t symbol_count = 0;

    // 按比例分配字符数量
    if (digit_weight > 0) {
        digit_count = (size_t) ((double) length * digit_weight / total_weight);
    }
    if (lower_weight > 0) {
        lower_count = (size_t) ((double) length * lower_weight / total_weight);
    }
    if (upper_weight > 0) {
        upper_count = (size_t) ((double) length * upper_weight / total_weight);
    }
    if (symbol_weight > 0) {
        symbol_count = (size_t) ((double) length * symbol_weight / total_weight);
    }

    // 处理由于整数除法导致的差值
    size_t current_total = digit_count + lower_count + upper_count + symbol_count;
    if (current_total < length) {
        size_t diff = length - current_total;
        // 将剩余的字符随机分配给有权重的类型
        for (size_t i = 0; i < diff; i++) {
            uint32_t rand_type = random_range(0, 3);
            switch (rand_type) {
                case 0:
                    if (digit_weight > 0) digit_count++;
                    else i--; // 重新抽取
                    break;
                case 1:
                    if (lower_weight > 0) lower_count++;
                    else i--;
                    break;
                case 2:
                    if (upper_weight > 0) upper_count++;
                    else i--;
                    break;
                case 3:
                    if (symbol_weight > 0) symbol_count++;
                    else i--;
                    break;
            }
        }
    } else if (current_total > length) {
        // 减少多出来的字符（从最大的类型中减）
        while (current_total > length) {
            if (digit_count > 0 && digit_count >= lower_count &&
                digit_count >= upper_count && digit_count >= symbol_count) {
                digit_count--;
            } else if (lower_count > 0 && lower_count >= upper_count &&
                       lower_count >= symbol_count) {
                lower_count--;
            } else if (upper_count > 0 && upper_count >= symbol_count) {
                upper_count--;
            } else if (symbol_count > 0) {
                symbol_count--;
            }
            current_total--;
        }
    }

    // 生成密码
    size_t pos = 0;

    // 添加数字
    for (size_t i = 0; i < digit_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_DIGITS);
    }

    // 添加小写字母
    for (size_t i = 0; i < lower_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_LOWERCASE);
    }

    // 添加大写字母
    for (size_t i = 0; i < upper_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_UPPERCASE);
    }

    // 添加符号
    for (size_t i = 0; i < symbol_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_SYMBOLS);
    }

    // 添加字符串结束符
    output[pos] = '\0';

    // 随机打乱密码
    shuffle_string(output, length);

    return 0;
}

// ==================== 根据指定长度生成密码 ====================

int generate_password_by_length(
        char *output,
        size_t digit_count,
        size_t lower_count,
        size_t upper_count,
        size_t symbol_count
) {
    if (output == NULL) {
        fprintf(stderr, "错误: 无效的输出缓冲区\n");
        return -1;
    }

    size_t total_length = digit_count + lower_count + upper_count + symbol_count;
    if (total_length == 0) {
        fprintf(stderr, "错误: 所有长度不能都为0\n");
        return -1;
    }

    size_t pos = 0;

    // 添加数字
    for (size_t i = 0; i < digit_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_DIGITS);
    }

    // 添加小写字母
    for (size_t i = 0; i < lower_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_LOWERCASE);
    }

    // 添加大写字母
    for (size_t i = 0; i < upper_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_UPPERCASE);
    }

    // 添加符号
    for (size_t i = 0; i < symbol_count; i++) {
        output[pos++] = random_char_from_set(CHARSET_SYMBOLS);
    }

    // 添加字符串结束符
    output[pos] = '\0';

    // 随机打乱密码
    shuffle_string(output, total_length);

    return 0;
}
