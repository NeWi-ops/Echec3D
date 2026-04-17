#pragma once

#include "./../Pieces/piece.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class PieceFactory {
public:
  // Définition du type de fonction pour créer une pièce
  // Elle prend une couleur et renvoie un pointeur unique vers une Pièce
  using CreatorFunc = std::function<std::unique_ptr<Piece>(PieceColor)>;

  // Empêcher l'instanciation (c'est une classe purement statique)
  PieceFactory() = delete;

  // Enregistre une nouvelle pièce dans l'usine.
  static void registerPiece(const std::string &key, CreatorFunc creator);

  // Fabrique une pièce à partir de son nom.
  static std::unique_ptr<Piece> createPiece(const std::string &key,
                                            PieceColor color);

  // POUR DEBUG
  static void printRegisteredPieces();

private:
  static std::map<std::string, CreatorFunc> &getRegistry();
};