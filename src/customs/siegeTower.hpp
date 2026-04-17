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

  explicit SiegeTower(PieceColor color) : CustomPiece(color) {}

  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<SiegeTower>(*this);
  }
  std::string getFenSymbol() const override { return "SiegeTower"; }
  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords pos) const override {
    std::vector<Coords> moves;

    const std::vector<Coords> directions = {
        {0, 1},  
        {0, -1}, 
        {1, 0},  
        {-1, 0}  
    };

    for (const auto &dir : directions) {
      for (int dist = 1; dist <= 4; ++dist) {

        Coords target = {pos.x + (dir.x * dist), pos.y + (dir.y * dist)};

        if (!board.isInsideBoard(target)) {
          break;
        }

        const Piece *targetPiece = board.getPiece(target);

        if (targetPiece == nullptr ||
            targetPiece->getColor() != this->getColor()) {
          moves.push_back(target);
        }
      }
    }

    return moves;
  }

  std::string getModelId() const override {
    return "siege_tower"; 
  }

  std::string getTextureId() const override {
    return "wood_planks"; 
  }

  float getScale() const override {
    return 1.3f; 
  }

  std::string getIcon() const override {
    return "T"; 
  }

  static std::vector<GameScenario> getAddonScenarios() {
    return {{"Siège du Château",
             "Les Tours de Siège (S) peuvent sauter les murs de pions !",
             "rn(SiegeTower:B)qk(SiegeTower:B)nr/pppppppp/8/8/8/8/PPPPPPPP/"
             "RN(SiegeTower:W)QK(SiegeTower:W)NR w KQkq - 0 1"}};
  }
};