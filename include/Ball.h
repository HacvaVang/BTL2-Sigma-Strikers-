#pragma once

#include <SDL.h>

class Field; // forward declaration so Ball can reference it without including

// Simple moving ball for the hockey field.  Position and velocity are stored in
// metres and metres/second.  The radius is also in metres.
class Ball {
public:
    Ball(float startX = 0.0f, float startY = 0.0f,
         float startVx = 0.0f, float startVy = 0.0f,
         float r = 0.5f);

    // advance the ball by dt seconds (simple linear motion)
    void update(float dt);

    // draw the ball using the same scaling logic as the field for
    // consistency.  If a texture is provided it will be drawn centred at the
    // ball's screen position and scaled to its radius. Otherwise the ball is
    // rendered as a filled circle.
    void render(SDL_Renderer* renderer, const Field& field,
                int screenW, int screenH,
                SDL_Texture *texture = nullptr) const;

    float x, y;        // position in metres
    float vx, vy;      // velocity in metres per second
    float radius;      // metres
};
