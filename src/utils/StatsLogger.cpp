#include "StatsLogger.hpp"
#include <iostream>

StatsLogger& StatsLogger::instance() {
    static StatsLogger instance;
    return instance;
}

StatsLogger::StatsLogger() {
    m_file.open("game_stats.csv", std::ios::out | std::ios::trunc);
    if (m_file.is_open()) {
        m_file << "Event_Type,Value_1,Value_2,Value_3,Value_4\n";
        m_file.flush();
    } else {
        std::cerr << "StatsLogger ERROR: Could not open game_stats.csv for writing." << std::endl;
    }
}

StatsLogger::~StatsLogger() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void StatsLogger::logLightningEvent(int x, int y, float flashAlpha, float flashDecay) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file << "LightningStrike," << x << "," << y << "," << flashAlpha << "," << flashDecay << "\n";
        m_file.flush();
    }
}

void StatsLogger::logLightningPush(float distance) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file << "LightningPush," << distance << ",0,0,0\n";
        m_file.flush();
    }
}

void StatsLogger::logCursedSquare(int type, int value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        const char* typeStr = (type == 0) ? "CursedDuration" : "CursedCooldown";
        m_file << typeStr << "," << value << ",0,0,0\n";
        m_file.flush();
    }
}

void StatsLogger::logParticleExplosion(int count) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file << "ParticleExplosion," << count << ",0,0,0\n";
        m_file.flush();
    }
}

void StatsLogger::logParticleLifespan(float lifespan, float scale) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file << "ParticleData," << lifespan << "," << scale << ",0,0\n";
        m_file.flush();
    }
}

void StatsLogger::logPaladinAction(int successes) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file << "PaladinBinomial," << successes << ",0,0,0\n";
        m_file.flush();
    }
}
