#pragma once

#include "./../Pieces/piece.hpp"
#include <array>
#include <iostream>
#include <memory>

class Arbiter {
private:
public:
  Arbiter();
  ~Arbiter();

  static std::vector<Move> getLegalMoves(const Board &originalBoard,
                                         Coords origin);

  static bool isKingInCheck(const Board &board, PieceColor kingColor);

  static bool hasLegalMoves(const Board &board, PieceColor color);

  static bool isInsufficientMaterial(const Board &board);
};
