#pragma once

#include "CoordsStruct.hpp"
#include "PieceEnum.hpp"
#include <optional>

struct Move {
  Coords from;
  Coords to;
  PieceType movingPiece;

  std::optional<PieceType> capturedPiece = std::nullopt;
  bool isPromotion = false;
  std::optional<PieceType> promoteTo = std::nullopt;
};