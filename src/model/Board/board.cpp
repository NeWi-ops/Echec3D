#include "board.hpp"
#include "./../Pieces/bishop.hpp"
#include "./../Pieces/king.hpp"
#include "./../Pieces/knight.hpp"
#include "./../Pieces/pawn.hpp"
#include "./../Pieces/queen.hpp"
#include "./../Pieces/rook.hpp"
#include <memory>

Board::Board() {

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {

      this->board[x][y] = nullptr;
    }
  }
}

Board::~Board() = default;

Board::Board(const Board &other) {
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {

      if (other.board[x][y]) {

        this->board[x][y] = other.board[x][y]->clone();
      } else {
        this->board[x][y] = nullptr;
      }
    }
  }
}

const Piece &Board::getPieceAt(int x, int y) const { return *(board[y][x]); }

void Board::setPieceAt(int x, int y, const Piece &piece) {
  board[y][x] = piece.clone();
}

void Board::clear() {
  for (auto &row : this->board) {
    for (auto &square : row) {
      square.reset();
    }
  }
}

void Board::setupStandardBoard() {

  this->clear();

  for (int x = 0; x < 8; ++x) {
    this->board[1][x] = std::make_unique<Pawn>(PieceColor::Black);
    this->board[6][x] = std::make_unique<Pawn>(PieceColor::White);
  }

  auto setupMajorRow = [&](int y, PieceColor color) {
    this->board[y][0] = std::make_unique<Rook>(color);
    this->board[y][1] = std::make_unique<Knight>(color);
    this->board[y][2] = std::make_unique<Bishop>(color);

    this->board[y][3] = std::make_unique<Queen>(color);
    this->board[y][4] = std::make_unique<King>(color);

    this->board[y][5] = std::make_unique<Bishop>(color);
    this->board[y][6] = std::make_unique<Knight>(color);
    this->board[y][7] = std::make_unique<Rook>(color);
  };

  setupMajorRow(0, PieceColor::Black);
  setupMajorRow(7, PieceColor::White);
}

void Board::movePiece(Coords from, Coords to) {

  board[to.y][to.x].reset();
  board[to.y][to.x] = std::move(board[from.y][from.x]);
}

void Board::setPieceAt(Coords pos, const Piece &piece) {
  board[pos.y][pos.x] = piece.clone();
}

void Board::removePieceAt(Coords pos) { board[pos.y][pos.x].reset(); }

[[nodiscard]] Piece *Board::getPiece(Coords pos) const {
  return board[pos.y][pos.x].get();
}

[[nodiscard]] Piece *Board::getPiece(int x, int y) const {
  return board[y][x].get();
}

[[nodiscard]] bool Board::isEmpty(Coords pos) const {
  return board[pos.y][pos.x] == nullptr;
}

[[nodiscard]] bool Board::isInsideBoard(Coords pos) const {
  return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8;
}

Board Board::clone() const {
  Board newBoard;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      if (this->board[y][x]) {
        newBoard.board[y][x] = this->board[y][x]->clone();
      } else {
        newBoard.board[y][x] = nullptr;
      }
    }
  }

  return newBoard;
}