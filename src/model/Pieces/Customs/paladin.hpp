#pragma once

#include "./../../Board/board.hpp" // Nécessaire pour isInsideBoard
#include "./../piece.hpp"
#include "./../piecefactory.hpp"
#include <string>
#include <vector>

class Paladin : public Piece {
public:
  // Constructeur
  Paladin(PieceColor color) : Piece(color, PieceType::Custom) {}

  // 1. Nom
  std::string getName() const override { return "Paladin"; }

  // 2. Caractère FEN ('A' pour Archonte/Paladin)
  char getFenChar() const override { return 'A'; }

  // 3. CLONE (OBLIGATOIRE sinon la classe reste abstraite !)
  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<Paladin>(*this);
  }

  // 4. Mouvements (Roi + Cavalier)
  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords pos) const override {
    std::vector<Coords> moves;

    // Définition de tous les décalages possibles
    // 8 mouvements de Roi + 8 mouvements de Cavalier
    std::vector<std::pair<int, int>> offsets = {
        // --- Mouvements de ROI (1 case autour) ---
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {0, -1},
        {0, 1},
        {1, -1},
        {1, 0},
        {1, 1},

        // --- Mouvements de CAVALIER (Forme en L) ---
        {-2, -1},
        {-2, 1},
        {-1, -2},
        {-1, 2},
        {1, -2},
        {1, 2},
        {2, -1},
        {2, 1}};

    for (const auto &offset : offsets) {
      Coords target;
      target.x = pos.x + offset.first;
      target.y = pos.y + offset.second;

      // 1. Vérifier si on est toujours sur le plateau
      if (!board.isInsideBoard(target)) {
        continue;
      }

      // 2. Vérifier ce qu'il y a sur la case cible
      const Piece *targetPiece = board.getPiece(target);

      if (targetPiece == nullptr) {
        // Case vide : OK
        moves.push_back(target);
      } else if (targetPiece->getColor() != this->getColor()) {
        // Pièce ennemie : OK (Capture)
        moves.push_back(target);
      }
      // Si c'est une pièce alliée, on ne fait rien (bloqué)
    }

    return moves;
  }
};
