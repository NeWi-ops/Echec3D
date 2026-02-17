#pragma once

#include "./../model/Board/board.hpp"
#include "./../model/Pieces/piece.hpp"
#include "./../model/Types/ColorEnum.hpp"
#include "./../model/Types/CoordsStruct.hpp"
#include "./../utils/scenarioRegister.hpp"
#include "./customsPiece.hpp"
#include <memory>
#include <vector>

class SiegeTower : public CustomPiece {
public:
  // Constructeur
  explicit SiegeTower(PieceColor color) : CustomPiece(color) {}

  // 1. CLONE (Pour le prototype du Board)
  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<SiegeTower>(*this);
  }

  // 2. NOM FEN (Sauvegarde) -> donnera (SiegeTower:W)
  std::string getFenSymbol() const override { return "SiegeTower"; }

  // 3. LOGIQUE DE JEU
  // Déplacement en ligne droite (Orthogonal), Max 4 cases, SAUTE par dessus les
  // obstacles.
  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords pos) const override {
    std::vector<Coords> moves;

    // Les 4 directions orthogonales (comme une Tour)
    const std::vector<Coords> directions = {
        {0, 1},  // Haut
        {0, -1}, // Bas
        {1, 0},  // Droite
        {-1, 0}  // Gauche
    };

    // Pour chaque direction
    for (const auto &dir : directions) {

      // On avance de 1 à 4 cases maximum
      for (int dist = 1; dist <= 4; ++dist) {

        Coords target = {pos.x + (dir.x * dist), pos.y + (dir.y * dist)};

        // Si on sort du plateau, on arrête cette direction
        if (!board.isInsideBoard(target)) {
          break;
        }

        const Piece *targetPiece = board.getPiece(target);

        // LOGIQUE DE "SAUT" (Jumping) :
        // Contrairement à une Tour classique, on ne fait pas de "break"
        // si on rencontre une pièce. On continue la boucle pour voir les cases
        // derrière.

        // Si la case cible est vide OU contient un ennemi, c'est valide.
        if (targetPiece == nullptr ||
            targetPiece->getColor() != this->getColor()) {
          moves.push_back(target);
        }

        // Si c'est un allié, on ne peut pas atterrir dessus,
        // MAIS on continue la boucle car on peut sauter par-dessus !
      }
    }

    return moves;
  }

  // 4. VISUEL (Configuration pour la Vue 3D)

  std::string getModelId() const override {
    return "siege_tower"; // Assure-toi d'avoir un siege_tower.obj ou utilise
                          // "rook"
  }

  std::string getTextureId() const override {
    return "wood_planks"; // Une texture bois serait idéale
  }

  float getScale() const override {
    return 1.3f; // C'est une grosse machine de guerre
  }

  // Icône pour l'interface 2D (ImGui)
  std::string getIcon() const override {
    return "T"; // "T" pour Tour de siège
  }

  // 5. MANIFESTE (Scénarios de démo)
  static std::vector<GameScenario> getAddonScenarios() {
    return {{"Siège du Château",
             "Les Tours de Siège (S) peuvent sauter les murs de pions !",
             // Une ligne de pions bloquant le passage, mais la Tour de Siège
             // saute par dessus
             "rn(SiegeTower:B)qk(SiegeTower:B)nr/pppppppp/8/8/8/8/PPPPPPPP/"
             "RN(SiegeTower:W)QK(SiegeTower:W)NR w KQkq - 0 1"}};
  }
};