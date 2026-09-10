#include "cli.h"
#include "core/core.h"
#include "core/random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 显示帮助信息
void show_help(const char *program_name) {
    printf("===========================================\n");
    printf("  PasswdGen - 安全密码生成器\n");
    printf("===========================================\n\n");
    
    printf("使用方法:\n\n");
    
    printf("1. 基于权重生成密码:\n");
    printf("   %s -w <长度> <数字权重> <小写权重> <大写权重> <符号权重>\n\n", program_name);
    printf("   参数说明:\n");
    printf("     -w           : 使用权重模式\n");
    printf("     <长度>       : 密码总长度 (必须 > 0)\n");
    printf("     <数字权重>   : 数字字符权重 (0-100)\n");
    printf("     <小写权重>   : 小写字母权重 (0-100)\n");
    printf("     <大写权重>   : 大写字母权重 (0-100)\n");
    printf("     <符号权重>   : 符号字符权重 (0-100)\n\n");
    printf("   示例:\n");
    printf("     %s -w 16 25 25 25 25      # 生成16位均衡密码\n", program_name);
    printf("     %s -w 20 10 40 40 10      # 生成20位偏向字母的密码\n", program_name);
    printf("     %s -w 12 100 0 0 0        # 生成12位纯数字密码\n\n", program_name);
    
    printf("2. 基于指定长度生成密码:\n");
    printf("   %s -l <数字数> <小写数> <大写数> <符号数>\n\n", program_name);
    printf("   参数说明:\n");
    printf("     -l           : 使用指定长度模式\n");
    printf("     <数字数>     : 数字字符数量 (>= 0)\n");
    printf("     <小写数>     : 小写字母数量 (>= 0)\n");
    printf("     <大写数>     : 大写字母数量 (>= 0)\n");
    printf("     <符号数>     : 符号字符数量 (>= 0)\n\n");
    printf("   示例:\n");
    printf("     %s -l 3 4 4 3             # 3数字+4小写+4大写+3符号\n", program_name);
    printf("     %s -l 0 6 6 0             # 6小写+6大写(纯字母)\n", program_name);
    printf("     %s -l 6 0 0 0             # 6位数字PIN码\n\n", program_name);
    
    printf("3. 批量生成 (可与 -w 或 -l 组合，位置任意):\n");
    printf("   %s -n <数量> -w <...>    或    %s -l <...> -n <数量>\n\n", program_name, program_name);
    printf("   参数说明:\n");
    printf("     -n           : 一次生成的密码数量 (1-500，默认1)\n\n");
    printf("   示例:\n");
    printf("     %s -n 10 -w 16 25 25 25 25   # 批量生成10个16位均衡密码\n", program_name);
    printf("     %s -l 3 4 4 3 -n 5           # 批量生成5个密码\n\n", program_name);

    printf("4. 显示帮助信息:\n");
    printf("   %s -h\n", program_name);
    printf("   %s --help\n\n", program_name);
    
    printf("字符集说明:\n");
    printf("  数字: 0-9\n");
    printf("  小写: a-z\n");
    printf("  大写: A-Z\n");
    printf("  符号: !@#$%%^&*()-_=+[]{}|;:,.<>?/~`\n\n");
    
    printf("注意事项:\n");
    printf("  - 权重模式: 至少一个权重必须大于0\n");
    printf("  - 长度模式: 至少一个数量必须大于0\n");
    printf("  - 生成的密码会自动随机打乱字符顺序\n");
    printf("  - 使用系统安全随机数生成器确保密码安全性\n\n");
}

// 检查字符串是否为有效数字
static int is_valid_number(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    
    return 1;
}

// 解析权重模式参数
static int parse_weight_mode(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "错误: 权重模式需要6个参数\n");
        fprintf(stderr, "使用 '%s --help' 查看帮助信息\n", argv[0]);
        return -1;
    }
    
    // 验证所有参数都是数字
    for (int i = 2; i < 7; i++) {
        if (!is_valid_number(argv[i])) {
            fprintf(stderr, "错误: 参数 '%s' 不是有效的数字\n", argv[i]);
            return -1;
        }
    }
    
    // 解析参数
    size_t length = (size_t)atoi(argv[2]);
    uint32_t digit_weight = (uint32_t)atoi(argv[3]);
    uint32_t lower_weight = (uint32_t)atoi(argv[4]);
    uint32_t upper_weight = (uint32_t)atoi(argv[5]);
    uint32_t symbol_weight = (uint32_t)atoi(argv[6]);
    
    // 参数验证
    if (length == 0) {
        fprintf(stderr, "错误: 密码长度必须大于0\n");
        return -1;
    }
    
    if (length > 1000) {
        fprintf(stderr, "错误: 密码长度不能超过1000\n");
        return -1;
    }
    
    if (digit_weight > 100 || lower_weight > 100 || 
        upper_weight > 100 || symbol_weight > 100) {
        fprintf(stderr, "错误: 权重值必须在0-100之间\n");
        return -1;
    }
    
    // 分配缓冲区
    char *password = (char *)malloc(length + 1);
    if (password == NULL) {
        fprintf(stderr, "错误: 内存分配失败\n");
        return -1;
    }
    
    // 生成密码
    int result = generate_password_by_weight(
        password, length,
        digit_weight, lower_weight, upper_weight, symbol_weight
    );
    
    if (result == 0) {
        printf("%s\n", password);
    }
    
    free(password);
    return result;
}

