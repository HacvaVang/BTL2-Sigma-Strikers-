#pragma once

#include "Vector.h"
#include <SDL.h>

class Field;
class Ball;
class Player;

// Simple axis-aligned rectangular obstacle placed on the field.  The
// position is specified in world coordinates (metres) referring to the
// obstacle's centre.  The obstacle does not move at runtime; it merely
// collides with the ball and players, pushing them away or bouncing the
// ball back.
class Obstacle {
public:
    Obstacle(const Vector &centre = Vector(), float w = 1.0f, float h = 1.0f)
        : pos(centre), width(w), height(h) {}

    // Render the obstacle inside the provided field viewport.
    void render(SDL_Renderer *renderer, const Field &field,
                int screenW, int screenH) const;

    // Check and resolve a collision between this obstacle and the ball.
    // If an intersection is detected the ball is moved out of the obstacle
    // and its velocity is reflected.  Returns true if a collision occurred.
    bool handleBallCollision(Ball &ball) const;

    // Push a player out of the obstacle if they overlap.  This is used by
    // Field::handlePlayerCollision.
    void resolvePlayerCollision(Player &player) const;

    Vector getPos() const { return pos; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

private:
    Vector pos;   // centre in metres
    float width;  // metres
    float height; // metres
};
