#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>

class Model3D; 
class Texture; 

class AssetManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Model3D>> models;
    
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, unsigned int> cubemaps;
    std::unordered_map<std::string, unsigned int> textures2D;

public:
    AssetManager() = default;
    ~AssetManager() { clear(); }

    std::shared_ptr<Model3D> getModel(const std::string& filepath);

    unsigned int getCubemap(const std::string& filepath);
    unsigned int getTexture2D(const std::string& filepath);
    void clear();
};