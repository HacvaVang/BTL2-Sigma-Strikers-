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
