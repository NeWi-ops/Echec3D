#pragma once

#include "./../Pieces/piece.hpp"
#include <array>
#include <iostream>
#include <memory>

class Board {
private:
  std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;

public:
  Board();
  ~Board();
  Board(Board const &other);

  const Piece &getPieceAt(int x, int y) const;
  void setPieceAt(int x, int y, const Piece &piece);

  void setupStandardBoard();

  void clear();

  [[nodiscard]] Piece *getPiece(Coords pos) const;

  [[nodiscard]] Piece *getPiece(int x, int y) const;

  [[nodiscard]] bool isEmpty(Coords pos) const;

  [[nodiscard]] bool isInsideBoard(Coords pos) const;

  void movePiece(Coords from, Coords to);

  void setPieceAt(Coords pos, const Piece &piece);

  void removePieceAt(Coords pos);

  Board clone() const;
};
