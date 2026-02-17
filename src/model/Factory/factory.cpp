#include "./factory.hpp"
#include <iostream>

// Le Singleton qui contient la Map
std::map<std::string, PieceFactory::CreatorFunc> &PieceFactory::getRegistry() {
  static std::map<std::string, CreatorFunc> registry;
  return registry;
}

void PieceFactory::registerPiece(const std::string &key, CreatorFunc creator) {
  // On enregistre la clé telle quelle (ex: "Paladin")
  auto &registry = getRegistry();
  std::cout << "[FACTORY] Enregistrement de la piece : " << key << std::endl;
  if (registry.find(key) != registry.end()) {
    std::cerr << "[WARNING] La piece '" << key
              << "' est deja enregistree ! Elle sera remplacee." << std::endl;
  }

  registry[key] = creator;
  // std::cout << "[FACTORY] Piece enregistree : " << key << std::endl;
}

std::unique_ptr<Piece> PieceFactory::createPiece(const std::string &key,
                                                 PieceColor color) {
  auto &registry = getRegistry();

  // On cherche la clé exacte
  auto it = registry.find(key);

  if (it != registry.end()) {
    // On appelle la fonction de création stockée
    std::cout << "[FACTORY] Creation de la piece : " << key << std::endl;
    return it->second(color);
  }

  std::cerr << "[ERROR] PieceFactory : Type de piece inconnu -> " << key
            << std::endl;
  return nullptr;
}

void PieceFactory::printRegisteredPieces() {
  auto &registry = getRegistry();
  std::cout << "--- Pieces connues par l'usine ---" << std::endl;
  for (const auto &pair : registry) {
    std::cout << " - " << pair.first << std::endl;
  }
  std::cout << "----------------------------------" << std::endl;
}