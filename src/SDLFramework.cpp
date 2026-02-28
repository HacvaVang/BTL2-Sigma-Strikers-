#include "../include/SDLFramework.h"
#include <SDL_image.h>
#include <iostream>

bool scaleTexturePercentage(SDL_Texture* &tex, SDL_Renderer* renderer, float scale) {
    if (!tex) return false;
    int w, h;
    if (SDL_QueryTexture(tex, nullptr, nullptr, &w, &h) != 0) {
        return false;
    }
    int newW = static_cast<int>(w * scale);
    int newH = static_cast<int>(h * scale);
    std::cout << "Scaling texture from " << w << "x" << h << " to " << newW << "x" << newH << std::endl;
    SDL_Texture* scaledTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                               SDL_TEXTUREACCESS_TARGET, newW, newH);
    if (!scaledTex) {
        return false;
    }
    // render original texture onto the new one with scaling
    SDL_SetRenderTarget(renderer, scaledTex);
    SDL_RenderCopy(renderer, tex, nullptr, nullptr);
    SDL_SetRenderTarget(renderer, nullptr);
    // replace original texture with the scaled one
    SDL_DestroyTexture(tex);
    tex = scaledTex;
    return true;
}


SDLFramework::SDLFramework()
: window(nullptr), renderer(nullptr), fieldTexture(nullptr),
  ballTexture(nullptr), playerTexture(nullptr), running(false), width(800), height(600) {}

SDLFramework::~SDLFramework() {
    if (playerTexture) SDL_DestroyTexture(playerTexture);
    if (fieldTexture) SDL_DestroyTexture(fieldTexture);
    if (ballTexture) SDL_DestroyTexture(ballTexture);
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

    // load our two specific sprites, allow failure
    fieldTexture = IMG_LoadTexture(renderer, "assets/sprite/football_field.png");
    if (!fieldTexture) {
        std::cerr << "Failed to load field texture: " << IMG_GetError() << std::endl;
    }
    ballTexture = IMG_LoadTexture(renderer, "assets/sprite/ball.png");
    if (!ballTexture) {
        std::cerr << "Failed to load ball texture: " << IMG_GetError() << std::endl;
    }
    // load player sprite (optional)
    playerTexture = IMG_LoadTexture(renderer, "assets/sprite/player.png");
    scaleTexturePercentage(playerTexture, renderer, 0.08f); // scale player sprite to 50% for better fit
    if (!playerTexture) {
        std::cerr << "Failed to load player texture: " << IMG_GetError() << std::endl;
    }

    running = true;
    return true;
}

bool SDLFramework::setResolution(int w, int h) {
    // destroy renderer and window, then recreate with new size
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
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

    // reload field and ball textures
    if (fieldTexture) { SDL_DestroyTexture(fieldTexture); fieldTexture = nullptr; }
    if (ballTexture) { SDL_DestroyTexture(ballTexture); ballTexture = nullptr; }
    fieldTexture = IMG_LoadTexture(renderer, "assets/sprite/football_field.png");
    if (!fieldTexture) {
        std::cerr << "Failed to reload field texture: " << IMG_GetError() << std::endl;
    }
    ballTexture = IMG_LoadTexture(renderer, "assets/sprite/ball.png");
    if (!ballTexture) {
        std::cerr << "Failed to reload ball texture: " << IMG_GetError() << std::endl;
    }
    playerTexture = IMG_LoadTexture(renderer, "assets/sprite/player.png");
    scaleTexturePercentage(playerTexture, renderer, 0.08f); // scale player sprite to 50% for better fit

    if (!playerTexture) {
        std::cerr << "Failed to load player texture: " << IMG_GetError() << std::endl;
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

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}
