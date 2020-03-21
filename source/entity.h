#pragma once

#include "renderer.h"

enum CollisionBehaviour { RESET, SCORE };

class Entity {
public:
    float x, y;
    float width, height;
    float rot;
    bool up;
    Renderer* renderer;
    CollisionBehaviour collisionBehaviouor;

    Entity(float x, float y, float width, float height, Renderer* renderer) : x(x), y(y), width(width), height(height), rot(0), renderer(renderer) {}
    virtual ~Entity() {
        delete renderer;
    }

    virtual void update(float dt) {}

    void render() {
        renderer->render(x, y, width, height, rot);
    }

    bool isColliding(Entity* other) {
        return x + width / 2 > other->x - other->width / 2 && x - width / 2 < other->x + width / 2 &&
            y + height / 2 > other->y - other->height / 2 && y - height / 2 < other->y + height / 2;
    }
};

class EntityPlayer : public Entity {
public:
    bool isFlipping;
    float initY, initRot;

    const int waveHeight = 50.0f;

    EntityPlayer() : Entity(50.0f, SCREEN_HEIGHT / 2.0f, 20.0f, 20.0f, new QuadRenderer(C2D_Color32(0xA4, 0xDF, 0xE0, 0xFF))) {
        initY = y;
        y = initY + waveHeight - height / 2;
        up = false;
    }

    void update(float dt) override {
        float targetY = initY + (waveHeight - height / 2) * (up ? -1 : 1);
        float flipSpeed = 650;
        if(isFlipping) {
            if(up) {
                y -= dt * flipSpeed;
                if(y <= targetY) {
                    isFlipping = false;
                    y = targetY;
                }
            } else {
                y += dt * flipSpeed;
                if(y >= targetY) {
                    isFlipping = false;
                    y = targetY;
                }
            }

            float dist = y - initY;
            rot = initRot + 90 + dist / (waveHeight - height / 2) * 90;
            // rot *= DEG2RAD; // convert to radians
        } else {
            y = targetY;
            rot = initRot;
        }
    }

    void flip() {
        if (isFlipping) return;
        up = !up;
        isFlipping = true;
    }
};

class EntitySpike : public Entity {
public:
    EntitySpike() : Entity(SCREEN_WIDTH_TOP + 40.0f, 0, 25.0f, 25.0f, new TriRenderer(C2D_Color32(0xEB, 0xE1, 0xE1, 0xFF))) {
        collisionBehaviouor = RESET;
    }
};

class EntityCoin : public Entity {
public:
    EntityCoin() : Entity(SCREEN_WIDTH_TOP + 40.0f, 0, 12.0f, 12.0f, new QuadRenderer(C2D_Color32(0xFF, 0xED, 0x6A, 0xFF))) {
        collisionBehaviouor = SCORE;
    }

    void update(float dt) override {
        Entity::update(dt);
        rot += 400 * dt;
    }
};

class EntityWave : public Entity {
public:
    EntityWave() : Entity(0, 0, 1, 1, new WaveRenderer(C2D_Color32(0x6C, 0x6C, 0x6C, 0xFF))) {}

    void updateValues(float offset, float amp, float freq, float height) {
        WaveRenderer* waveRenderer = (WaveRenderer*) renderer;
        waveRenderer->offset = offset;
        waveRenderer->amp = amp;
        waveRenderer->freq = freq;
        waveRenderer->height = height;
    }
};