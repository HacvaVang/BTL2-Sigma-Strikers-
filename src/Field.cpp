#include "../include/Field.h"
#include "../include/Ball.h"
#include <algorithm>

Field::Field(float width_m, float height_m)
    : width(width_m), height(height_m) {}

SDL_FPoint Field::worldToScreen(float worldX, float worldY,
                                int screenW, int screenH) const {
    // stretch the field to fill the window; maintain forces x/y separately
    float sx = screenW / width;
    float sy = screenH / height;
    return SDL_FPoint{ worldX * sx, worldY * sy };
}

void Field::render(SDL_Renderer* renderer, int screenW, int screenH,
                      SDL_Texture *texture) const {
    // if we have a background texture, stretch it to cover the window
    SDL_Rect bg{0, 0, screenW, screenH};
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &bg);
    } else {
        // field background
        SDL_SetRenderDrawColor(renderer, 30, 120, 60, 255);
        SDL_RenderFillRect(renderer, &bg);
    }

    // draw the four barriers as a thin white border (texture does not include)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect border{0, 0, screenW, screenH};
    SDL_RenderDrawRect(renderer, &border);
}

void Field::handleCollision(Ball& ball) const {
    // left wall
    if (ball.pos.x - ball.radius < 0.0f) {
        ball.pos.x = ball.radius;
        ball.vel.x = -ball.vel.x;
    }
    // right wall
    if (ball.pos.x + ball.radius > width) {
        ball.pos.x = width - ball.radius;
        ball.vel.x = -ball.vel.x;
    }
    // top wall
    if (ball.pos.y - ball.radius < 0.0f) {
        ball.pos.y = ball.radius;
        ball.vel.y = -ball.vel.y;
    }
    // bottom wall
    if (ball.pos.y + ball.radius > height) {
        ball.pos.y = height - ball.radius;
        ball.vel.y = -ball.vel.y;
    }
}
