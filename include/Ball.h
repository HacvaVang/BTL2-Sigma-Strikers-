#pragma once
#include <SDL.h>
#include "Vector.h"

class Field; // forward declaration so Ball can reference it without including

// Simple moving ball for the hockey field.  Position and velocity are stored in
// metres and metres/second.  The radius is also in metres.
class Ball {
public:
    // The constructor takes optional starting position and velocity vectors
    // (defaulting to zero) along with a radius.
    Ball(const Vector& startPos = Vector(),
         const Vector& startVel = Vector(),
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

    Vector pos;        // position in metres
    Vector vel;        // velocity in metres per second
    float radius;      // metres
};
