#include "../include/HUD.h"
#include <cmath>
#include <sstream>
#include <iomanip>

HUD::HUD() : font(nullptr), bigFont(nullptr) {}

HUD::~HUD() {
    if (font) TTF_CloseFont(font);
    if (bigFont) TTF_CloseFont(bigFont);
}

bool HUD::init(const std::string &fontPath, int fontSize) {
    if (TTF_Init() == -1) {
        return false;
    }
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) return false;

    bigFont = TTF_OpenFont(fontPath.c_str(), fontSize * 2);
    if (!bigFont) {
        bigFont = font; // fallback
    }

    return true;
}

void HUD::drawText(SDL_Renderer *renderer, const std::string &text,
                   int x, int y, SDL_Color color, TTF_Font *f, bool centered) const {
    TTF_Font *useFont = f ? f : font;
    if (!useFont) return;

    SDL_Surface *surf = TTF_RenderText_Blended(useFont, text.c_str(), color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        SDL_Rect dst;
        dst.w = surf->w;
        dst.h = surf->h;
        if (centered) {
            dst.x = x - surf->w / 2;
            dst.y = y - surf->h / 2;
        } else {
            dst.x = x;
            dst.y = y;
        }
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

void HUD::render(SDL_Renderer *renderer, int screenW, int screenH,
                 int score1, int score2, float timeRemaining) const {
    // Semi-transparent top bar background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect topBar = {0, 0, screenW, 50};
    SDL_RenderFillRect(renderer, &topBar);

    // Team 1 score (left side - blue team)
    SDL_Color blueColor = {100, 150, 255, 255};
    SDL_Color redColor = {255, 100, 100, 255};
    SDL_Color whiteColor = {255, 255, 255, 255};
    SDL_Color goldColor = {255, 215, 0, 255};

    // Team labels and scores
    std::string team1Text = std::to_string(score1);
    std::string team2Text = std::to_string(score2);

    drawText(renderer, team1Text, (int)(screenW * .25f), 25, blueColor, bigFont, true);
    drawText(renderer, team2Text, (int)(screenW * .75f), 25, redColor, bigFont, true );

    // Timer in center
    int minutes = (int)timeRemaining / 60;
    int seconds = (int)timeRemaining % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;

    // Flash timer when low
    SDL_Color timerColor = whiteColor;
    if (timeRemaining <= 10.0f) {
        timerColor = ((int)(timeRemaining * 3) % 2 == 0) ? redColor : whiteColor;
    } else if (timeRemaining <= 30.0f) {
        timerColor = goldColor;
    }

    drawText(renderer, oss.str(), screenW / 2, 25, timerColor, bigFont, true);

    // // Score separator
    // std::string scoreDisplay = std::to_string(score1) + " - " + std::to_string(score2);
    // drawText(renderer, scoreDisplay, screenW / 2, 8, whiteColor, nullptr, true);
}

void HUD::renderMessage(SDL_Renderer *renderer, int screenW, int screenH,
                        const std::string &message) const {
    // Dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
    SDL_Rect overlay = {screenW / 4, screenH / 3, screenW / 2, screenH / 3};
    SDL_RenderFillRect(renderer, &overlay);

    // Border
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &overlay);

    // Message text
    SDL_Color gold = {255, 215, 0, 255};
    drawText(renderer, message, screenW / 2, screenH / 2, gold, bigFont, true);
}
