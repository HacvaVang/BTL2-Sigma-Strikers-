#include "../include/Field.h"
#include "../include/Ball.h"
#include <algorithm>

Field::Field(float width_m, float height_m)
    : width(width_m), height(height_m), goalHeight(6.0f), goalDepth(2.0f) {}

SDL_FPoint Field::worldToScreen(float worldX, float worldY,
                                int screenW, int screenH) const {
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
        // field background - dark green
        SDL_SetRenderDrawColor(renderer, 30, 120, 60, 255);
        SDL_RenderFillRect(renderer, &bg);
    }

    float sx = (float)screenW / width;
    float sy = (float)screenH / height;

    // Draw center line
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    int centerX = screenW / 2;
    SDL_RenderDrawLine(renderer, centerX, 0, centerX, screenH);

    // Draw center circle
    int circleR = (int)(3.0f * std::min(sx, sy));
    for (int dy = -circleR; dy <= circleR; ++dy) {
        int dx = (int)std::sqrt((float)(circleR * circleR - dy * dy));
        SDL_RenderDrawPoint(renderer, centerX - dx, screenH / 2 + dy);
        SDL_RenderDrawPoint(renderer, centerX + dx, screenH / 2 + dy);
    }

    // Draw goal zones
    float goalTopY = getGoalTop();
    float goalBottomY = getGoalBottom();

    int gt = (int)(goalTopY * sy);
    int gb = (int)(goalBottomY * sy);
    int gd = (int)(goalDepth * sx);

    // Left goal (blue tint)
    SDL_SetRenderDrawColor(renderer, 50, 100, 200, 120);
    SDL_Rect leftGoal = {0, gt, gd, gb - gt};
    SDL_RenderFillRect(renderer, &leftGoal);
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
    SDL_RenderDrawRect(renderer, &leftGoal);

    // Right goal (red tint)
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 120);
    SDL_Rect rightGoal = {screenW - gd, gt, gd, gb - gt};
    SDL_RenderFillRect(renderer, &rightGoal);
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &rightGoal);

    // Draw the four barriers as a thin white border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Top wall
    SDL_RenderDrawLine(renderer, 0, 0, screenW, 0);
    // Bottom wall
    SDL_RenderDrawLine(renderer, 0, screenH - 1, screenW, screenH - 1);
    // Left wall (excluding goal opening)
    SDL_RenderDrawLine(renderer, 0, 0, 0, gt);
    SDL_RenderDrawLine(renderer, 0, gb, 0, screenH);
    // Right wall (excluding goal opening)
    SDL_RenderDrawLine(renderer, screenW - 1, 0, screenW - 1, gt);
    SDL_RenderDrawLine(renderer, screenW - 1, gb, screenW - 1, screenH);
}

int Field::handleCollision(Ball& ball) const {
    float goalTop = getGoalTop();
    float goalBottom = getGoalBottom();

    // Top wall
    if (ball.pos.y - ball.radius < 0.0f) {
        ball.pos.y = ball.radius;
        ball.vel.y = -ball.vel.y;
    }
    // Bottom wall
    if (ball.pos.y + ball.radius > height) {
        ball.pos.y = height - ball.radius;
        ball.vel.y = -ball.vel.y;
    }

    // Left wall - check if ball is in goal zone
    if (ball.pos.x - ball.radius < 0.0f) {
        if (ball.pos.y >= goalTop && ball.pos.y <= goalBottom) {
            // Ball entered left goal - team 2 scores
            if (ball.pos.x < -goalDepth) {
                return 1; // left goal scored
            }
            // Let ball continue into goal zone
        } else {
            ball.pos.x = ball.radius;
            ball.vel.x = -ball.vel.x;
        }
    }

    // Right wall - check if ball is in goal zone
    if (ball.pos.x + ball.radius > width) {
        if (ball.pos.y >= goalTop && ball.pos.y <= goalBottom) {
            // Ball entered right goal - team 1 scores
            if (ball.pos.x > width + goalDepth) {
                return 2; // right goal scored
            }
            // Let ball continue into goal zone
        } else {
            ball.pos.x = width - ball.radius;
            ball.vel.x = -ball.vel.x;
        }
    }

    return 0; // no goal
}
