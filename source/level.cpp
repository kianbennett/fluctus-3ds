#include "level.h"

Level::Level() {
    waveAmpInit = 30.0f;
    waveFreqInit = 0.005f;
    waveSpeedInit = 250.0f;
    waveHeight = 100.0f;
    spawnIntervalMinInit = 0.5f;
    spawnIntervalMaxInit = 1.5f;
}

void Level::update(float dt) {
    waveOffset += dt * waveSpeed;

    if(wave != nullptr) {
        wave->update(dt);
        wave->updateValues(waveOffset, waveAmp, waveFreq, waveHeight);
    }

    if(state == GameState::InGame) {
        score += dt * 10;
        timeElapsed += dt;
        handleDifficulty();

        if(player != nullptr) {
            player->update(dt);
            player->initY = getWavePointY(player->x);
            player->initRot = getWavePointAngle(player->x);
        }
        
        spawnTick += dt;
        if(spawnTick >= spawnIntervalNext) {
            // Set the next spawn interval as a random value between max and min
            spawnIntervalNext = spawnIntervalMin + ((rand() % (int) ((spawnIntervalMax - spawnIntervalMin) * 100.0f)) / 100.0f); // 0.5 - 1.5s
            spawnTick = 0;
            // 1 in 6 chance of spawning a coin instead of a spike
            int r = rand() % 6;
            spawnEntity(r ? (Entity*) new EntitySpike() : (Entity*) new EntityCoin());
        }

        for(int i = (int) spawnedEntities.size() - 1; i >= 0; i--) {
            spawnedEntities[i]->update(dt);
            // Move entities to the left at the same speed as the wave
            spawnedEntities[i]->x -= waveSpeed * dt;
            // Destroy entity if it goes too far to the left
            if(spawnedEntities[i]->x < -40) {
                destroyEntity(spawnedEntities[i]);
                spawnedEntities.erase(spawnedEntities.begin() + i);
            }
            if(player != nullptr && player->isColliding(spawnedEntities[i])) {
                switch (spawnedEntities[i]->collisionBehaviouor) {
                    case RESET:
                        changeState(GameState::PostGame);
                        return;
                        break;
                    case SCORE:
                        destroyEntity(spawnedEntities[i]);
                        spawnedEntities.erase(spawnedEntities.begin() + i);
                        score += 20;
                        break;
                }
            }
        }
    }
}

void Level::render() {
    if(wave != nullptr) wave->render();
    if(player != nullptr) player->render();
    for(int i = 0; i < (int) spawnedEntities.size(); i++) spawnedEntities[i]->render();
}

void Level::cleanup() {
    destroyEntity(wave);
    destroyEntity(player);
    wave = nullptr;
    player = nullptr;
    for(int i = 0; i < (int) spawnedEntities.size(); i++) destroyEntity(spawnedEntities[i]);
    spawnedEntities.clear();
}

void Level::reset() {
    waveSpeed = waveSpeedInit;
    spawnIntervalMin = spawnIntervalMinInit;
    spawnIntervalMax = spawnIntervalMaxInit;
    spawnIntervalNext = spawnIntervalMin;
    waveAmp = waveAmpInit;
    waveFreq = waveFreqInit;
    timeElapsed = 0;
    spawnTick = 0;
    
    destroyEntity(player);
    player = nullptr;
    for(int i = 0; i < (int) spawnedEntities.size(); i++) destroyEntity(spawnedEntities[i]);
    spawnedEntities.clear();
}

void Level::handleDifficulty() {
    if(timeElapsed > 1) { 
        float time = timeElapsed - 1;
        waveSpeed = clamp(waveSpeedInit + (time * 4.0f), 0.0f, 550.0f);
        spawnIntervalMin = clamp(spawnIntervalMinInit - (time / 60), 0.20f, FLT_MAX);
        spawnIntervalMax = clamp(spawnIntervalMaxInit - (time / 50), 0.5f, FLT_MAX);
        waveAmp = clamp(waveAmpInit + (time / 3.0f), 0.0f, 80.0f);
        waveFreq = clamp(waveFreqInit + (time * 0.00005f), 0.0f, 0.005f);
    }
}

void Level::onStateChange(GameState state) {
    reset();
    switch(state) {
        case GameState::PreGame:
            break;
        case GameState::InGame:
            spawnPlayer();
            break;
        case GameState::PostGame:
            break;
    }
}

void Level::spawnWave() {
    wave = new EntityWave();
}

void Level::spawnPlayer() {
    player = new EntityPlayer();
}

void Level::spawnEntity(Entity* entity) {
    // If last was up, only 1/3 chance of being up again, to avoid having too many on one side in a row
    if(lastUp) {
        entity->up = !(rand() % 4);
    } else {
        entity->up = rand() % 4;
    }
    lastUp = entity->up;

    entity->rot = getWavePointAngle(entity->x);
    entity->y = getWavePointY(entity->x) + (waveHeight / 2 - 12.5f) * (entity->up ? -1 : 1);
    if(!entity->up) entity->height *= -1;
    spawnedEntities.push_back(entity);
}

void Level::destroyEntity(Entity* entity) {
    if(entity != nullptr) delete entity;
}

float Level::getWavePointY(float x) {
    return SCREEN_HEIGHT / 2 + waveAmp * sin(waveFreq * (x + waveOffset));
}

// Get tangent angle by getting the angle between two points on the curve slightly apart
float Level::getWavePointAngle(float x) {
    float x1 = x - 1;
    float y1 = getWavePointY(x1);
    float x2 = x + 1;
    float y2 = getWavePointY(x2);
    return atan((y2 - y1) / (x2 - x1)) * 180.0f / 3.14159265f;
}