#include "./assetManager.hpp"
#include <iostream>

void AssetManager::registerMesh(const std::string &name, unsigned int vao,
                                unsigned int count) {
  m_meshes[name] = {vao, count};
}

Mesh AssetManager::getMesh(const std::string &name) {
  if (m_meshes.find(name) == m_meshes.end()) {
    return m_meshes["default_cube"]; // Fallback pour éviter le crash
  }
  return m_meshes[name];
}

void AssetManager::registerTexture(const std::string &name,
                                   unsigned int textureID) {
  m_textures[name] = textureID;
}

unsigned int AssetManager::getTexture(const std::string &name) {
  return m_textures.count(name) ? m_textures[name] : 0;
}