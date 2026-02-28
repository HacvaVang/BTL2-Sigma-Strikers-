#include "../include/Field.h"
#include "../include/Ball.h"
#include <algorithm>
#include <cmath>

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
    float goalBot = getGoalBottom();
    float r = ball.radius;
    float e = 0.75f;  // restitution (energy kept per bounce)

    // Is ball centre vertically inside the goal opening?
    bool inGoalY = (ball.pos.y >= goalTop && ball.pos.y <= goalBot);

    // ================================================================
    // TOP WALL  (y = 0, normal points down into field)
    // ================================================================
    if (ball.pos.y - r < 0.0f) {
        ball.pos.y = r;
        if (ball.vel.y < 0.0f) ball.vel.y = -ball.vel.y * e;
    }

    // ================================================================
    // BOTTOM WALL  (y = height, normal points up)
    // ================================================================
    if (ball.pos.y + r > height) {
        ball.pos.y = height - r;
        if (ball.vel.y > 0.0f) ball.vel.y = -ball.vel.y * e;
    }

    // ================================================================
    // LEFT SIDE  (x = 0)
    // ================================================================
    if (ball.pos.x - r < 0.0f) {
        if (inGoalY) {
            // Ball is inside the left goal box (rectangle: x in [-goalDepth, 0])
            // Back wall of goal box
            if (ball.pos.x - r < -goalDepth) {
                return 1;  // GOAL! Team 2 scores
            }
            // Top wall of goal box
            if (ball.pos.y - r < goalTop) {
                ball.pos.y = goalTop + r;
                if (ball.vel.y < 0.0f) ball.vel.y = -ball.vel.y * e;
            }
            // Bottom wall of goal box
            if (ball.pos.y + r > goalBot) {
                ball.pos.y = goalBot - r;
                if (ball.vel.y > 0.0f) ball.vel.y = -ball.vel.y * e;
            }
        } else {
            // Solid left wall — bounce back
            ball.pos.x = r;
            if (ball.vel.x < 0.0f) ball.vel.x = -ball.vel.x * e;
        }
    }

    // ================================================================
    // RIGHT SIDE  (x = width)
    // ================================================================
    if (ball.pos.x + r > width) {
        if (inGoalY) {
            // Ball is inside the right goal box (rectangle: x in [width, width+goalDepth])
            // Back wall of goal box
            if (ball.pos.x + r > width + goalDepth) {
                return 2;  // GOAL! Team 1 scores
            }
            // Top wall of goal box
            if (ball.pos.y - r < goalTop) {
                ball.pos.y = goalTop + r;
                if (ball.vel.y < 0.0f) ball.vel.y = -ball.vel.y * e;
            }
            // Bottom wall of goal box
            if (ball.pos.y + r > goalBot) {
                ball.pos.y = goalBot - r;
                if (ball.vel.y > 0.0f) ball.vel.y = -ball.vel.y * e;
            }
        } else {
            // Solid right wall — bounce back
            ball.pos.x = width - r;
            if (ball.vel.x > 0.0f) ball.vel.x = -ball.vel.x * e;
        }
    }

    return 0;  // no goal
}
