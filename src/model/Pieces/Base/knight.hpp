#pragma once

#include "./../../../types/MoveStruct.hpp"
#include "./../../Board/board.hpp"
#include "./../piece.hpp"
#include "./../piecefactory.hpp"
#include <vector>
class Knight : public Piece {
public:
  explicit Knight(PieceColor color) : Piece(color, PieceType::Knight) {}

  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords coords) const override {
    std::vector<Coords> moves;

    int jumps[8][2] = {{-1, -2}, {1, -2}, {-1, 2}, {1, 2},
                       {-2, -1}, {-2, 1}, {2, -1}, {2, 1}};

    for (auto jump : jumps) {
      Coords target = {coords.x + jump[0], coords.y + jump[1]};

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
    return std::make_unique<Knight>(*this);
  }

  char getFenChar() const override { return 'n'; }

  std::string getName() const override { return "Knight"; }
};

REGISTER_PIECE(Knight, 'n')