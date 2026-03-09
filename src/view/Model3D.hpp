#pragma once

#include <string>
#include <glad/glad.h>

class Model3D {
private:
    unsigned int VAO, VBO;
    int vertexCount;
    bool isLoaded;

public:
    // Le constructeur prendra le chemin vers le fichier .obj
    Model3D(const std::string& filepath);
    ~Model3D();

    // La fameuse fonction que Scene3D essaie d'appeler
    void draw() const;
    
    bool getIsLoaded() const { return isLoaded; }
};