#include "FenConverter.hpp"
#include <sstream>
#include <vector>

#include "./../Factory/factory.hpp"
#include "./../Pieces/bishop.hpp"
#include "./../Pieces/king.hpp"
#include "./../Pieces/knight.hpp"
#include "./../Pieces/pawn.hpp"
#include "./../Pieces/queen.hpp"
#include "./../Pieces/rook.hpp"
#include <algorithm>
#include <string>

std::string FenConverter::save(const Game &game) {
  std::stringstream ss;
  const Board &board = game.getBoard();

  for (int y = 0; y < 8; ++y) {
    int emptyCount = 0;
    for (int x = 0; x < 8; ++x) {
      const Piece *p = board.getPiece(x, y);
      if (!p) {
        emptyCount++;
      } else {
        if (emptyCount > 0) {
          ss << emptyCount;
          emptyCount = 0;
        }
        std::string c = "?";
        c = p->getFenSymbol();
        if (p->getColor() == PieceColor::White)
          std::transform(c.begin(), c.end(), c.begin(), ::toupper);
        ss << c;
      }
    }
    if (emptyCount > 0)
      ss << emptyCount;
    if (y < 7)
      ss << "/";
  }

  ss << " " << (game.getCurrentTurn() == PieceColor::White ? "w" : "b");
  ss << " - - 0 1";

  return ss.str();
}

void FenConverter::load(Game &game, const std::string &fen) {

  auto newBoard = std::make_unique<Board>();

  std::stringstream ss(fen);
  std::string segment;
  std::vector<std::string> parts;

  // decoupage du FEN
  while (std::getline(ss, segment, ' ')) {
    parts.push_back(segment);
  }

  if (parts.empty())
    return;

  int y = 0;
  int x = 0;

  std::string boardFen = parts[0];

  for (size_t i = 0; i < boardFen.length(); ++i) {
    char c = boardFen[i];

    // changement de ligne
    if (c == '/') {
      y++;
      x = 0;
      continue;
    }

    // cases vides
    if (isdigit(c)) {
      x += (c - '0');
      continue;
    }

    std::string key = "";
    PieceColor color = PieceColor::White;

    // EXTENDED FEN : detection de la parenthese
    // ex: (NomDeLaPiece:W)
    if (c == '(') {
      i++;

      std::string content = "";
      // on lit jusqu'à la parenthese fermante
      while (i < boardFen.length() && boardFen[i] != ')') {
        content += boardFen[i];
        i++;
      }

      size_t sepPos = content.find(':');
      if (sepPos != std::string::npos) {
        key = content.substr(0, sepPos); // ex: "Paladin"
        std::string colStr = content.substr(sepPos + 1);

        color = (colStr == "W" || colStr == "w") ? PieceColor::White
                                                 : PieceColor::Black;
      }
    } else {
      key = std::string(1, tolower(c));

      color = isupper(c) ? PieceColor::White : PieceColor::Black;
    }

    if (!key.empty()) {
      auto newPiece = PieceFactory::createPiece(key, color);

      if (newPiece) {
        newBoard->setPieceAt({x, y}, *newPiece);
      }

      x++;
    }
  }

  PieceColor turn = PieceColor::White;
  if (parts.size() > 1) {
    turn = (parts[1] == "w") ? PieceColor::White : PieceColor::Black;
  }

  game.resetState(std::move(newBoard), turn);
}