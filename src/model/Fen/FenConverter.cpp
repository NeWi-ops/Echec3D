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

  // Découpage du FEN (Plateau, Tour, Roque, etc.)
  while (std::getline(ss, segment, ' ')) {
    parts.push_back(segment);
  }

  if (parts.empty())
    return;

  int y = 0;
  int x = 0;

  // On récupère la string du plateau pour pouvoir la parcourir avec un index
  // 'i' (Nécessaire pour lire plusieurs caractères d'un coup pour le Custom)
  std::string boardFen = parts[0];

  for (size_t i = 0; i < boardFen.length(); ++i) {
    char c = boardFen[i];

    // 1. Changement de ligne
    if (c == '/') {
      y++;
      x = 0;
      continue;
    }

    // 2. Cases vides (Chiffres)
    if (isdigit(c)) {
      x += (c - '0');
      continue;
    }

    // Préparation des variables pour la Factory
    std::string key = "";
    PieceColor color = PieceColor::White;

    // 3. Cas EXTENDED FEN : Détection de la parenthèse '('
    // Format attendu : (NomDeLaPiece:W) ou (NomDeLaPiece:B)
    if (c == '(') {
      i++; // On avance après la parenthèse ouvrante

      std::string content = "";
      // On lit jusqu'à la parenthèse fermante
      while (i < boardFen.length() && boardFen[i] != ')') {
        content += boardFen[i];
        i++;
      }

      // On sépare le Nom de la Couleur (séparateur ':')
      size_t sepPos = content.find(':');
      if (sepPos != std::string::npos) {
        key = content.substr(0, sepPos); // ex: "Paladin"
        std::string colStr = content.substr(sepPos + 1);

        // On détermine la couleur (W/w = Blanc, le reste = Noir)
        color = (colStr == "W" || colStr == "w") ? PieceColor::White
                                                 : PieceColor::Black;
      }
    }
    // 4. Cas STANDARD FEN (r, n, b, q, k, p)
    else {
      // La clé est la lettre en minuscule ("r", "k"...)
      key = std::string(1, tolower(c));

      // La couleur est déterminée par la casse (Majuscule = Blanc)
      color = isupper(c) ? PieceColor::White : PieceColor::Black;
    }

    // 5. Création via la Factory (Remplaçant le Switch)
    if (!key.empty()) {
      auto newPiece = PieceFactory::createPiece(key, color);

      if (newPiece) {
        newBoard->setPieceAt({x, y}, *newPiece);
      } else {
        // Optionnel : Log si une pièce du FEN n'est pas connue dans le
        // GameRegistry std::cerr << "Erreur FEN : Piece '" << key << "' non
        // enregistree." << std::endl;
      }

      x++; // On avance sur le plateau
    }
  }

  // Gestion du tour (inchangée)
  PieceColor turn = PieceColor::White;
  if (parts.size() > 1) {
    turn = (parts[1] == "w") ? PieceColor::White : PieceColor::Black;
  }

  game.resetState(std::move(newBoard), turn);
}