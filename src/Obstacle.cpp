#include "../include/Obstacle.h"
#include "../include/Field.h"
#include "../include/Ball.h"
#include "../include/Team.h" // for Player definition

#include <algorithm>
#include <cmath>

static float clampVal(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void Obstacle::render(SDL_Renderer *renderer, const Field &field,
                      int screenW, int screenH) const {
    SDL_Rect vp = field.getViewport(screenW, screenH);
    float sx = (float)vp.w / field.getWidth();
    float sy = (float)vp.h / field.getHeight();

    // position of top-left corner in screen coords
    float screenX = vp.x + (pos.x - width / 2.0f) * sx;
    float screenY = vp.y + (pos.y - height / 2.0f) * sy;
    SDL_Rect rect = { (int)screenX, (int)screenY,
                      (int)(width * sx), (int)(height * sy) };

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

bool Obstacle::handleBallCollision(Ball &ball) const {
    // compute nearest point on rectangle to ball centre
    float halfw = width / 2.0f;
    float halfh = height / 2.0f;
    float minx = pos.x - halfw;
    float maxx = pos.x + halfw;
    float miny = pos.y - halfh;
    float maxy = pos.y + halfh;

    float cx = clampVal(ball.pos.x, minx, maxx);
    float cy = clampVal(ball.pos.y, miny, maxy);

    float dx = ball.pos.x - cx;
    float dy = ball.pos.y - cy;
    float dist2 = dx*dx + dy*dy;
    float r2 = ball.radius * ball.radius;
    if (dist2 < r2) {
        float dist = std::sqrt(dist2);
        if (dist < 1e-6f) {
            // centre inside obstacle, choose arbitrary normal
            dy = 1.0f;
            dist = 1.0f;
        }
        Vector norm = { dx / dist, dy / dist };
        // push ball out of obstacle
        ball.pos.x = cx + norm.x * ball.radius;
        ball.pos.y = cy + norm.y * ball.radius;

        // reflect velocity
        float vdot = ball.vel.x * norm.x + ball.vel.y * norm.y;
        if (vdot < 0.0f) {
            float e = 0.75f; // restitution
            ball.vel.x -= (1.0f + e) * vdot * norm.x;
            ball.vel.y -= (1.0f + e) * vdot * norm.y;
        }
        return true;
    }
    return false;
}

void Obstacle::resolvePlayerCollision(Player &player) const {
    float halfw = width / 2.0f;
    float halfh = height / 2.0f;
    float minx = pos.x - halfw;
    float maxx = pos.x + halfw;
    float miny = pos.y - halfh;
    float maxy = pos.y + halfh;

    float cx = clampVal(player.pos.x, minx, maxx);
    float cy = clampVal(player.pos.y, miny, maxy);

    float dx = player.pos.x - cx;
    float dy = player.pos.y - cy;
    float dist2 = dx*dx + dy*dy;
    float r2 = player.radius * player.radius;
    if (dist2 < r2) {
        float dist = std::sqrt(dist2);
        if (dist < 1e-6f) {
            dy = 1.0f;
            dist = 1.0f;
        }
        float overlap = player.radius - dist;
        player.pos.x += (dx / dist) * overlap;
        player.pos.y += (dy / dist) * overlap;
    }
}
