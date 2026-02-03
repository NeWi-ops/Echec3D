#pragma once

#include "./piece.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#define REGISTER_PIECE(CLASS_NAME, FEN_CHAR)                                   \
  namespace {                                                                  \
  /* volatile empêche l'optimisation */                                        \
  static volatile bool registered_##CLASS_NAME = []() {                        \
    PieceFactory::registerPiece(FEN_CHAR, [](PieceColor c) {                   \
      return std::make_unique<CLASS_NAME>(c);                                  \
    });                                                                        \
    return true;                                                               \
  }();                                                                         \
  }

class PieceFactory {
public:
  using CreatorFunc = std::function<std::unique_ptr<Piece>(PieceColor)>;

  // Singleton du registre
  static std::map<char, CreatorFunc> &getRegistry() {
    static std::map<char, CreatorFunc> registry;
    return registry;
  }

  static void registerPiece(char fenChar, CreatorFunc creator) {
    std::cout << "[FACTORY] Enregistrement de la piece : " << fenChar
              << std::endl;
    getRegistry()[toupper(fenChar)] = creator;
  }

  static std::unique_ptr<Piece> createPiece(char fenChar, PieceColor color) {
    auto &registry = getRegistry();
    char key = toupper(fenChar);
    if (registry.find(key) != registry.end()) {
      return registry[key](color);
    }
    return nullptr;
  }
};