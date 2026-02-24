#include "../include/SDLFramework.h"
#include "../include/Field.h"
#include "../include/Ball.h"
#include <iostream>
#include "../include/Menu.h"

int main(int argc, char** argv) {
    SDLFramework app;
    if (!app.init("Sigma Strikers - SDL Framework", 1024, 768)) {
        std::cerr << "Failed to initialize SDL framework" << std::endl;
        return 1;
    }

    // Show resolution menu before entering main loop (use arrow keys + Enter)
    showResolutionMenu(app);

    // create field (40m x 20m by default) and a white puck that will bounce
    Field field(40.0f, 20.0f);
    Ball ball(field.getWidth() / 2.0f, field.getHeight() / 2.0f,
              10.0f, 8.0f, 0.5f);

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();
    SDL_Event e;

    // manual game loop so we can update/render our own objects
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;

        ball.update(dt);
        field.handleCollision(ball);

        SDL_SetRenderDrawColor(app.getRenderer(), 20, 20, 40, 255);
        SDL_RenderClear(app.getRenderer());

        field.render(app.getRenderer(), app.getWidth(), app.getHeight(),
                      app.getFieldTexture());
        ball.render(app.getRenderer(), field, app.getWidth(), app.getHeight(),
                    app.getBallTexture());

        SDL_RenderPresent(app.getRenderer());
        SDL_Delay(16);
    }
    
    return 0;
}
