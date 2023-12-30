#version 330 core
layout(location = 0) in vec3 aPos; // 位置输入
layout(location = 1) in vec3 aNormal; // 法向量输入
layout(location = 2) in vec2 aTexCoords; // 纹理坐标输入


out vec2 TexCoords;// 纹理坐标输出
out vec3 Normal;// 法向量输出
out vec3 FragPos; // 当前坐标在世界空间的位置

uniform mat4 model; // 模型变换矩阵
uniform mat4 view; // 视图变换矩阵
uniform mat4 projection; // 投影变换矩阵

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}