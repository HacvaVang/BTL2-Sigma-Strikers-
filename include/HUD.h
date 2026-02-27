#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

// Simple Heads-Up Display that renders team scores and a countdown timer.
class HUD {
public:
    HUD();
    ~HUD();

    // Initialize TTF and load font. Returns false on failure.
    bool init(const std::string &fontPath, int fontSize = 28);

    // Render scores and remaining time onto the screen.
    void render(SDL_Renderer *renderer, int screenW, int screenH,
                int score1, int score2, float timeRemaining) const;

    // Render a centered message (e.g., "GOAL!" or "Game Over")
    void renderMessage(SDL_Renderer *renderer, int screenW, int screenH,
                       const std::string &message) const;

private:
    TTF_Font *font;
    TTF_Font *bigFont;

    void drawText(SDL_Renderer *renderer, const std::string &text,
                  int x, int y, SDL_Color color, TTF_Font *f = nullptr, bool centered = false) const;
};
