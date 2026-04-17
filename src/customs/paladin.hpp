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
  explicit Paladin(PieceColor color) : CustomPiece(color) {}

  int calculateBonusMoves(int currentTurnCount) const;

  int getCurrentBonus() const { return currentTurnBonus; }
  int getLastTurnRolled() const { return lastTurnRolled; }



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

  std::string getFenSymbol() const override { return "Paladin"; }
  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords pos) const override;



  std::string getModelId() const override { return "paladin"; }
  std::string getTextureId() const override { return "paladin_gold"; }

  float getScale() const override { return 1.2f; }

  std::string getIcon() const override { return "P"; }

private:
  mutable int lastTurnRolled = -1;
  mutable int currentTurnBonus = 0;
};