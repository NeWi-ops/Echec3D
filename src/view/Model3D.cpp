#include "Model3D.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

Model3D::Model3D(const std::string& filepath) {
    isLoaded = false;
    vertexCount = 0;
    VAO = 0;
    VBO = 0;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<float> final_data; // Données finales pour OpenGL (Position + Normale)

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Model3D] Fichier introuvable : " << filepath << " -> Cube de secours active." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") { // Sommet (Position)
            glm::vec3 v;
            iss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        } 
        else if (type == "vn") { // Normale (Lumière)
            glm::vec3 vn;
            iss >> vn.x >> vn.y >> vn.z;
            temp_normals.push_back(vn);
        } 
        else if (type == "f") { // Face (Triangle)
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;

            std::string vertices[3] = {vertex1, vertex2, vertex3};
            
            // Décodage des indices (gère les formats v/vt/vn, v//vn ou juste v)
            for (int i = 0; i < 3; ++i) {
                int vIdx = 0, vtIdx = 0, vnIdx = 0;
                
                if (vertices[i].find("//") != std::string::npos) {
                    sscanf(vertices[i].c_str(), "%d//%d", &vIdx, &vnIdx);
                } else if (vertices[i].find("/") != std::string::npos) {
                    sscanf(vertices[i].c_str(), "%d/%d/%d", &vIdx, &vtIdx, &vnIdx);
                } else {
                    sscanf(vertices[i].c_str(), "%d", &vIdx);
                }

                // En format OBJ, les indices commencent à 1. En C++, à 0.
                if (vIdx > 0 && vIdx <= temp_vertices.size()) {
                    glm::vec3 vertex = temp_vertices[vIdx - 1];
                    final_data.push_back(vertex.x);
                    final_data.push_back(vertex.y);
                    final_data.push_back(vertex.z);
                }

                if (vnIdx > 0 && vnIdx <= temp_normals.size()) {
                    glm::vec3 normal = temp_normals[vnIdx - 1];
                    final_data.push_back(normal.x);
                    final_data.push_back(normal.y);
                    final_data.push_back(normal.z);
                } else {
                    // Normale par défaut si le fichier n'en a pas
                    final_data.push_back(0.0f); final_data.push_back(1.0f); final_data.push_back(0.0f);
                }
            }
        }
    }

    if (final_data.empty()) return;

    // --- ENVOI À LA CARTE GRAPHIQUE (OPENGL) ---
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, final_data.size() * sizeof(float), final_data.data(), GL_STATIC_DRAW);

    // Attribut 0 : Position (3 floats, saut de 6)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribut 1 : Normale (3 floats, saut de 6, décalage de 3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vertexCount = final_data.size() / 6; 
    isLoaded = true;
    std::cout << "[Model3D] Succes ! " << filepath << " (" << vertexCount / 3 << " triangles)" << std::endl;
}

Model3D::~Model3D() {
    if (isLoaded) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Model3D::draw() const {
    if (!isLoaded) return;
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}