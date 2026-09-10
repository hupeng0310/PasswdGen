#include <stdlib.h>
#include "cli.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
    // Windows 控制台默认使用 GBK（代码页936），切换为 UTF-8 以正确显示中文输出
    SetConsoleOutputCP(CP_UTF8);
#endif

    int result = parse_and_execute(argc, argv);
    
    // 返回码: 0=成功, 1=显示帮助, -1=错误
    if (result == -1) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
