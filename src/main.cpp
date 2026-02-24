#include "../include/SDLFramework.h"
#include "../include/Field.h"
#include "../include/Ball.h"
#include <iostream>
#include "../include/Menu.h"
#include "../include/Team.h"

int main(int argc, char** argv) {
    SDLFramework app;
    if (!app.init("Sigma Strikers - SDL Framework", 1024, 768)) {
        std::cerr << "Failed to initialize SDL framework" << std::endl;
        return 1;
    }

    // present a main menu until user chooses to play or quit
    bool wantToPlay = false;
    while (true) {
        MainMenuChoice choice = showMainMenu(app);
        if (choice == MENU_PLAY) {
            wantToPlay = true;
            break;
        } else if (choice == MENU_TUTORIAL) {
            showTutorial(app);
            continue; // return to main menu
        } else if (choice == MENU_SETTINGS) {
            showResolutionMenu(app);
            continue;
        } else {
            // quit or closed window
            break;
        }
    }

    if (!wantToPlay) {
        return 0; // exit application
    }

    // create field (40m x 20m by default) and a white puck that will bounce
    Field field(40.0f, 20.0f);
    // start the ball in the centre with an initial velocity
    Ball ball(
        Vector(field.getWidth() / 2.0f, field.getHeight() / 2.0f),
        Vector(10.0f, 8.0f),
        0.5f
    );

    // single team example; two players start on left half of the field
    Team team(Vector(5.0f, 5.0f), Vector(5.0f, 15.0f));

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();
    SDL_Event e;

    // manual game loop so we can update/render our own objects
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
            // pass event to team for swapping
            team.handleEvent(e);
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        team.update(dt, keys, &field);

        ball.update(dt);
        field.handleCollision(ball);

        SDL_SetRenderDrawColor(app.getRenderer(), 20, 20, 40, 255);
        SDL_RenderClear(app.getRenderer());

        field.render(app.getRenderer(), app.getWidth(), app.getHeight(),
                      app.getFieldTexture());
        // render team players on top of field
        team.render(app.getRenderer(), field, app.getWidth(), app.getHeight());
        ball.render(app.getRenderer(), field, app.getWidth(), app.getHeight(),
                    app.getBallTexture());

        SDL_RenderPresent(app.getRenderer());
        SDL_Delay(16);
    }
    
    return 0;
}
