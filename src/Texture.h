//
// Created by Wangyx on 2023/12/25.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class TextureInf {
private:
    unsigned int ID;

public:
    explicit TextureInf(const std::string& texturePath);
    void use() const;
    unsigned int getID();

};


#endif //TEXTURE_H
