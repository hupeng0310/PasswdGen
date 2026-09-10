# AGENTS.md

本文件为 AI 助手在本仓库中工作时提供指引。

## 构建指令

支持 MSVC（当前主力工具链，CLion 配置）、MinGW 和 Linux GCC。

### MSVC（Windows）

命令行构建需先加载 MSVC 环境（cmd/PowerShell 中执行）：

```cmd
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

- 输出: `build-release/PasswdGen.exe`
- Ninja 是单配置生成器，构建类型由 `-DCMAKE_BUILD_TYPE` 决定；MSVC 的 cl.exe 不支持 C11 变长数组（VLA），代码中不要引入 VLA
- 使用 `Visual Studio` 多配置生成器时，编译命令需加 `--config Release`

### MinGW（Windows）

```bash
# 配置 Release 构建
cmake -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build-release
```

- 输出: `build-release/PasswdGen.exe`

### Linux (GCC)

```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

- 输出: `build-release/PasswdGen`
- 随机数使用 `/dev/urandom`，无需额外链接库

Debug 构建使用 CLion 默认的 `cmake-build-debug/` 目录。

## 测试

在项目根目录运行 `test_cli.bat`（仅限 Windows，使用 debug 构建）：
```cmd
test_cli.bat
```
未使用单元测试框架，通过批处理脚本调用 CLI 进行测试。

## 架构

C11 项目，三层架构，严格自顶向下依赖：

```
main.c → cli.c → core/core.c → core/random.c
                ↘ core/random.c (生命周期管理)
```

- **cli.c** — 解析命令行参数、校验输入、管理随机子系统生命周期（初始化/清理）、分配输出缓冲区、调用生成函数
- **core/core.c** — 两种密码生成算法：基于权重的比例分配和精确数量指定，均使用 Fisher-Yates 洗牌。仅消费随机原语，不管理其生命周期
- **core/random.c** — 跨平台安全随机数生成。Windows: BCrypt API（`bcrypt` 库）；Linux: `/dev/urandom`。`random_range()` 使用拒绝采样避免取模偏差

## 平台注意事项

- 源码文件为 UTF-8 编码。Windows cmd/PowerShell 默认使用 GBK（代码页 936），会导致中文输出乱码。可通过 `chcp 65001` 或在代码中调用 `SetConsoleOutputCP(65001)` 解决。
- Windows 需链接 `bcrypt` 库；Linux 无需额外库。
