#include "../include/Menu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <iostream>

// Global settings with defaults
GameSettings gSettings = { 120 }; // 2 minutes default

// -----------------------------------------------------------------------------
// Helper: draw centered text in a rect
// -----------------------------------------------------------------------------
static void drawText(SDL_Renderer* renderer, TTF_Font* font,
                     const std::string &text, SDL_Rect dst, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        SDL_Rect r = dst;
        r.w = surf->w;
        r.h = surf->h;
        r.x += (dst.w - surf->w) / 2;
        r.y += (dst.h - surf->h) / 2;
        SDL_RenderCopy(renderer, tex, nullptr, &r);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

// Helper: draw centered text at absolute position
static void drawTextCentered(SDL_Renderer* renderer, TTF_Font* font,
                              const std::string &text, int cx, int cy, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        SDL_Rect r;
        r.w = surf->w;
        r.h = surf->h;
        r.x = cx - surf->w / 2;
        r.y = cy - surf->h / 2;
        SDL_RenderCopy(renderer, tex, nullptr, &r);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

// =============================================================================
// Resolution Menu
// =============================================================================
bool showResolutionMenu(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return false;

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }

    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return false;
    }

    std::vector<std::pair<int,int>> options = {
        {800,600}, {1024,768}, {1280,720}, {1366,768}, {1920,1080}
    };
    int selected = 0;
    bool menuRunning = true;
    SDL_Event e;

    while (menuRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selected = (selected - 1 + (int)options.size()) % options.size();
                        break;
                    case SDLK_DOWN:
                        selected = (selected + 1) % (int)options.size();
                        break;
                    case SDLK_ESCAPE:
                        TTF_CloseFont(font);
                        return false;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER: {
                        int w = options[selected].first;
                        int h = options[selected].second;
                        std::cout << "Applying resolution: " << w << "x" << h << std::endl;
                        bool ok = app.setResolution(w, h);
                        TTF_CloseFont(font);
                        return ok;
                    }
                    default: break;
                }
            }
        }

        int winW = app.getWidth();
        int winH = app.getHeight();

        SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
        SDL_RenderClear(renderer);

        // Title
        TTF_Font* titleFont = TTF_OpenFont("assets/fonts/LEMONMILK-Medium.otf", 36);
        if (titleFont) {
            drawTextCentered(renderer, titleFont, "RESOLUTION", winW / 2, 60, {255, 215, 0, 255});
            TTF_CloseFont(titleFont);
        }

        int itemH = 55;
        int spacing = 10;
        int totalH = (int)options.size() * itemH + ((int)options.size() - 1) * spacing;
        int startY = winH / 2 - totalH / 2;

        for (size_t i = 0; i < options.size(); ++i) {
            int x = winW / 2 - 140;
            int y = startY + (int)i * (itemH + spacing);
            SDL_Rect r{x, y, 280, itemH};

            if ((int)i == selected) {
                // Highlighted - gradient-like effect
                SDL_SetRenderDrawColor(renderer, 60, 80, 180, 255);
                SDL_RenderFillRect(renderer, &r);
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
                SDL_RenderDrawRect(renderer, &r);
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                SDL_RenderFillRect(renderer, &r);
                SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
                SDL_RenderDrawRect(renderer, &r);
            }

            std::string resText = std::to_string(options[i].first) + " x " + std::to_string(options[i].second);
            SDL_Color textColor = ((int)i == selected) ? SDL_Color{255, 255, 255, 255} : SDL_Color{160, 160, 180, 255};
            drawText(renderer, font, resText, r, textColor);
        }

        // Instructions
        drawTextCentered(renderer, font, "UP/DOWN to select, ENTER to apply, ESC to go back",
                         winW / 2, winH - 40, {120, 120, 140, 255});

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    return false;
}

