#pragma once

#include "./../types/PieceStruct.hpp"
#include <array>
#include <iostream>

class Game {
private:
  std::array<std::array<Piece, 8>, 8> board;

public:
  Game();
  ~Game();
};
