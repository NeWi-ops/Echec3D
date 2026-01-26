#include "./game.hpp"
#include <array>
#include <iostream>

#include "./../types/PieceStruct.hpp"

Game::Game() {
  this->board = std::array<std::array<Piece, 8>, 8>{};
  // Initialize the chess board with pieces in starting positions
}