// =============================================================================
// Main Menu
// =============================================================================
MainMenuChoice showMainMenu(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return MENU_QUIT;

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return MENU_QUIT;
    }
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 30);
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return MENU_QUIT;
    }
    TTF_Font* titleFont = TTF_OpenFont("assets/fonts/LEMONMILK-Medium.otf", 48);

    std::vector<std::string> options = {"Play vs AI", "PvP (2 Players)", "Tutorial", "Settings", "Quit"};
    int selected = 0;
    bool running = true;
    SDL_Event e;

    float animTimer = 0.0f;
    Uint32 lastTicks = SDL_GetTicks();

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;
        animTimer += dt;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                selected = (int)options.size() - 1; // Quit
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selected = (selected - 1 + (int)options.size()) % options.size();
                        break;
                    case SDLK_DOWN:
                        selected = (selected + 1) % options.size();
                        break;
                    case SDLK_ESCAPE:
                        selected = (int)options.size() - 1;
                        running = false;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        running = false;
                        break;
                    default: break;
                }
            }
        }

        int w = app.getWidth();
        int h = app.getHeight();

        // Dark background with subtle gradient effect
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        // Animated background lines
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 6; ++i) {
            int lineY = (int)(h * (i / 6.0f) + std::sin(animTimer + i * 0.5f) * 20);
            SDL_SetRenderDrawColor(renderer, 30, 30, 80, 40);
            SDL_RenderDrawLine(renderer, 0, lineY, w, lineY);
        }

        // Title
        if (titleFont) {
            // Title shadow
            drawTextCentered(renderer, titleFont, "SIGMA STRIKERS",
                             w / 2 + 2, 82, {0, 0, 0, 200});
            // Title
            int pulseAlpha = 200 + (int)(55 * std::sin(animTimer * 2.0f));
            drawTextCentered(renderer, titleFont, "SIGMA STRIKERS",
                             w / 2, 80, {255, 215, 0, (Uint8)pulseAlpha});
        }

        // Subtitle
        drawTextCentered(renderer, font, "Hockey Action Game",
                         w / 2, 130, {150, 150, 180, 255});

        // Menu items
        int itemH = 60;
        int spacing = 14;
        int totalH = (int)options.size() * itemH + ((int)options.size() - 1) * spacing;
        int startY = h / 2 - totalH / 2 + 40;

        for (size_t i = 0; i < options.size(); ++i) {
            int itemW = 300;
            int x = w / 2 - itemW / 2;
            int y = startY + (int)i * (itemH + spacing);

            // Slide animation for selected item
            if ((int)i == selected) {
                x -= 5;
                itemW += 10;
            }

            SDL_Rect rect = {x, y, itemW, itemH};

            if ((int)i == selected) {
                // Selected item - bright accent
                SDL_SetRenderDrawColor(renderer, 50, 70, 160, 255);
                SDL_RenderFillRect(renderer, &rect);

                // Glow border
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
                SDL_Rect outer = {rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2};
                SDL_SetRenderDrawColor(renderer, 80, 120, 220, 150);
                SDL_RenderDrawRect(renderer, &outer);

                // Arrow indicator
                drawTextCentered(renderer, font, "> " + options[i] + " <",
                                 w / 2, y + itemH / 2, {255, 255, 255, 255});
            } else {
                SDL_SetRenderDrawColor(renderer, 30, 30, 50, 200);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
                SDL_RenderDrawRect(renderer, &rect);

                drawText(renderer, font, options[i], rect,
                         SDL_Color{180, 180, 200, 255});
            }
        }

        // Bottom instructions
        drawTextCentered(renderer, font, "Arrow Keys + Enter to select",
                         w / 2, h - 30, {100, 100, 130, 255});

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (titleFont && titleFont != font) TTF_CloseFont(titleFont);
    TTF_CloseFont(font);

    // Map selected index to MainMenuChoice
    switch (selected) {
        case 0: return MENU_PLAY;
        case 1: return MENU_PVP;
        case 2: return MENU_TUTORIAL;
        case 3: return MENU_SETTINGS;
        default: return MENU_QUIT;
    }
}

// =============================================================================
// Tutorial
// =============================================================================
void showTutorial(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return;
    if (TTF_Init() == -1) return;
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 22);
    if (!font) {
        TTF_Quit();
        return;
    }
    TTF_Font* titleFont = TTF_OpenFont("assets/fonts/LEMONMILK-Medium.otf", 36);

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) running = false;
        }

        int w = app.getWidth();
        int h = app.getHeight();
        SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
        SDL_RenderClear(renderer);

        if (titleFont) {
            drawTextCentered(renderer, titleFont, "HOW TO PLAY", w / 2, 50, {255, 215, 0, 255});
        }

        std::vector<std::string> lines = {
            "",
            "=== TEAM 1 (Blue - Left Side) ===",
            "  W/A/S/D  -  Move active player",
            "  E        -  Swap between your 2 players",
            "",
            "=== TEAM 2 (Red - Right Side) ===",
            "  Arrow Keys  -  Move active player",
            "  Right Shift -  Swap between your 2 players",
            "",
            "=== GAMEPLAY ===",
            "  Push the puck into the opponent's goal to score!",
            "  Your inactive player is controlled by AI.",
            "  The game ends when the timer runs out.",
            "",
            "  ESC  -  Quit / Return to menu",
            "",
            "Press any key to go back..."
        };

        int y = 100;
        for (auto &ln : lines) {
            if (ln.empty()) { y += 15; continue; }

            SDL_Color col = {200, 200, 220, 255};
            if (ln.find("===") != std::string::npos) {
                col = {100, 180, 255, 255};
                if (ln.find("TEAM 2") != std::string::npos) col = {255, 120, 120, 255};
                if (ln.find("GAMEPLAY") != std::string::npos) col = {255, 215, 0, 255};
            }

            SDL_Rect r = {40, y, w - 80, 28};
            drawText(renderer, font, ln, r, col);
            y += 30;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (titleFont) TTF_CloseFont(titleFont);
    TTF_CloseFont(font);
}

