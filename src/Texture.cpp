//
// Created by Wangyx on 2023/12/25.
//

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureInf::TextureInf(const std::string& texturePath) {

    // 生成纹理
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureInf::use() const {
    glBindTexture(GL_TEXTURE_2D, ID);
}

unsigned int TextureInf::getID() {
    return ID;
}
