#pragma once

#include "./../Board/board.hpp"
#include "./../Types/MoveStruct.hpp"
#include "./piece.hpp"
#include <vector>

class Pawn : public Piece {
public:
  explicit Pawn(PieceColor color) : Piece(color, PieceType::Pawn) {}

  std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                          Coords coords) const override {
    std::vector<Coords> moves;

    int direction = (getColor() == PieceColor::White) ? -1 : 1;
    int startRow = (getColor() == PieceColor::White) ? 6 : 1;

    Coords forward = {coords.x, coords.y + direction};

    if (board.isInsideBoard(forward) && board.getPiece(forward) == nullptr) {
      moves.push_back(forward);

      if (coords.y == startRow) {
        Coords doubleForward = {coords.x, coords.y + 2 * direction};
        if (board.isInsideBoard(doubleForward) &&
            board.getPiece(doubleForward) == nullptr) {
          moves.push_back(doubleForward);
        }
      }
    }

    int dxList[] = {-1, 1};

    for (int dx : dxList) {
      Coords target = {coords.x + dx, coords.y + direction};

      if (board.isInsideBoard(target)) {

        const Piece *targetPiece = board.getPiece(target);

        if (targetPiece != nullptr &&
            targetPiece->getColor() != this->getColor()) {
          moves.push_back(target);
        }
      }
    }

    return moves;
  };

  std::unique_ptr<Piece> clone() const override {
    return std::make_unique<Pawn>(*this);
  }
};