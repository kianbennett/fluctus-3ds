#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cfloat>
#include <algorithm>
#include "entity.h"
#include "enum.h"

extern const int SCREEN_HEIGHT;
extern float score;
extern GameState state;
extern void changeState(GameState s);

class Level {
public:
    float waveAmp, waveFreq, waveHeight, waveSpeed;
    float waveAmpInit, waveFreqInit, waveSpeedInit;
    float spawnIntervalMin, spawnIntervalMax;
    float spawnIntervalMinInit, spawnIntervalMaxInit;

    EntityPlayer* player;
    EntityWave* wave;

    std::vector<Entity*> spawnedEntities;

    Level();
    void update(float dt);
    void render();
    void cleanup();
    void reset();
    void handleDifficulty();
    void onStateChange(GameState state);
    
    void spawnWave();
    void spawnPlayer();
    void spawnEntity(Entity* entity);
    void destroyEntity(Entity* entity);

private:
    float waveOffset, spawnTick, spawnIntervalNext, timeElapsed;
    bool lastUp;

    float getWavePointY(float x);
    float getWavePointAngle(float x);

    float clamp(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }
};