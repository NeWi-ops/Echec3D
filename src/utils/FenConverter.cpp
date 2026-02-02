#include "FenConverter.hpp"
#include <sstream>
#include <vector>

#include "./../model/Pieces/bishop.hpp"
#include "./../model/Pieces/king.hpp"
#include "./../model/Pieces/knight.hpp"
#include "./../model/Pieces/pawn.hpp"
#include "./../model/Pieces/queen.hpp"
#include "./../model/Pieces/rook.hpp"

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
        char c = '?';
        switch (p->getType()) {
        case PieceType::Pawn:
          c = 'p';
          break;
        case PieceType::Rook:
          c = 'r';
          break;
        case PieceType::Knight:
          c = 'n';
          break;
        case PieceType::Bishop:
          c = 'b';
          break;
        case PieceType::Queen:
          c = 'q';
          break;
        case PieceType::King:
          c = 'k';
          break;
        }
        if (p->getColor() == PieceColor::White)
          c = toupper(c);
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

  while (std::getline(ss, segment, ' ')) {
    parts.push_back(segment);
  }

  if (parts.empty())
    return;

  int y = 0;
  int x = 0;
  for (char c : parts[0]) {
    if (c == '/') {
      y++;
      x = 0;
    } else if (isdigit(c)) {
      x += (c - '0');
    } else {
      PieceColor color = isupper(c) ? PieceColor::White : PieceColor::Black;
      char typeChar = tolower(c);
      std::unique_ptr<Piece> newPiece;

      switch (typeChar) {
      case 'p':
        newPiece = std::make_unique<Pawn>(color);
        break;
      case 'r':
        newPiece = std::make_unique<Rook>(color);
        break;
      case 'n':
        newPiece = std::make_unique<Knight>(color);
        break;
      case 'b':
        newPiece = std::make_unique<Bishop>(color);
        break;
      case 'q':
        newPiece = std::make_unique<Queen>(color);
        break;
      case 'k':
        newPiece = std::make_unique<King>(color);
        break;
      }
      if (newPiece)
        newBoard->setPieceAt({x, y}, *newPiece);
      x++;
    }
  }

  PieceColor turn = PieceColor::White;
  if (parts.size() > 1) {
    turn = (parts[1] == "w") ? PieceColor::White : PieceColor::Black;
  }

  game.resetState(std::move(newBoard), turn);
}