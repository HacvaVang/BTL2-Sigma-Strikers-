#include "../include/Menu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <iostream>

bool showResolutionMenu(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return false;

    // Initialize TTF
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load a font (default system fonts or bundled font)
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return false;
    }

    // Preset resolutions
    std::vector<std::pair<int,int>> options = {
        {800,600}, {1024,768}, {1280,720}, {1366,768}, {1920,1080}
    };
    int selected = 0;
    bool menuRunning = true;
    SDL_Event e;

    // Simple menu loop: draw boxes for each option, highlight selected.
    while (menuRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT: 
                        selected = (selected - 1 + (int)options.size()) % options.size(); 
                        break;
                    case SDLK_RIGHT: 
                        selected = (selected + 1) % (int)options.size(); 
                        break;
                    case SDLK_ESCAPE: 
                        return false;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER: {
                        int w = options[selected].first;
                        int h = options[selected].second;
                        std::cout << "Applying resolution: " << w << "x" << h << std::endl;
                        bool ok = app.setResolution(w,h);
                        return ok;
                    }
                }
            }
        }

        int winW = app.getWidth();
        int winH = app.getHeight();

        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        // Draw menu items as rectangles centered vertically
        int itemH = 60;
        int spacing = 12;
        int totalH = (int)options.size() * itemH + ((int)options.size()-1) * spacing;
        int startY = winH/2 - totalH/2;

        for (size_t i = 0;i < options.size(); ++i) {
            int x = winW/2 - 200/2;
            int y = startY + (int)i * (itemH + spacing);
            SDL_Rect r{ x, y, 200, itemH };
            if ((int)i == selected) {
                SDL_SetRenderDrawColor(renderer, 200, 120, 40, 255);
                SDL_RenderFillRect(renderer, &r);
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
                SDL_RenderFillRect(renderer, &r);
            }
            // Small inner rect as a simple label placeholder
            SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
            SDL_Rect inner{ x+8, y+8, 200-16, itemH-16 };
            SDL_RenderFillRect(renderer, &inner);

            // Render resolution text using SDL_ttf
            std::string resText = std::to_string(options[i].first) + "x" + std::to_string(options[i].second);
            SDL_Color textColor = ((int)i == selected) ? SDL_Color{255, 255, 100, 255} : SDL_Color{200, 200, 200, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, resText.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect{0, 0, textSurface->w, textSurface->h};
                    int textX = x + (200 - textSurface->w) / 2;
                    int textY = y + (itemH - textSurface->h) / 2;
                    textRect.x = textX;
                    textRect.y = textY;
                    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();

    return false;
}

// -----------------------------------------------------------------------------
// Simple main menu and tutorial helpers
// -----------------------------------------------------------------------------

// draw a single text item centered in given rect
static void drawText(SDL_Renderer* renderer, TTF_Font* font,
                     const std::string &text, SDL_Rect dst, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);
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

MainMenuChoice showMainMenu(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return MENU_QUIT;

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return MENU_QUIT;
    }
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 32);
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return MENU_QUIT;
    }

    std::vector<std::string> options = {"Play", "Tutorial", "Settings", "Quit"};
    int selected = 0;
    bool running = true;
    SDL_Event e;

    while (running) {
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
                }
            }
        }

        int w = app.getWidth();
        int h = app.getHeight();
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        int itemH = 70;
        int spacing = 20;
        int totalH = (int)options.size() * itemH + ((int)options.size() - 1) * spacing;
        int startY = h/2 - totalH/2;

        for (size_t i = 0; i < options.size(); ++i) {
            SDL_Rect rect = { w/2 - 220/2, startY + (int)i * (itemH + spacing), 220, itemH };
            SDL_SetRenderDrawColor(renderer,
                (int)i == selected ? 200 : 100,
                (int)i == selected ? 200 : 100,
                (int)i == selected ? 255 : 100,
                255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_Color txtCol = (i == (size_t)selected) ? SDL_Color{20,20,60,255} : SDL_Color{220,220,220,255};
            drawText(renderer, font, options[i], rect, txtCol);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    return static_cast<MainMenuChoice>(selected);
}

void showTutorial(SDLFramework &app) {
    SDL_Renderer* renderer = app.getRenderer();
    if (!renderer) return;
    if (TTF_Init() == -1) return;
    TTF_Font* font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
    if (!font) {
        TTF_Quit();
        return;
    }

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                running = false;
            }
        }

        int w = app.getWidth();
        int h = app.getHeight();
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
        SDL_RenderClear(renderer);

        std::vector<std::string> lines = {
            "Tutorial", 
            "Use arrow keys to move.",
            "Press ESC to quit or return to menu.",
            "Press any key or click to go back."
        };
        int y = 60;
        for (auto &ln : lines) {
            SDL_Rect r = {20, y, w-40, 40};
            drawText(renderer, font, ln, r, SDL_Color{220,220,220,255});
            y += 50;
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();
}

