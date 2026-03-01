#include "../include/Field.h"
#include "../include/Obstacle.h"
#include "../include/Ball.h"
#include <algorithm>
#include <cmath>

Field::Field(float width_m, float height_m)
    : width(width_m), height(height_m), goalHeight(6.0f), goalDepth(2.0f) {}

SDL_FPoint Field::worldToScreen(float worldX, float worldY,
                                int screenW, int screenH) const {
    // compute the same viewport used by render()
    int fieldW = (int)(screenW * 0.95f);
    int fieldH = fieldW / 2;
    int fieldX = (screenW - fieldW) / 2;
    int fieldY = screenH - fieldH;

    float sx = (float)fieldW / width;
    float sy = (float)fieldH / height;

    return SDL_FPoint{ fieldX + worldX * sx,
                       fieldY + worldY * sy };
}

SDL_Rect Field::getViewport(int screenW, int screenH) const {
    int fieldW = (int)(screenW * 0.95f);
    int fieldH = fieldW / 2;
    int fieldX = (screenW - fieldW) / 2;
    int fieldY = screenH - fieldH;
    return SDL_Rect{ fieldX, fieldY, fieldW, fieldH };
}

// ---------------------------------------------------------------------------
// obstacle & player helper implementations
// ---------------------------------------------------------------------------

void Field::addObstacle(const Obstacle &obs) {
    obstacles.push_back(obs);
}

void Field::handlePlayerCollision(Player &player) const {
    for (const Obstacle &obs : obstacles) {
        obs.resolvePlayerCollision(player);
    }
}

void Field::render(SDL_Renderer* renderer, int screenW, int screenH,
                      SDL_Texture *texture) const {
    int fieldW = (int)(screenW * 0.95f);
    int fieldH = fieldW / 2;
    SDL_Rect bg{(screenW - fieldW) / 2, screenH - fieldH, fieldW, fieldH};
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &bg);
    } else {
        // field background - dark green
        SDL_SetRenderDrawColor(renderer, 30, 120, 60, 255);
        SDL_RenderFillRect(renderer, &bg);
    }

    // scaling factors for world coordinates -> field viewport
    float sx = (float)fieldW / width;
    float sy = (float)fieldH / height;

    // helper lambda to transform an x/y pair
    auto toScreen = [&](float wx, float wy) {
        SDL_FPoint p = worldToScreen(wx, wy, screenW, screenH);
        return p;
    };

    // Draw center line (in world coords x = width/2 from y=0..height)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_FPoint p1 = toScreen(width / 2.0f, 0.0f);
    SDL_FPoint p2 = toScreen(width / 2.0f, height);
    SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);

    // Draw center circle
    int circleR = (int)(3.0f * std::min(sx, sy));
    SDL_FPoint center = toScreen(width / 2.0f, height / 2.0f);
    for (int dy = -circleR; dy <= circleR; ++dy) {
        int dx = (int)std::sqrt((float)(circleR * circleR - dy * dy));
        SDL_RenderDrawPoint(renderer, (int)center.x - dx, (int)center.y + dy);
        SDL_RenderDrawPoint(renderer, (int)center.x + dx, (int)center.y + dy);
    }

    // Draw goal zones
    float goalTopY = getGoalTop();
    float goalBottomY = getGoalBottom();

    int gt = (int)(goalTopY * sy);
    int gb = (int)(goalBottomY * sy);
    int gd = (int)(goalDepth * sx);

    // Left goal (blue tint) - convert to screen coords manually since we have
    // computed gt/gb/gd in field-relative pixels. fieldX offset must be added.
    int fieldX = bg.x;
    SDL_SetRenderDrawColor(renderer, 50, 100, 200, 120);
    SDL_Rect leftGoal = { fieldX, bg.y + gt, gd, gb - gt };
    SDL_RenderFillRect(renderer, &leftGoal);
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
    SDL_RenderDrawRect(renderer, &leftGoal);

    // Right goal (red tint)
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 120);
    SDL_Rect rightGoal = { fieldX + fieldW - gd, bg.y + gt, gd, gb - gt };
    SDL_RenderFillRect(renderer, &rightGoal);
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &rightGoal);

    // Draw the four barriers as a thin white border around the viewport
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Top wall
    SDL_RenderDrawLine(renderer, fieldX, bg.y, fieldX + fieldW, bg.y);
    // Bottom wall
    SDL_RenderDrawLine(renderer, fieldX, bg.y + fieldH - 1,
                       fieldX + fieldW, bg.y + fieldH - 1);
    // Left wall (excluding goal opening)
    SDL_RenderDrawLine(renderer, fieldX, bg.y, fieldX, bg.y + gt);
    SDL_RenderDrawLine(renderer, fieldX, bg.y + gb, fieldX, bg.y + fieldH);
    // Right wall (excluding goal opening)
    SDL_RenderDrawLine(renderer, fieldX + fieldW - 1, bg.y, fieldX + fieldW - 1, bg.y + gt);
    SDL_RenderDrawLine(renderer, fieldX + fieldW - 1, bg.y + gb,
                       fieldX + fieldW - 1, bg.y + fieldH);

    // render any obstacles after field elements so they appear on top
    for (const Obstacle &obs : obstacles) {
        obs.render(renderer, *this, screenW, screenH);
    }
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
            if (ball.pos.x - r < -2 * r) {
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
            if (ball.pos.x + r > width + 2 * r) {
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

    // finally handle collisions with obstacles
    for (const Obstacle &obs : obstacles) {
        obs.handleBallCollision(ball);
    }

    return 0;  // no goal
}
