#include "../include/Ball.h"
#include "../include/Field.h"
#include "../include/Team.h"
#include <cmath>
#include <algorithm>

Ball::Ball(const Vector& startPos,
           const Vector& startVel,
           float r)
    : pos(startPos), vel(startVel), radius(r), friction(0.98f) {}

void Ball::update(float dt) {
    // Apply velocity
    pos += vel * dt;

    // Apply friction (slight deceleration each frame)
    vel *= friction;

    // Stop ball if very slow
    if (vel.length() < 0.1f) {
        vel = Vector(0, 0);
    }
}

void Ball::handlePlayerCollision(const Player& player, float playerRadius) {
    Vector diff = pos - player.pos;
    float dist = diff.length();
    float minDist = radius + playerRadius;

    if (dist < minDist && dist > 0.001f) {
        // Push ball away from player
        Vector normal = diff.normalized();

        // Separate ball from player
        pos = player.pos + normal * minDist;

        // Calculate bounce velocity
        // The ball gets "hit" away from the player
        float hitSpeed = std::max(15.0f, vel.length() + 5.0f);
        vel = normal * hitSpeed;
    }
}

void Ball::reset(const Vector& centerPos, const Vector& startVel) {
    pos = centerPos;
    vel = startVel;
}

// helper: draw a filled circle centred at (cx,cy) with given pixel radius
static void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt(r*r - dy*dy));
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Ball::render(SDL_Renderer* renderer, const Field& field,
                  int screenW, int screenH,
                  SDL_Texture *texture) const {
    // convert world coordinates to screen
    float sx = screenW / field.getWidth();
    float sy = screenH / field.getHeight();

    int px = static_cast<int>(pos.x * sx);
    int py = static_cast<int>(pos.y * sy);

    // compute radius in pixels; prefer uniform scaling so ball remains circular
    int pr = static_cast<int>(radius * std::min(sx, sy));
    if (pr < 4) pr = 4;

    if (texture) {
        SDL_Rect dst{ px - pr, py - pr, pr * 2, pr * 2 };
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
    } else {
        // Draw a white puck
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawFilledCircle(renderer, px, py, pr);

        // Add a subtle border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        for (int deg = 0; deg < 360; ++deg) {
            float rad = deg * 3.14159f / 180.0f;
            int bx = px + (int)(pr * std::cos(rad));
            int by = py + (int)(pr * std::sin(rad));
            SDL_RenderDrawPoint(renderer, bx, by);
        }
    }
}
