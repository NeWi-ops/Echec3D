#include "FenConverter.hpp"
#include <cctype> // Nécessaire pour toupper, tolower, isdigit
#include <sstream>
#include <vector>

// On retire tous les includes des pièces spécifiques !
// On ajoute juste le Chef d'Orchestre (la Factory)
#include "./../model/Pieces/piecefactory.hpp"

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

        // --- NOUVEAU CODE (Dynamique) ---
        // On récupère le caractère de base (ex: 'p', 'A', 'k')
        char c = p->getFenChar();

        // On gère la casse selon la couleur (Majuscule = Blanc, Minuscule =
        // Noir)
        if (p->getColor() == PieceColor::White)
          c = toupper(c);
        else
          c = tolower(c);

        ss << c;
        // --------------------------------
      }
    }
    if (emptyCount > 0)
      ss << emptyCount;
    if (y < 7)
      ss << "/";
  }

  ss << " " << (game.getCurrentTurn() == PieceColor::White ? "w" : "b");
  ss << " - - 0 1"; // Droits de roque et en-passant par défaut pour l'instant

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

  // Lecture de la partie Plateau du FEN
  for (char c : parts[0]) {
    if (c == '/') {
      y++;
      x = 0;
    } else if (isdigit(c)) {
      x += (c - '0');
    } else {
      // C'est une pièce !
      PieceColor color = isupper(c) ? PieceColor::White : PieceColor::Black;

      // --- NOUVEAU CODE (Factory) ---
      // On demande à l'usine de créer la pièce correspondant à la lettre 'c'
      // (La Factory gère elle-même le mapping 'A' -> Paladin, 'r' -> Rook...)
      std::unique_ptr<Piece> newPiece = PieceFactory::createPiece(c, color);

      if (newPiece) {
        // Si l'usine a réussi à créer la pièce, on la place
        // Note: J'utilise setPieceAt selon ton code précédent
        newBoard->setPieceAt({x, y}, *newPiece);
      } else {
        // Optionnel : Log d'erreur si une lettre inconnue est trouvée
        // std::cerr << "Piece inconnue dans le FEN : " << c << std::endl;
      }
      // ------------------------------

      x++;
    }
  }

  PieceColor turn = PieceColor::White;
  if (parts.size() > 1) {
    turn = (parts[1] == "w") ? PieceColor::White : PieceColor::Black;
  }

  game.resetState(std::move(newBoard), turn);
}