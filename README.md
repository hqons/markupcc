# MarkupCC

`mkcc` 是一个用于将自定义标记语言（MKML）转换为 [SFML](https://www.sfml-dev.org/) C++ 项目的命令行工具。它提供了从模板初始化、编译到运行的完整流程，帮助你用类 HTML 的语法快速创建桌面图形界面程序。

## 功能

- **`mkcc init`**：在当前目录生成 `.mkcc`、`mkccmake.json` 和 `makefile` 等基础模板。
- **`mkcc make`**：根据 `mkccmake.json` 的配置解析 MKML 文件，生成并编译 C++ 代码。
- **`mkcc run`**：运行构建出的二进制文件，若不存在则自动执行 `make`。
- **`mkcc release`**：预留的打包命令（开发中）。
- **`mkcc help`**：显示命令帮助。

`mkcc_doc.py` 脚本可以扫描 `/*back_start*/ ... /*back_end*/` 区块中的 `MKML` 宏，生成位于 `docs/` 下的 JSON 与 HTML 文档。

## 构建

### 依赖

- C++17 编译器（如 `g++`）
- [CMake](https://cmake.org/)
- [libxml2](http://xmlsoft.org/)
- [nlohmann/json](https://github.com/nlohmann/json)（头文件）
- [SFML](https://www.sfml-dev.org/)（用于构建生成的项目）

### 生成可执行文件

```bash
cmake -S . -B build
cmake --build build
```

构建完成后，可执行文件位于 `build/mkcc`（或对应平台的输出路径）。你也可以运行 `make` 使用生成的 Makefile。

## 使用

在拥有 `mkcc` 的环境中（可通过执行 `setup.sh` 安装到 `/usr/bin`），在你的项目目录执行：

```bash
mkcc init             # 初始化模板
mkcc make             # 编译 MKML
mkcc run              # 运行程序
```

项目的配置位于生成的 `mkccmake.json` 中，通常包含 `name`、`version`、`entry`（入口 MKML 文件）和 `output`（构建目录）等字段。

## 生成文档

```bash
python mkcc_doc.py core
```

脚本会在 `docs/` 目录下生成 `mkcc_doc.json` 与 `mkcc_doc.html`，便于查看支持的 `MKML` 宏。

## 许可证

本项目基于 [MIT License](LICENSE) 发行。

