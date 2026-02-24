#include "../include/Ball.h"
#include "../include/Field.h"
#include <cmath> // for sqrt
#include <algorithm> // for std::min

Ball::Ball(float startX, float startY,
           float startVx, float startVy,
           float r)
    : x(startX), y(startY), vx(startVx), vy(startVy), radius(r) {}

void Ball::update(float dt) {
    x += vx * dt;
    y += vy * dt;
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

    int px = static_cast<int>(x * sx);
    int py = static_cast<int>(y * sy);

    // compute radius in pixels; prefer uniform scaling so ball remains circular
    int pr = static_cast<int>(radius * std::min(sx, sy));
    if (texture) {
        SDL_Rect dst{ px - pr, py - pr, pr * 2, pr * 2 };
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawFilledCircle(renderer, px, py, pr);
    }
}
