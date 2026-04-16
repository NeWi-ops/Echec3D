#include "./../model/game.hpp"
#include "./../model/Board/board.hpp"
#include "./../model/Pieces/piece.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "RandomGenerator.hpp"
#include "StatsLogger.hpp"

LightningManager::LightningManager() {
    std::random_device rd;
    rng = std::mt19937(rd());
    resetTimer();
}

void LightningManager::resetTimer() {
    std::exponential_distribution<float> expDist(1.0f / 90.0f); 
    m_timeUntilNextStrike = std::max(10.0f, expDist(rng));        
}

void LightningManager::update(float deltaTime, Game& game) {
    // --- Part 1: Visual decay (flash fading) ---
    if (m_flashAlpha > 0.0f) {
        m_flashAlpha -= deltaTime * m_decayRate;
        if (m_flashAlpha < 0.0f) m_flashAlpha = 0.0f;
    }

    // --- Part 2: Real-time countdown ---
    m_timeUntilNextStrike -= deltaTime;

    if (m_timeUntilNextStrike > 0.0f) {
        return;
    }

    // --- Part 3: Strike! ---
    std::uniform_int_distribution<int> uniDist(0, 7);
    int ex = uniDist(rng);
    int ey = uniDist(rng);
    lastEpicenter = {ex, ey};
    m_hasStruckRecently = true;
    m_flashAlpha = static_cast<float>(RandomGenerator::generateUniformContinuous(0.7, 1.0));
    m_decayRate = static_cast<float>(RandomGenerator::generateUniformContinuous(1.0, 2.0));
    StatsLogger::instance().logLightningEvent(ex, ey, m_flashAlpha, m_decayRate);
    targetHighlights.clear();

    std::cout << "--- THE SKY DARKENS ---\n";
    std::cout << "LIGHTNING STRIKE at epicenter (" << ex << ", " << ey << ")!\n";

    bool modifiedBoard = false;
    Board& board = game.getBoard();

    bool isEpicenterImmune = true;
    std::normal_distribution<double> normDist(2.0, 0.5);

    struct PushTask {
        Coords from;
        int dx, dy;
    };
    std::vector<PushTask> tasks;

    // Phase 1: Identify all pieces that could be pushed
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (isEpicenterImmune && dx == 0 && dy == 0) {
                continue; // Eye of the storm
            }
            int px = ex + dx;
            int py = ey + dy;
            Coords pos{px, py};
            if (board.isInsideBoard(pos)) {
                targetHighlights.push_back(pos); // Highlight entire blast radius
                if (!board.isEmpty(pos)) {
                    tasks.push_back({pos, dx, dy});
                }
            }
        }
    }

    // Phase 2: Execute each push
    for (const auto& task : tasks) {
        Piece* p = board.getPiece(task.from);
        if (!p) continue; // Piece might have been captured by a previous push in this loop

        // Kings are immune to the blast wave and cannot be pushed
        if (p->getType() == PieceType::King) {
            continue;
        }

        int distance = static_cast<int>(std::round(normDist(rng)));
        distance = std::clamp(distance, 0, 4);
        StatsLogger::instance().logLightningPush(static_cast<float>(distance));
        if (distance == 0) continue;

        bool moveSuccess = true;
        Coords target = task.from;
        bool shouldCapture = false;
        std::vector<Coords> path;
        path.push_back(task.from);

        for (int step = 1; step <= distance; ++step) {
            target = {task.from.x + task.dx * step, task.from.y + task.dy * step};
            
            // Rule 1: Out of bounds -> Push fails
            if (!board.isInsideBoard(target)) {
                moveSuccess = false;
                break;
            }

            path.push_back(target);

            Piece* targetP = board.getPiece(target);
            if (targetP) {
                if (step < distance) {
                    // Obstacle mid-air
                    moveSuccess = false;
                    break;
                }
                
                // Rule 2: Ally collision -> Push fails
                if (targetP->getColor() == p->getColor()) {
                    moveSuccess = false;
                    break;
                }
                // Rule 3: Enemy King collision -> Push fails
                if (targetP->getType() == PieceType::King) {
                    moveSuccess = false;
                    break;
                }
                // Enemy collision -> Capture!
                shouldCapture = true;
            }
        }

        if (moveSuccess) {
            if (shouldCapture) {
                board.removePieceAt(target);
            }
            
            recentAnimations.push_back({p->getType(), p->getColor(), path});
            
            // Apply movement
            board.movePiece(task.from, target);
            targetHighlights.push_back(target);
            modifiedBoard = true;
        }
    }

    std::cout << "Lightning struck! History is wiped.\n";
    game.clearHistory();

    resetTimer();
}