// =============================================================================
// Settings Menu
// =============================================================================
void showSettingsMenu(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return;

    if (TTF_Init() == -1) return;
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
    if (!font) return;
    TTF_Font* titleFont = TTF_OpenFont("assets/fonts/LEMONMILK-Medium.otf", 36);

    // Settings options
    struct SettingItem {
        std::string label;
        std::vector<std::string> values;
        int currentIndex;
    };

    std::vector<SettingItem> settings = {
        {"Match Duration", {"1 min", "2 min", "3 min", "5 min"}, 1},
        {"Resolution", {"800x600", "1024x768", "1280x720", "1366x768", "1920x1080"}, 1},
        {"Back", {}, 0}
    };

    // Sync current duration
    int durSec = gSettings.matchDuration;
    if (durSec == 60) settings[0].currentIndex = 0;
    else if (durSec == 120) settings[0].currentIndex = 1;
    else if (durSec == 180) settings[0].currentIndex = 2;
    else settings[0].currentIndex = 3;

    int selected = 0;
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selected = (selected - 1 + (int)settings.size()) % settings.size();
                        break;
                    case SDLK_DOWN:
                        selected = (selected + 1) % settings.size();
                        break;
                    case SDLK_LEFT:
                        if (!settings[selected].values.empty()) {
                            settings[selected].currentIndex =
                                (settings[selected].currentIndex - 1 + (int)settings[selected].values.size()) %
                                settings[selected].values.size();
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!settings[selected].values.empty()) {
                            settings[selected].currentIndex =
                                (settings[selected].currentIndex + 1) % settings[selected].values.size();
                        }
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        if (settings[selected].label == "Back") {
                            running = false;
                        }
                        // Apply resolution if selected
                        if (settings[selected].label == "Resolution") {
                            std::vector<std::pair<int,int>> resOpts = {
                                {800,600}, {1024,768}, {1280,720}, {1366,768}, {1920,1080}
                            };
                            auto &res = resOpts[settings[1].currentIndex];
                            app.setResolution(res.first, res.second);
                            // Reopen font after renderer change
                            TTF_CloseFont(font);
                            if (titleFont) TTF_CloseFont(titleFont);
                            font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
                            titleFont = TTF_OpenFont("assets/fonts/LEMONMILK-Medium.otf", 36);
                            if (!font) { running = false; break; }
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default: break;
                }
            }
        }

        // Apply settings changes live
        int durValues[] = {60, 120, 180, 300};
        gSettings.matchDuration = durValues[settings[0].currentIndex];

        int w = app.getWidth();
        int h = app.getHeight();

        SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
        SDL_RenderClear(renderer);

        // Title
        if (titleFont) {
            drawTextCentered(renderer, titleFont, "SETTINGS", w / 2, 60, {255, 215, 0, 255});
        }

        int itemH = 55;
        int spacing = 12;
        int totalH = (int)settings.size() * itemH + ((int)settings.size() - 1) * spacing;
        int startY = h / 2 - totalH / 2;

        for (size_t i = 0; i < settings.size(); ++i) {
            int itemW = 450;
            int x = w / 2 - itemW / 2;
            int y = startY + (int)i * (itemH + spacing);
            SDL_Rect rect = {x, y, itemW, itemH};

            if ((int)i == selected) {
                SDL_SetRenderDrawColor(renderer, 50, 70, 160, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 30, 30, 50, 200);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }

            SDL_Color labelCol = ((int)i == selected) ? SDL_Color{255, 255, 255, 255} : SDL_Color{160, 160, 180, 255};

            if (settings[i].values.empty()) {
                // Simple button (Back)
                drawText(renderer, font, settings[i].label, rect, labelCol);
            } else {
                // Label on left
                SDL_Rect labelRect = {x + 15, y, itemW / 2, itemH};
                // Use left-aligned draw
                SDL_Surface* surf = TTF_RenderText_Blended(font, settings[i].label.c_str(), labelCol);
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    if (tex) {
                        SDL_Rect dst = {x + 15, y + (itemH - surf->h) / 2, surf->w, surf->h};
                        SDL_RenderCopy(renderer, tex, nullptr, &dst);
                        SDL_DestroyTexture(tex);
                    }
                    SDL_FreeSurface(surf);
                }

                // Value on right with arrows
                std::string valText = "< " + settings[i].values[settings[i].currentIndex] + " >";
                SDL_Color valCol = ((int)i == selected) ? SDL_Color{255, 215, 0, 255} : SDL_Color{140, 140, 160, 255};
                SDL_Rect valRect = {x + itemW / 2, y, itemW / 2 - 15, itemH};
                drawText(renderer, font, valText, valRect, valCol);
            }
        }

        // Instructions
        drawTextCentered(renderer, font, "UP/DOWN to select, LEFT/RIGHT to change, ENTER to apply, ESC to go back",
                         w / 2, h - 30, {100, 100, 130, 255});

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (titleFont) TTF_CloseFont(titleFont);
    if (font) TTF_CloseFont(font);
}
