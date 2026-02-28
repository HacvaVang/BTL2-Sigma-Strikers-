#include "../include/Team.h"
#include <SDL.h>
#include <cmath>

// helper to clamp value between min and max
static float clamp(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void Player::update(float dt, const Uint8 *keyState, const KeyBindings &keys,
                    const Field *bounds) {
    Vector dir(0, 0);
    if (keyState[keys.up])    dir.y -= 1;
    if (keyState[keys.down])  dir.y += 1;
    if (keyState[keys.left])  dir.x -= 1;
    if (keyState[keys.right]) dir.x += 1;
    if (dir.x != 0 || dir.y != 0) {
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= len;
        pos += dir * speed * dt;
    }
    if (bounds) {
        pos.x = clamp(pos.x, radius, bounds->getWidth() - radius);
        pos.y = clamp(pos.y, radius, bounds->getHeight() - radius);
    }
}

// draw a simple filled circle
static void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt(r*r - dy*dy));
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Player::render(SDL_Renderer *renderer, const Field &field,
                    int screenW, int screenH, SDL_Color color, SDL_Texture *tex) const {
    float sx = screenW / field.getWidth();
    float sy = screenH / field.getHeight();
    int px = static_cast<int>(pos.x * sx);
    int py = static_cast<int>(pos.y * sy);
    int pr = static_cast<int>(radius * std::min(sx, sy));
    if (pr < 6) pr = 6;

    if (tex) {
        // Draw the player sprite, tinted with team color
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        // Scale sprite to fit nicely on the field
        int spriteSize = pr * 3;  // 1.5x diameter
        SDL_Rect dst = { px - spriteSize / 2, py - spriteSize / 2,
                         spriteSize, spriteSize };
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        // Reset tint
        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    } else {
        // Fallback: draw circle
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        drawFilledCircle(renderer, px, py, pr);

        // Outline
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        for (int deg = 0; deg < 360; deg += 2) {
            float rad = deg * 3.14159f / 180.0f;
            int bx = px + (int)(pr * std::cos(rad));
            int by = py + (int)(pr * std::sin(rad));
            SDL_RenderDrawPoint(renderer, bx, by);
        }
    }
}

void Team::update(float dt, const Uint8 *keyState, const Field *bounds) {
    getActivePlayer().update(dt, keyState, keys, bounds);
}

void Team::handleEvent(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == keys.swap) {
        swapActive();
    }
}

void Team::render(SDL_Renderer *renderer, const Field &field,
                  int screenW, int screenH,
                  SDL_Color activeColor, SDL_Color inactiveColor,
                  SDL_Texture *playerTex) const {
    // Draw inactive player (dimmer)
    const Player &inactive = (activeIndex == 0) ? p2 : p1;
    inactive.render(renderer, field, screenW, screenH, inactiveColor, playerTex);

    // Draw active player (brighter) on top
    const Player &active = (activeIndex == 0) ? p1 : p2;
    active.render(renderer, field, screenW, screenH, activeColor, playerTex);

    // Draw an indicator arrow above the active player
    float sx = screenW / field.getWidth();
    float sy = screenH / field.getHeight();
    int ax = static_cast<int>(active.pos.x * sx);
    int ay = static_cast<int>(active.pos.y * sy);
    int arrPr = static_cast<int>(active.radius * std::min(sx, sy));
    if (arrPr < 6) arrPr = 6;
    int sprHalf = arrPr * 3 / 2; // match sprite half-size

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    // Small triangle above player sprite
    int triTop = ay - sprHalf - 12;
    int triBot = ay - sprHalf - 4;
    SDL_RenderDrawLine(renderer, ax, triTop, ax - 5, triBot);
    SDL_RenderDrawLine(renderer, ax, triTop, ax + 5, triBot);
    SDL_RenderDrawLine(renderer, ax - 5, triBot, ax + 5, triBot);
}

Player &Team::getActivePlayer() {
    return (activeIndex == 0) ? p1 : p2;
}

const Player &Team::getActivePlayer() const {
    return (activeIndex == 0) ? p1 : p2;
}

Player &Team::getInactivePlayer() {
    return (activeIndex == 0) ? p2 : p1;
}

const Player &Team::getInactivePlayer() const {
    return (activeIndex == 0) ? p2 : p1;
}

void Team::swapActive() {
    activeIndex = 1 - activeIndex;
}

void Team::resetPositions(const Vector &start1, const Vector &start2) {
    p1.pos = start1;
    p2.pos = start2;
}

// new helper for Player movement
void Player::move(float dx, float dy, float dt, const Field *bounds) {
    if (dx != 0 || dy != 0) {
        float len = std::sqrt(dx*dx + dy*dy);
        Vector dir = { dx/len, dy/len };
        pos += dir * speed * dt;
    }
    if (bounds) {
        pos.x = clamp(pos.x, 0.0f, bounds->getWidth());
        pos.y = clamp(pos.y, 0.0f, bounds->getHeight());
    }
}
