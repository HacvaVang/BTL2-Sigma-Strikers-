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
    // legacy convenience wrapper for standard WASD controls
    Vector dir(0, 0);
    if (keyState[SDL_SCANCODE_W]) dir.y -= 1;
    if (keyState[SDL_SCANCODE_S]) dir.y += 1;
    if (keyState[SDL_SCANCODE_A]) dir.x -= 1;
    if (keyState[SDL_SCANCODE_D]) dir.x += 1;
    move(dir.x, dir.y, dt, bounds);
}

// draw a simple filled circle like Ball::render does (used when no sprite)
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

    if (tex) {
        // draw texture centred on player
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        SDL_Rect dst{ px - w/2, py - h/2, w, h };
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
    } else {
        int pr = 10; // fixed pixel radius for players
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        drawFilledCircle(renderer, px, py, pr);
    }
}

void Team::update(float dt, const Uint8 *keyState, const Field *bounds) {
    // compute direction according to this team's control mapping
    Vector dir(0,0);
    if (keyState[up]) dir.y -= 1;
    if (keyState[down]) dir.y += 1;
    if (keyState[left]) dir.x -= 1;
    if (keyState[right]) dir.x += 1;
    getActivePlayer().move(dir.x, dir.y, dt, bounds);
}

void Team::handleEvent(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == swapKey) {
        swapActive();
    }
}

void Team::render(SDL_Renderer *renderer, const Field &field,
                  int screenW, int screenH, SDL_Texture *playerTex) const {
    // draw both players; active one in brighter colour.  use the team's base
    // color which may vary between teams.
    SDL_Color bright = teamColor;
    SDL_Color dark = { static_cast<Uint8>(teamColor.r * 0.6f),
                       static_cast<Uint8>(teamColor.g * 0.6f),
                       static_cast<Uint8>(teamColor.b * 0.6f),
                       teamColor.a };

    p1.render(renderer, field, screenW, screenH,
              activeIndex == 0 ? bright : dark,
              playerTex);
    p2.render(renderer, field, screenW, screenH,
              activeIndex == 1 ? bright : dark,
              playerTex);
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
