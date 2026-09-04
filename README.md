# Pd2Engine(WIP)

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](License.txt)

Pd2Engine 是一款易于理解，易于开发，易于调试的现代C++3D实时渲染引擎(WIP)

## 特点

1. 基于现代C++(23)开发
1. 独立从零开始自学，设计，和开发，为了切实保证学习和工程能力，文档及代码为完全手写，未使用AI生成
1. 采用现代图形/引擎技术,但技术选型偏稳健务实，不激进
1. 定位高于一般的教学级引擎，但不过度复杂，期望是一个具备现代引擎参考价值同时又务实的good engine starter example

## 功能点

1. PBR渲染
2. 支持GLTF模型
3. 数据驱动，ECS
4. render graph
5. 多线程job system
6. Vulkan底层封装
7. 已预留跨平台，跨图形API, 跨窗口系统的扩展性

## 构建要求

目前项目只支持在Windows，MSVC工具链上构建和开发，其他编译器和平台预留扩展性，但目前尚不支持

### 环境要求

1. Git(clone即可，不带submodule)
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

目前为编程式实现，不支持脚本，暂时没有编辑器，但提供了场景描述接口，请参考samples文件夹(TODO)

### 设计文档

见[docs/index.adoc](./docs/index.adoc)

> 项目代码为本人原创作品，遵循Apache 2.0 协议
> 设计文档为本人原创作品，遵循CC-BY 4.0 协议