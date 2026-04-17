#include "paladin.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../utils/StatsLogger.hpp"
#include <cmath>


int Paladin::calculateBonusMoves(int currentTurnCount) const {
    int successes = RandomGenerator::generateBinomial(currentTurnCount, 1.0 / 12.0);
    StatsLogger::instance().logPaladinAction(successes);
    
    if (successes == 0) {
        return 0; 
    }
    
    int bonus = 1 + (successes - 1) / 5;

    if (bonus > 2) {
        bonus = 2;
    }

    return bonus;
}

std::vector<Coords> Paladin::getPseudoLegalMoves(const Board &board, Coords pos) const {
    int fullTurn = (board.getTurnCount() + 1) / 2;

    if (lastTurnRolled != fullTurn) {
        currentTurnBonus = calculateBonusMoves(fullTurn);
        lastTurnRolled = fullTurn;
    }

    if (currentTurnBonus == 0) {
        return std::vector<Coords>{};
    }

    std::vector<Coords> moves;

    for (int dx = -currentTurnBonus; dx <= currentTurnBonus; ++dx) {
        for (int dy = -currentTurnBonus; dy <= currentTurnBonus; ++dy) {
            if (dx == 0 && dy == 0) continue;

            Coords target = {pos.x + dx, pos.y + dy};

            if (!board.isInsideBoard(target)) {
                continue;
            }
            const Piece *targetPiece = board.getPiece(target);
            if (targetPiece == nullptr || targetPiece->getColor() != this->getColor()) {
                moves.push_back(target);
            }
        }
    }

    const std::vector<Coords> knightOffsets = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},
        {2, -1},  {2, 1}
    };

    for (const auto &offset : knightOffsets) {
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