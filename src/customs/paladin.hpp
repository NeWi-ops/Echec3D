#pragma once

#include "./../model/Board/board.hpp"
#include "./../model/Pieces/piece.hpp"
#include "./../model/Types/ColorEnum.hpp"
#include "./../model/Types/CoordsStruct.hpp"
#include "./customsPiece.hpp"
#include <memory>
#include <vector>

class Paladin : public CustomPiece {
public:
  // Constructeur : On passe la couleur au parent (CustomPiece)
  explicit Paladin(PieceColor color) : CustomPiece(color) {}

  // =========================================================
  // IMPLEMENTATION DU MOTEUR (Logique de Jeu)
  // =========================================================

  /**
   * Clone (Obligatoire pour le fonctionnement du Board)
   */
  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<Paladin>(*this);
  }

  /**
   * Nom pour le Extended FEN.
   * Donne : (Paladin:W) ou (Paladin:B)
   */
  std::string getFenSymbol() const override { return "Paladin"; }

  /**
   * Mouvements : Le Paladin est un "Centaure" (Roi + Cavalier).
   * Il se déplace d'1 case dans toutes les directions OU saute en L.
   */
  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords pos) const override {
    std::vector<Coords> moves;

    // Liste de tous les décalages possibles (8 Roi + 8 Cavalier)
    const std::vector<Coords> offsets = {// --- ROI (1 case autour) ---
                                         {-1, -1},
                                         {-1, 0},
                                         {-1, 1},
                                         {0, -1},
                                         {0, 1},
                                         {1, -1},
                                         {1, 0},
                                         {1, 1},

                                         // --- CAVALIER (Sauts en L) ---
                                         {-2, -1},
                                         {-2, 1},
                                         {-1, -2},
                                         {-1, 2},
                                         {1, -2},
                                         {1, 2},
                                         {2, -1},
                                         {2, 1}};

    for (const auto &offset : offsets) {
      Coords target = {pos.x + offset.x, pos.y + offset.y};

      // 1. Est-ce que la case cible est sur le plateau ?
      if (!board.isInsideBoard(target)) {
        continue;
      }

      // 2. Est-ce que la case est libre ou occupée par un ennemi ?
      const Piece *targetPiece = board.getPiece(target);

      if (targetPiece == nullptr ||
          targetPiece->getColor() != this->getColor()) {
        moves.push_back(target);
      }
      // Si c'est un allié, on est bloqué (on ne l'ajoute pas)
    }

    return moves;
  }

  // =========================================================
  // IMPLEMENTATION VISUELLE (Le Contrat CustomPiece)
  // =========================================================

  /**
   * Identifiant pour l'AssetManager.
   * Il faudra charger un fichier obj associé à la clé "paladin".
   */
  std::string getModelId() const override { return "paladin"; }

  /**
   * Identifiant de texture.
   * Il faudra charger une texture associée à la clé "paladin_gold".
   */
  std::string getTextureId() const override { return "paladin_gold"; }

  /**
   * Le Paladin est une unité imposante, on le grossit de 20%.
   */
  float getScale() const override { return 1.2f; }

  // On utilise le getColorModifier par défaut de CustomPiece
  // (Blanc pur pour les blancs, Gris foncé pour les noirs)
  // Mais on pourrait le surcharger ici si on voulait un Paladin toujours doré.

  std::string getIcon() const override { return "P"; }
};