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
    float m_timeUntilNextStrike = 0.0f;
    std::mt19937 rng;
    Coords lastEpicenter{-1, -1};
    bool m_hasStruckRecently = false;
    std::vector<Coords> targetHighlights;
    float m_flashAlpha = 0.0f;
    float m_decayRate = 1.5f;
    std::vector<StrikeAnimation> recentAnimations;


public:
    LightningManager();
    void update(float deltaTime, Game& game);
    void resetTimer();

    bool hasStruckRecently() const { return m_hasStruckRecently; }
    Coords getLastEpicenter() const { return lastEpicenter; }
    const std::vector<Coords>& getTargetHighlights() const { return targetHighlights; }
    float getFlashAlpha() const { return m_flashAlpha; }


    std::vector<StrikeAnimation> popAnimations() {
        std::vector<StrikeAnimation> copy = recentAnimations;
        recentAnimations.clear();
        return copy;
    }

    void dismissStrike() { 
        m_hasStruckRecently = false; 
        targetHighlights.clear();
        m_flashAlpha = 0.0f;
        recentAnimations.clear();
    }
};
