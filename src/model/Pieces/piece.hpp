#pragma once

#include "./../Types/ColorEnum.hpp"
#include "./../Types/PieceEnum.hpp"

#include "./../Types/MoveStruct.hpp"
#include <memory>
#include <vector>

class Board;

class Piece {
public:
  virtual ~Piece() = default;

  virtual std::unique_ptr<Piece> clone() const = 0;
  virtual std::vector<Coords> getPseudoLegalMoves(const Board &board,
                                                  Coords coords) const = 0;

  void setColor(PieceColor color) { this->m_color = color; }
  [[nodiscard]] PieceColor getColor() const { return this->m_color; }
  [[nodiscard]] PieceType getType() const { return m_type; }

  static PieceColor opposite(PieceColor color) {
    return (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
  }

  virtual std::string getFenSymbol() const = 0;

protected:
  Piece(PieceColor color, PieceType type) : m_color(color), m_type(type) {}

  PieceColor m_color;
  PieceType m_type;
};