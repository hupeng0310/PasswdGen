cmake_minimum_required(VERSION 3.29.2)
project(PasswdGen C)

# 设置C标准为C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 源文件列表
set(SOURCES
    src/main.c
    src/cli.c
    src/core/random.c
    src/core/core.c
)

# 头文件目录
include_directories(src)

# 创建可执行文件
add_executable(PasswdGen ${SOURCES})

# Windows平台特定配置
if(WIN32)
    # 链接bcrypt库（Windows安全随机数API）
    target_link_libraries(PasswdGen bcrypt)
endif()

# Linux平台特定配置（不需要额外链接库）
if(UNIX AND NOT APPLE)
    # /dev/urandom 是Linux标准API，无需额外链接
endif()
