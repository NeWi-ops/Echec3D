#pragma once
#include <map>
#include <memory>
#include <string>

// Structure pour stocker les infos d'un modèle 3D
struct Mesh {
  unsigned int VAO;
  unsigned int vertexCount;
};

class AssetManager {
public:
  static AssetManager &instance() {
    static AssetManager inst;
    return inst;
  }

  // Gestion des Modèles (Meshes)
  void registerMesh(const std::string &name, unsigned int vao,
                    unsigned int count);
  Mesh getMesh(const std::string &name);

  // Gestion des Textures
  void registerTexture(const std::string &name, unsigned int textureID);
  unsigned int getTexture(const std::string &name);

private:
  AssetManager() = default;
  std::map<std::string, Mesh> m_meshes;
  std::map<std::string, unsigned int> m_textures;
};