// 解析长度模式参数
static int parse_length_mode(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "错误: 长度模式需要5个参数\n");
        fprintf(stderr, "使用 '%s --help' 查看帮助信息\n", argv[0]);
        return -1;
    }
    
    // 验证所有参数都是数字
    for (int i = 2; i < 6; i++) {
        if (!is_valid_number(argv[i])) {
            fprintf(stderr, "错误: 参数 '%s' 不是有效的数字\n", argv[i]);
            return -1;
        }
    }
    
    // 解析参数
    size_t digit_count = (size_t)atoi(argv[2]);
    size_t lower_count = (size_t)atoi(argv[3]);
    size_t upper_count = (size_t)atoi(argv[4]);
    size_t symbol_count = (size_t)atoi(argv[5]);
    
    // 参数验证
    size_t total_length = digit_count + lower_count + upper_count + symbol_count;
    
    if (total_length == 0) {
        fprintf(stderr, "错误: 至少需要生成1个字符\n");
        return -1;
    }
    
    if (total_length > 1000) {
        fprintf(stderr, "错误: 密码总长度不能超过1000\n");
        return -1;
    }
    
    // 分配缓冲区
    char *password = (char *)malloc(total_length + 1);
    if (password == NULL) {
        fprintf(stderr, "错误: 内存分配失败\n");
        return -1;
    }
    
    // 生成密码
    int result = generate_password_by_length(
        password,
        digit_count, lower_count, upper_count, symbol_count
    );
    
    if (result == 0) {
        printf("%s\n", password);
    }
    
    free(password);
    return result;
}

// 从参数列表中提取 -n/--count，返回生成数量，并将剩余参数紧凑到 filtered 中
static int extract_count(int argc, char *argv[], char **filtered, int *out_argc) {
    int count = 1;
    int j = 0;
    filtered[j++] = argv[0];
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--count") == 0) {
            if (i + 1 >= argc || !is_valid_number(argv[i + 1])) {
                fprintf(stderr, "错误: -n 参数需要一个有效的数字\n");
                return -1;
            }
            count = atoi(argv[++i]);
            if (count < 1 || count > 500) {
                fprintf(stderr, "错误: 生成数量必须在1-500之间\n");
                return -1;
            }
        } else {
            filtered[j++] = argv[i];
        }
    }
    *out_argc = j;
    return count;
}

// 主解析函数
int parse_and_execute(int argc, char *argv[]) {
    if (argc == 1) {
        show_help(argv[0]);
        return 1;
    }

    // 检查帮助参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help(argv[0]);
            return 1;
        }
    }

    // 提取 -n 参数（MSVC 不支持 C11 VLA，改用动态分配）
    char **filtered = malloc((size_t)argc * sizeof(char *));
    if (filtered == NULL) {
        fprintf(stderr, "错误: 内存分配失败\n");
        return -1;
    }
    int new_argc = 0;
    int count = extract_count(argc, argv, filtered, &new_argc);
    if (count < 0) {
        free(filtered);
        return -1;
    }

    if (new_argc == 1) {
        show_help(argv[0]);
        free(filtered);
        return 1;
    }

    // 初始化随机数生成器
    if (random_init() != 0) {
        fprintf(stderr, "错误: 随机数生成器初始化失败\n");
        free(filtered);
        return -1;
    }

    int result = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(filtered[1], "-w") == 0 || strcmp(filtered[1], "--weight") == 0) {
            result = parse_weight_mode(new_argc, filtered);
        } else if (strcmp(filtered[1], "-l") == 0 || strcmp(filtered[1], "--length") == 0) {
            result = parse_length_mode(new_argc, filtered);
        } else {
            fprintf(stderr, "错误: 未知参数 '%s'\n", filtered[1]);
            fprintf(stderr, "使用 '%s --help' 查看帮助信息\n", argv[0]);
            result = -1;
        }
        if (result != 0) break;
    }

    random_cleanup();
    free(filtered);
    return result;
}
