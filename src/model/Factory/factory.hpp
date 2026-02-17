#pragma once

#include "./../Pieces/piece.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class PieceFactory {
public:
  // Définition du type de fonction "Recette" pour créer une pièce
  // Elle prend une couleur et renvoie un pointeur unique vers une Pièce
  using CreatorFunc = std::function<std::unique_ptr<Piece>(PieceColor)>;

  // Empêcher l'instanciation (c'est une classe purement statique)
  PieceFactory() = delete;

  /**
   * Enregistre une nouvelle pièce dans l'usine.
   * @param key : Le nom/symbole de la pièce (ex: "r", "Paladin")
   * @param creator : La fonction lambda qui fait le 'new'
   */
  static void registerPiece(const std::string &key, CreatorFunc creator);

  /**
   * Fabrique une pièce à partir de son nom.
   * @param key : Le nom de la pièce (ex: "r", "Paladin")
   * @param color : La couleur voulue
   * @return unique_ptr vers la pièce, ou nullptr si inconnue.
   */
  static std::unique_ptr<Piece> createPiece(const std::string &key,
                                            PieceColor color);

  /**
   * (Optionnel) Pour le debug : liste toutes les pièces connues
   */
  static void printRegisteredPieces();

private:
  // Le registre est caché (Singleton de Meyer) pour être sûr qu'il existe
  // avant qu'on l'utilise.
  static std::map<std::string, CreatorFunc> &getRegistry();
};