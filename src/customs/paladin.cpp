#include "paladin.hpp"
#include "../utils/RandomGenerator.hpp"
#include <cmath>


int Paladin::calculateBonusMoves(int currentTurnCount) const {
    // 1. On lance les dés (Loi Binomiale)
    int successes = RandomGenerator::generateBinomial(currentTurnCount, 1.0 / 12.0);
    
    // 2. On applique la règle de Game Design
    if (successes == 0) {
        return 0; // Pas de chance, aucun bonus
    }
    
    // Règle : 1-5 succès = +1 // 6-10 succès = +2 // etc.
    // La formule mathématique (successes - 1) / 5 fait exactement ça en division entière !
    int bonus = 1 + (successes - 1) / 5;

    if (bonus > 2) {
        bonus = 2;
    }

    return bonus;
}

std::vector<Coords> Paladin::getPseudoLegalMoves(const Board &board, Coords pos) const {
    int fullTurn = (board.getTurnCount() + 1) / 2;

    // Per-Turn Cache Logic
    if (lastTurnRolled != fullTurn) {
        currentTurnBonus = calculateBonusMoves(fullTurn);
        lastTurnRolled = fullTurn;
    }

    // Paralysis Rule (Silently block movement if paralyzed)
    if (currentTurnBonus == 0) {
        return std::vector<Coords>{};
    }

    std::vector<Coords> moves;

    // Omnidirectional moves up to currentTurnBonus distance
    for (int dx = -currentTurnBonus; dx <= currentTurnBonus; ++dx) {
        for (int dy = -currentTurnBonus; dy <= currentTurnBonus; ++dy) {
            if (dx == 0 && dy == 0) continue;

            Coords target = {pos.x + dx, pos.y + dy};

            // 1. Est-ce que la case cible est sur le plateau ?
            if (!board.isInsideBoard(target)) {
                continue;
            }

            // 2. Est-ce que la case est libre ou occupée par un ennemi ?
            const Piece *targetPiece = board.getPiece(target);
            if (targetPiece == nullptr || targetPiece->getColor() != this->getColor()) {
                moves.push_back(target);
            }
        }
    }

    // Innate Knight Moves
    const std::vector<Coords> knightOffsets = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},
        {2, -1},  {2, 1}
    };

    for (const auto &offset : knightOffsets) {
        // Prevent duplicate move insertion if radial loop already covered Knight targets
        if (std::abs(offset.x) <= currentTurnBonus && std::abs(offset.y) <= currentTurnBonus) {
            continue;
        }

        Coords target = {pos.x + offset.x, pos.y + offset.y};

        if (!board.isInsideBoard(target)) continue;

        const Piece *targetPiece = board.getPiece(target);
        if (targetPiece == nullptr || targetPiece->getColor() != this->getColor()) {
            moves.push_back(target);
        }
    }

    return moves;
}