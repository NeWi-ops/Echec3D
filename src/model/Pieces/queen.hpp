#pragma once

#include "./../Board/board.hpp"
#include "./../Types/MoveStruct.hpp"
#include "./piece.hpp"
#include <vector>

class Queen : public Piece {
public:
  explicit Queen(PieceColor color) : Piece(color, PieceType::Queen) {}

  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords coords) const override {
    std::vector<Coords> moves;

    int directions[8][2] = {{0, -1},  {0, 1},  {-1, 0}, {1, 0},
                            {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

    for (auto dir : directions) {
      int dx = dir[0];
      int dy = dir[1];

      Coords current = coords;

      while (true) {
        current.x += dx;
        current.y += dy;

        if (!board.isInsideBoard(current)) {
          break;
        }

        const Piece *targetPiece = board.getPiece(current);

        if (targetPiece == nullptr) {
          moves.push_back(current);

        }

        else {

          if (targetPiece->getColor() != this->getColor()) {
            moves.push_back(current);
          }

          break;
        }
      }
    }

    return moves;
  };

  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<Queen>(*this);
  }

  std::string getFenSymbol() const override { return "q"; }
};