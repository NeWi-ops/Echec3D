#pragma once

#include "./../../../types/MoveStruct.hpp"
#include "./../../Board/board.hpp"
#include "./../piece.hpp"
#include "./../piecefactory.hpp"
#include <vector>
class Rook : public Piece {
public:
  explicit Rook(PieceColor color) : Piece(color, PieceType::Rook) {}

  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords coords) const override {
    std::vector<Coords> moves;

    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

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
    return std::make_unique<Rook>(*this);
  }

  char getFenChar() const override { return 'r'; }

  std::string getName() const override { return "Rook"; }
};

REGISTER_PIECE(Rook, 'r')