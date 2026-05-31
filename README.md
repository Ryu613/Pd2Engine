# Pd2Engine

Pd2Engine 是一款易于扩展，易于理解，易于调试的现代进阶级轻量化渲染引擎

## 特点

1. 基于C++23开发
2. 渲染底层目前采用Vulkan实现，但预留了其他backend的扩展性
3. 自行独立设计和研发，由于需要切实保证学习质量，文档及代码均为手写，未使用AI生成
4. 现代图形/渲染技术
5. 刻意避免单纯的功能堆叠，过度设计，可扩展，尽量精巧且易理解

## 功能点

1. 支持PBR渲染
2. 支持GLTF, KTX模型和材质的导入
3. render graph
4. ECS
5. 支持多线程渲染
6. Vulkan渲染底层

## 构建要求

目前项目只支持在Windows上构建, 并且主要在MSVC工具链上开发，对其他编译器尚未测试

### 需求

. Git
. Vcpkg
. CMake
. MSVC工具链

### 操作

1. git clone 本项目
2. 项目根目录下vcpkg install
3. 项目根目录下CMakeUserPresets.json.template去掉".template",并配置合适的参数
4. 使用CMake构建项目

### 如何使用

请参考samples文件夹

### 设计文档

见docs/index.adoc
