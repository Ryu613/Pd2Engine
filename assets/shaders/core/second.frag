#version 450

// 从上一个shader输入进来的fragColor这里接上
layout(location = 0) in vec3 fragColor;

// 本shader要输出的变量
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}