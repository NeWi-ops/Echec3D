#pragma once

#include <string>
#include <glad/glad.h>

class Model3D {
private:
    unsigned int VAO, VBO;
    int vertexCount;
    bool isLoaded;

public:
    Model3D(const std::string& filepath);
    ~Model3D();

    void draw() const;
    
    bool getIsLoaded() const { return isLoaded; }
};