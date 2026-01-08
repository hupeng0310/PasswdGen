#ifndef PASSWDGEN_CLI_H
#define PASSWDGEN_CLI_H

/**
 * @brief 解析命令行参数并执行相应的密码生成操作
 * 
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 0表示成功，-1表示失败，1表示显示帮助后退出
 */
int parse_and_execute(int argc, char *argv[]);

/**
 * @brief 显示帮助信息
 */
void show_help(const char *program_name);

#endif // PASSWDGEN_CLI_H
