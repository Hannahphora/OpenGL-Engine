#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <atomic>

unsigned int loadTextureFromFile(const char* path, const std::string& directory);

class Model {
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection = false;

    Model(std::string const& path);
    void Draw(ShaderProgram& shader);

private:
    void loadModel(const std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};