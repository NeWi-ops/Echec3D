#include "AssetManager.hpp"
#include "Model3D.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

std::shared_ptr<Model3D> AssetManager::getModel(const std::string& filepath) {
    // 1. On cherche si ce fichier a DÉJÀ été chargé auparavant
    auto it = models.find(filepath);

    if (it != models.end()) {
        // TROUVÉ ! On renvoie simplement le pointeur vers la mémoire existante.
        // Cela prend 0 milliseconde et ne consomme pas de VRAM supplémentaire.
        return it->second; 
    }

    // 2. NON TROUVÉ. C'est la première fois qu'on demande ce fichier.
    std::cout << "[AssetManager] Chargement du nouveau modèle : " << filepath << std::endl;

    // Tu dois remplacer cette ligne par la vraie façon dont tu instancies ton modèle.
    // Exemple : std::shared_ptr<Model3D> newModel = std::make_shared<Model3D>(filepath);
    std::shared_ptr<Model3D> newModel = std::make_shared<Model3D>(filepath); 

    // 3. On l'enregistre dans notre dictionnaire pour la prochaine fois
    if (newModel) {
        models[filepath] = newModel;
    } else {
        std::cerr << "[AssetManager] ERREUR : Impossible de charger " << filepath << std::endl;
    }

    return newModel;
}

unsigned int AssetManager::getCubemap(const std::string& filepath) {
    if (cubemaps.find(filepath) != cubemaps.end()) {
        return cubemaps[filepath];
    }

    std::cout << "[AssetManager] Chargement du cubemap : " << filepath << std::endl;

    stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "[AssetManager] ERREUR : Impossible de charger le cubemap " << filepath << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int faceSize = width / 4; 
    if (height / 3 != faceSize) {
        std::cerr << "[AssetManager] ATTENTION : Le cubemap n'a pas un ratio de croix horizontale standard (4x3). " << std::endl;
    }

    int faceCols[6] = {2, 0, 1, 1, 1, 3};
    int faceRows[6] = {1, 1, 0, 2, 1, 1}; 

    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

    for (unsigned int i = 0; i < 6; i++) {
        int xOffset = faceCols[i] * faceSize;
        int yOffset = faceRows[i] * faceSize;

        glPixelStorei(GL_UNPACK_SKIP_PIXELS, xOffset);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, yOffset);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                     0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, data);
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubemaps[filepath] = textureID;
    return textureID;
}

unsigned int AssetManager::getTexture2D(const std::string& filepath) {
    if (textures2D.find(filepath) != textures2D.end()) {
        return textures2D[filepath];
    }

    std::cout << "[AssetManager] Chargement texture : " << filepath << std::endl;
    stbi_set_flip_vertically_on_load(true); // Usually true for OBJ and regular 3D engine textures

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "[AssetManager] ERREUR texture " << filepath << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    textures2D[filepath] = textureID;
    return textureID;
}

void AssetManager::clear() {
    // std::shared_ptr va automatiquement détruire les modèles si plus personne ne s'en sert
    models.clear();
    textures.clear();
    for (auto& pair : cubemaps) {
        glDeleteTextures(1, &pair.second);
    }
    cubemaps.clear();
    for (auto& pair : textures2D) {
        glDeleteTextures(1, &pair.second);
    }
    textures2D.clear();
    std::cout << "[AssetManager] Memoire nettoyee." << std::endl;
}