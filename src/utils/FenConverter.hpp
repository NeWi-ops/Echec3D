#pragma once
#include "./../model/game.hpp"
#include <string>

class FenConverter {
public:
  static std::string save(const Game &game);

  static void load(Game &game, const std::string &fen);
};