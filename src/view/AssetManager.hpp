#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>

// Forward declaration : on prévient le compilateur que ces classes existent
// (Tu devras adapter ces noms selon comment tu as appelé tes classes 3D)
class Model3D; 
class Texture; 

class AssetManager {
private:
    // Le "Cache" : associe le chemin du fichier à son modèle chargé en mémoire
    std::unordered_map<std::string, std::shared_ptr<Model3D>> models;
    
    // On fait pareil pour les textures (images appliquées sur la 3D)
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, unsigned int> cubemaps;
    std::unordered_map<std::string, unsigned int> textures2D;

public:
    AssetManager() = default;
    ~AssetManager() { clear(); }

    // La fonction magique : charge le modèle SI il n'est pas déjà en cache
    std::shared_ptr<Model3D> getModel(const std::string& filepath);

    // Optionnel : la même chose pour tes images/textures
    // std::shared_ptr<Texture> getTexture(const std::string& filepath);

    unsigned int getCubemap(const std::string& filepath);
    unsigned int getTexture2D(const std::string& filepath);

    // Vide la mémoire (utile si tu veux tout réinitialiser)
    void clear();
};