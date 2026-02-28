#pragma once

#include <SDL.h>

class Ball; // forward

// Represents a rectangular hockey field with boundary barriers and goal zones.
// The field dimensions are specified in metres; rendering is scaled to the
// current window resolution so that the entire field always fits on screen.
class Field {
public:
    // width_m and height_m are real-world dimensions in metres (40x20 by default).
    Field(float width_m = 40.0f, float height_m = 20.0f);

    // Draw the field background and border using the provided renderer and
    // current window size. If a non-null texture is supplied it will be
    // stretched to cover the entire window; otherwise a solid colour with a
    // border is drawn. Also draws goal zones and center line.
    void render(SDL_Renderer* renderer, int screenW, int screenH,
                SDL_Texture *texture = nullptr) const;

    // Check a ball against the four walls and reflect its velocity if it
    // intersects a barrier (except in goal zones).
    // Returns: 0 = no goal, 1 = left goal (team2 scores), 2 = right goal (team1 scores)
    int handleCollision(Ball& ball) const;

    float getWidth() const { return width; }
    float getHeight() const { return height; }

    // Goal zone dimensions (in metres)
    float getGoalTop() const { return height / 2.0f - goalHeight / 2.0f; }
    float getGoalBottom() const { return height / 2.0f + goalHeight / 2.0f; }
    float getGoalDepth() const { return goalDepth; }
    float getGoalHeight() const { return goalHeight; }

    // Transform from field world-space into the rectangle where the field is
    // actually drawn. The viewport is used by rendering routines for players
    // and the ball so they stay inside the pitch rather than the whole window.
    SDL_Rect getViewport(int screenW, int screenH) const;
    // convert a world coordinate to screen pixels
    SDL_FPoint worldToScreen(float worldX, float worldY, int screenW, int screenH) const;


private:
    float width;   // metres
    float height;  // metres
    float goalHeight; // height of goal opening (metres)
    float goalDepth;  // how deep the goal extends behind the wall (metres)

};
