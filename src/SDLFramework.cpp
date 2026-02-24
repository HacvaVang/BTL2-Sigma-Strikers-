#include "../include/SDLFramework.h"
#include <SDL_image.h>
#include <iostream>

SDLFramework::SDLFramework()
: window(nullptr), renderer(nullptr), sprite(nullptr), running(false), width(800), height(600) {}

SDLFramework::~SDLFramework() {
    if (sprite) SDL_DestroyTexture(sprite);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

bool SDLFramework::init(const std::string &title, int w, int h) {
    width = w; height = h;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        // continue; not fatal for simple demo
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Try to load sprite if present
    sprite = IMG_LoadTexture(renderer, "assets/sprite.png");
    if (!sprite) {
        // not fatal; we'll render a simple rectangle instead
        std::cerr << "No sprite loaded (assets/sprite.png): " << IMG_GetError() << std::endl;
    }

    running = true;
    return true;
}

bool SDLFramework::setResolution(int w, int h) {
    // destroy renderer and window, then recreate with new size
    if (sprite) {
        SDL_DestroyTexture(sprite);
        sprite = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    width = w; height = h;
    window = SDL_CreateWindow("Sigma Strikers - SDL Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "CreateWindow Error (setResolution): " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "CreateRenderer Error (setResolution): " << SDL_GetError() << std::endl;
        return false;
    }

    // reload sprite if available
    sprite = IMG_LoadTexture(renderer, "assets/sprite.png");
    if (!sprite) {
        std::cerr << "No sprite loaded after setResolution (assets/sprite.png): " << IMG_GetError() << std::endl;
    }

    return true;
}

SDL_Renderer* SDLFramework::getRenderer() { return renderer; }
SDL_Window* SDLFramework::getWindow() { return window; }
int SDLFramework::getWidth() const { return width; }
int SDLFramework::getHeight() const { return height; }

void SDLFramework::run() {
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        if (sprite) {
            SDL_Rect dst{ width/2 - 64, height/2 - 64, 128, 128 };
            SDL_RenderCopy(renderer, sprite, nullptr, &dst);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);
            SDL_Rect r{ width/2 - 50, height/2 - 50, 100, 100 };
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}
