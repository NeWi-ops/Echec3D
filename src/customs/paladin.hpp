#pragma once
#include <iostream>
#include "./../model/Board/board.hpp"
#include "./../model/Pieces/piece.hpp"
#include "./../model/Types/ColorEnum.hpp"
#include "./../model/Types/CoordsStruct.hpp"
#include "./../utils/scenarioRegister.hpp"
#include "./customsPiece.hpp"
#include <memory>
#include <vector>

class Paladin : public CustomPiece {
public:
  // Constructeur : On passe la couleur au parent (CustomPiece)
  explicit Paladin(PieceColor color) : CustomPiece(color) {}

  // Calcule les mouvements bonus du Paladin (Binomial Bonus Movement).
  // Lance 'currentTurnCount' dés virtuels, succès avec 1/6 de probabilité.
  int calculateBonusMoves(int currentTurnCount) const;

  // Accesseurs pour l'UI
  int getCurrentBonus() const { return currentTurnBonus; }
  int getLastTurnRolled() const { return lastTurnRolled; }

  // =========================================================
  // IMPLEMENTATION DU MOTEUR (Logique de Jeu)
  // =========================================================

  /**
   * Clone (Obligatoire pour le fonctionnement du Board)
   */
  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<Paladin>(*this);
  }

  static std::vector<GameScenario> getAddonScenarios() {
    return {
        {"Duel de Paladins", "Les reines sont remplacees par des Paladins.",
         "rnb(Paladin:B)kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB(Paladin:W)KBNR w "
         "KQkq - 0 1"},
        {"Guerre Totale", "Tous les pions sont des Paladins !",
         "rnbqkbnr/"
         "(Paladin:B)(Paladin:B)(Paladin:B)(Paladin:B)(Paladin:B)(Paladin:B)("
         "Paladin:B)(Paladin:B)/8/8/8/8/"
         "(Paladin:W)(Paladin:W)(Paladin:W)(Paladin:W)(Paladin:W)(Paladin:W)("
         "Paladin:W)(Paladin:W)/RNBQKBNR w KQkq - 0 1"},
        {"Test Boss", "Un Paladin blanc seul contre des pions.",
         "8/8/3p1p2/2p3p1/3(Paladin:W)3/2p3p1/3p1p2/4K2k w - - 0 1"}};
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
                                          Coords pos) const override;

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

private:
  mutable int lastTurnRolled = -1;
  mutable int currentTurnBonus = 0;
};