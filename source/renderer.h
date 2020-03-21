#pragma once

#include <citro2d.h>
#include <math.h>
#include <vector>

extern const float DEG2RAD;
extern const int SCREEN_WIDTH_TOP;
extern const int SCREEN_HEIGHT;

class Renderer {
public:
    Renderer(u32 colour) : colour(colour) {}
    virtual ~Renderer() {}
    virtual void render(float x, float y, float w, float h, float rot) = 0;
protected:
    u32 colour;
};

class QuadRenderer : public Renderer {
public:
    QuadRenderer(u32 colour) : Renderer(colour) {}

    void render(float x, float y, float w, float h, float rot) override {
        rot *= DEG2RAD;

        float x1 = -w / 2; // top left
        float y1 = -h / 2;
        float x2 =  w / 2; // top right
        float y2 = -h / 2;
        float x3 =  w / 2; // bottom right
        float y3 =  h / 2;
        float x4 = -w / 2; // bottom left
        float y4 =  h / 2;

        float x1rot = x1 * cos(rot) - y1 * sin(rot);
        float y1rot = x1 * sin(rot) + y1 * cos(rot);
        float x2rot = x2 * cos(rot) - y2 * sin(rot);
        float y2rot = x2 * sin(rot) + y2 * cos(rot);
        float x3rot = x3 * cos(rot) - y3 * sin(rot);
        float y3rot = x3 * sin(rot) + y3 * cos(rot);
        float x4rot = x4 * cos(rot) - y4 * sin(rot);
        float y4rot = x4 * sin(rot) + y4 * cos(rot);

        C2D_DrawTriangle(x + x1rot, y + y1rot, colour, x + x2rot, y + y2rot, colour, x + x3rot, y + y3rot, colour, 0);
        C2D_DrawTriangle(x + x3rot, y + y3rot, colour, x + x4rot, y + y4rot, colour, x + x1rot, y + y1rot, colour, 0);
    }
};

class TriRenderer : public Renderer {
public:
    TriRenderer(u32 colour) : Renderer(colour) {}

    void render(float x, float y, float w, float h, float rot) override {
        rot *= DEG2RAD;

        float x1 = -w / 2;  // left
        float y1 = -h / 2;
        float x2 =  0;       // top
        float y2 =  h / 2;
        float x3 =  w / 2;  // right
        float y3 = -h / 2;

        float x1rot = x1 * cos(rot) - y1 * sin(rot);
        float y1rot = x1 * sin(rot) + y1 * cos(rot);
        float x2rot = x2 * cos(rot) - y2 * sin(rot);
        float y2rot = x2 * sin(rot) + y2 * cos(rot);
        float x3rot = x3 * cos(rot) - y3 * sin(rot);
        float y3rot = x3 * sin(rot) + y3 * cos(rot);

        C2D_DrawTriangle(x + x1rot, y + y1rot, colour, x + x2rot, y + y2rot, colour, x + x3rot, y + y3rot, colour, 0);
    }
};

class WaveRenderer : public Renderer {
public:
    float offset, amp, freq, height;

    WaveRenderer(u32 colour) : Renderer(colour) {}

    void updateValues(float offset, float amp, float freq, float height) {
        this->offset = offset;
        this->amp = amp;
        this->freq = freq;
        this->height = height;
    }

    void render(float x, float y, float w, float h, float rot) override {
        int segments = 25;
        float segmentWidth = SCREEN_WIDTH_TOP / (float) segments;

        for(int i = 0; i < segments; i++) {
            float segX = i * segmentWidth;
            float nextSegX = (i + 1) * segmentWidth;
            float segY = SCREEN_HEIGHT / 2.0f + amp * sin(freq * (segX + offset));
            float nextSegY = SCREEN_HEIGHT / 2.0f + amp * sin(freq * (nextSegX + offset));

            float vertices[12];

            vertices[0] = segX;				    // top left
            vertices[1] = segY - height / 2;
            vertices[2] = segX + segmentWidth;	    // bottom right
            vertices[3] = nextSegY + height / 2;
            vertices[4] = segX;					// bottom left
            vertices[5] = segY + height / 2;
            vertices[6] = segX;					// top left
            vertices[7] = segY - height / 2;
            vertices[8] = segX + segmentWidth;	    // top right
            vertices[9] = nextSegY - height / 2;
            vertices[10] = segX + segmentWidth;    // bottom right
            vertices[11] = nextSegY + height / 2;

            C2D_DrawTriangle(x + vertices[0], y + vertices[1], colour, x + vertices[2], y + vertices[3], colour, x + vertices[4], y + vertices[5], colour, 0);
            C2D_DrawTriangle(x + vertices[6], y + vertices[7], colour, x + vertices[8], y + vertices[9], colour, x + vertices[10], y + vertices[11], colour, 0);
        }
    }
};