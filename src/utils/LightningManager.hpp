#pragma once

#include <random>
#include "../model/Types/CoordsStruct.hpp"
#include "../model/Types/PieceStruct.hpp"
#include <vector>

struct StrikeAnimation {
    PieceType type;
    PieceColor color;
    std::vector<Coords> path;
};

class Game; // Forward declaration


class LightningManager {
private:
    int turnsUntilNextStrike;
    std::mt19937 rng;
    Coords lastEpicenter{-1, -1};
    bool m_hasStruckRecently = false;
    std::vector<Coords> targetHighlights;
    float m_flashAlpha = 0.0f;
    float m_decayRate = 1.5f;
    std::vector<StrikeAnimation> recentAnimations;

    void resetTimer();

public:
    LightningManager();
    void update(Game& game);

    bool hasStruckRecently() const { return m_hasStruckRecently; }
    Coords getLastEpicenter() const { return lastEpicenter; }
    const std::vector<Coords>& getTargetHighlights() const { return targetHighlights; }
    float getFlashAlpha() const { return m_flashAlpha; }
    void updateVisuals(float deltaTime);

    std::vector<StrikeAnimation> popAnimations() {
        std::vector<StrikeAnimation> copy = recentAnimations;
        recentAnimations.clear();
        return copy;
    }

    void dismissStrike() { 
        m_hasStruckRecently = false; 
        targetHighlights.clear();
    }
};
