//
// Created by Wangyx on 2023/12/27.
//

#ifndef  MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{

private:
    /*  模型数据  */
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
    /*  函数   */
    void loadModel(std::string const& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                 std::string typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory);

public:
    /*  函数   */
    explicit Model(std::string const &path, bool gamma = false);
    void Draw(Shader shader);

};

#endif //MODEL_H
