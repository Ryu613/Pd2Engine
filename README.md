# Pd2Engine(WIP)

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](License.txt)

Pd2Engine 是一款易于理解，易于开发，易于调试的现代C++3D实时渲染引擎(WIP)

## 特点

1. 基于现代C++(23)开发
1. 独立自学，设计，和开发，为了切实保证学习和工程能力，文档及代码为完全手写，未使用AI生成
1. 现代图形/渲染技术
1. 定位为进阶到高级水平
1. 小到中型规模的工程级项目

## 功能点

1. PBR渲染
2. 支持GLTF, KTX模型和材质的导入
3. 数据驱动，ECS
4. render graph
5. 多线程
6. Vulkan API
7. 预留了跨平台，跨渲染后端扩展性

## 构建要求

目前项目只支持在Windows， MSVC工具链上构建和开发，其他编译器和平台预留扩展性，但目前尚不支持

### 环境要求

1. Git
1. Vcpkg
1. CMake 3.31
1. MSVC build tools
1. (可选但推荐)asciidoc(vscode的asciidoctor插件并打开kroki预览功能)
1. (可选但推荐)doxygen + doxybook2 + mkdocs
1. (可选但推荐)clangd ≥21

### 操作

1. git clone 本项目
2. 项目根目录下vcpkg install
3. 项目根目录下CMakeUserPresets.json.template去掉".template",并配置合适的参数
4. 使用CMake构建项目

### 如何使用

请参考samples文件夹(TODO)

### 设计文档

见[docs/index.adoc](./docs/index.adoc)
