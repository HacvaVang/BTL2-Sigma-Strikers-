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
        Vector normal = diff.normalized();
        float overlap = minDist - dist;

        // Push ball out by exactly the overlap amount (smooth, no teleport)
        pos += normal * overlap;

        // Project current velocity onto collision normal
        float velAlongNormal = vel.dot(normal);

        if (velAlongNormal < 0.0f) {
            // Ball is moving into the player -> reflect with slight energy loss
            // Restitution = 0.85 (not perfectly elastic, feels natural)
            vel -= normal * ((1.0f + 0.85f) * velAlongNormal);
        }

        // Ensure a minimum outward speed so the ball escapes contact,
        // but do NOT add speed if already moving away fast enough.
        float outwardSpeed = vel.dot(normal);
        float minOutward = 2.0f; // gentle push, not explosive
        if (outwardSpeed < minOutward) {
            vel += normal * (minOutward - outwardSpeed);
        }
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
    // map world position into field viewport
    SDL_FPoint p = field.worldToScreen(pos.x, pos.y, screenW, screenH);
    SDL_Rect vp = field.getViewport(screenW, screenH);
    float sx = (float)vp.w / field.getWidth();
    float sy = (float)vp.h / field.getHeight();

    int px = static_cast<int>(p.x);
    int py = static_cast<int>(p.y);

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
