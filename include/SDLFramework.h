#ifndef SDLFRAMEWORK_H
#define SDLFRAMEWORK_H

#include <SDL.h>
#include <string>


class SDLFramework {
public:
    SDLFramework();
    ~SDLFramework();
    bool init(const std::string &title, int w, int h);
    void run();
    // Change resolution at runtime. Returns true on success.
    bool setResolution(int w, int h);

    // Accessors for renderer/window for simple UI drawing
    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();
    int getWidth() const;
    int getHeight() const;

    SDL_Texture* getFieldTexture() const { return fieldTexture; }
    SDL_Texture* getBallTexture() const { return ballTexture; }
    SDL_Texture* getPlayerTexture() const { return playerTexture; }
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* fieldTexture;
    SDL_Texture* ballTexture;
    SDL_Texture* playerTexture;    // new player sprite slot
    bool running;
    int width;
    int height;
};




#endif // SDLFRAMEWORK_H
