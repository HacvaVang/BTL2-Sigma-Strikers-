#include "../include/Team.h"
#include <SDL.h>
#include <cmath>

// helper to clamp value between min and max
static float clamp(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void Player::update(float dt, const Uint8 *keyState, const Field *bounds) {
    Vector dir(0, 0);
    if (keyState[SDL_SCANCODE_W]) dir.y -= 1;
    if (keyState[SDL_SCANCODE_S]) dir.y += 1;
    if (keyState[SDL_SCANCODE_A]) dir.x -= 1;
    if (keyState[SDL_SCANCODE_D]) dir.x += 1;
    if (dir.x != 0 || dir.y != 0) {
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= len;
        pos += dir * speed * dt;
    }
    if (bounds) {
        pos.x = clamp(pos.x, 0.0f, bounds->getWidth());
        pos.y = clamp(pos.y, 0.0f, bounds->getHeight());
    }
}

// draw a simple filled circle like Ball::render does
static void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt(r*r - dy*dy));
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Player::render(SDL_Renderer *renderer, const Field &field,
                    int screenW, int screenH, SDL_Color color) const {
    float sx = screenW / field.getWidth();
    float sy = screenH / field.getHeight();
    int px = static_cast<int>(pos.x * sx);
    int py = static_cast<int>(pos.y * sy);
    int pr = 10; // fixed pixel radius for players
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    drawFilledCircle(renderer, px, py, pr);
}

void Team::update(float dt, const Uint8 *keyState, const Field *bounds) {
    getActivePlayer().update(dt, keyState, bounds);
}

void Team::handleEvent(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_e) {
        swapActive();
    }
}

void Team::render(SDL_Renderer *renderer, const Field &field,
                  int screenW, int screenH) const {
    // draw both players; active one in brighter colour
    p1.render(renderer, field, screenW, screenH,
              activeIndex == 0 ? SDL_Color{200,50,50,255} : SDL_Color{120,20,20,255});
    p2.render(renderer, field, screenW, screenH,
              activeIndex == 1 ? SDL_Color{200,50,50,255} : SDL_Color{120,20,20,255});
}

Player &Team::getActivePlayer() {
    return (activeIndex == 0) ? p1 : p2;
}

const Player &Team::getActivePlayer() const {
    return (activeIndex == 0) ? p1 : p2;
}

void Team::swapActive() {
    activeIndex = !activeIndex;
}
