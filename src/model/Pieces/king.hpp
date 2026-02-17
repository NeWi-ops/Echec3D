#pragma once

#include "./../Board/board.hpp"
#include "./../Types/MoveStruct.hpp"
#include "./piece.hpp"
#include <vector>

class King : public Piece {
public:
  explicit King(PieceColor color) : Piece(color, PieceType::King) {}

  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords coords) const override {
    std::vector<Coords> moves;

    int offsets[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                         {1, 0},   {-1, 1}, {0, 1},  {1, 1}};

    for (auto offset : offsets) {
      Coords target = {coords.x + offset[0], coords.y + offset[1]};

      if (board.isInsideBoard(target)) {

        const Piece *targetPiece = board.getPiece(target);

        if (targetPiece == nullptr ||
            targetPiece->getColor() != this->getColor()) {
          moves.push_back(target);
        }
      }
    }

    return moves;
  };

  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<King>(*this);
  }

  std::string getFenSymbol() const override { return "k"; }
};