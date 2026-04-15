#pragma once
#include <fstream>
#include <string>
#include <mutex>

class StatsLogger {
public:
    static StatsLogger& instance();
    
    void logLightningEvent(int x, int y, float flashAlpha, float flashDecay);
    void logLightningPush(float distance);
    void logCursedSquare(int type, int value);
    void logParticleExplosion(int count);
    void logParticleLifespan(float lifespan, float scale);
    void logPaladinAction(int successes);

private:
    StatsLogger();
    ~StatsLogger();
    
    // Static instance is managed by instance() method
    std::ofstream m_file;
    std::mutex m_mutex;
};
