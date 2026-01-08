#include <stdlib.h>
#include "cli.h"

int main(int argc, char *argv[]) {
    int result = parse_and_execute(argc, argv);
    
    // 返回码: 0=成功, 1=显示帮助, -1=错误
    if (result == -1) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
