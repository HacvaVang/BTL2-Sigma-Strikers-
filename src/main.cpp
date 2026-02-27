#include "../include/SDLFramework.h"
#include "../include/Field.h"
#include "../include/Ball.h"
#include <iostream>
#include <SDL_ttf.h>
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

    // initialize TTF and open a font for on‑screen UI
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    }
    TTF_Font *font = TTF_OpenFont("assets/fonts/mohave-semibold.otf", 24);
    if (!font) {
        std::cerr << "Could not load font for gameplay UI: " << TTF_GetError() << std::endl;
    }

    // two teams: one controlled with WASD/E, the other with arrow keys + right-shift
    Team team1(
        Vector(5.0f, 5.0f), Vector(5.0f, 15.0f),
        SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D,
        SDL_SCANCODE_E,
        SDL_Color{200,50,50,255}            // red-ish for team1
    );
    Team team2(
        Vector(field.getWidth() - 5.0f, 5.0f),
        Vector(field.getWidth() - 5.0f, 15.0f),
        SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
        SDL_SCANCODE_RSHIFT,
        SDL_Color{50,50,200,255}            // blue-ish for team2
    );

    // keep track of elapsed game time (seconds)
    float gameTime = 0.0f;

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();
    SDL_Event e;

    // manual game loop so we can update/render our own objects
    // helper to draw simple text at given coordinates
    auto renderText = [&](SDL_Renderer *renderer, TTF_Font *font,
                          const std::string &text, int x, int y,
                          SDL_Color color = SDL_Color{255,255,255,255}) {
        SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surf) return;
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
        if (tex) {
            SDL_Rect dst{ x, y, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_FreeSurface(surf);
    };

    // start the main loop
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
            // pass event to teams for swapping
            team1.handleEvent(e);
            team2.handleEvent(e);
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;
        gameTime += dt;

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        team1.update(dt, keys, &field);
        team2.update(dt, keys, &field);

        // simple scoring logic: ball leaving left/right of field
        if (ball.pos.x < 0) {
            team2.score++;
            ball.pos = Vector(field.getWidth()/2, field.getHeight()/2);
            ball.vel.x = -ball.vel.x; // send it back
        } else if (ball.pos.x > field.getWidth()) {
            team1.score++;
            ball.pos = Vector(field.getWidth()/2, field.getHeight()/2);
            ball.vel.x = -ball.vel.x;
        }

        ball.update(dt);
        field.handleCollision(ball);

        SDL_Renderer *renderer = app.getRenderer();
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        field.render(renderer, app.getWidth(), app.getHeight(),
                      app.getFieldTexture());
        // render both teams
        team1.render(renderer, field, app.getWidth(), app.getHeight(),
                     app.getPlayerTexture());
        team2.render(renderer, field, app.getWidth(), app.getHeight(),
                     app.getPlayerTexture());
        ball.render(renderer, field, app.getWidth(), app.getHeight(),
                    app.getBallTexture());

        // draw UI: scores and elapsed time
        int w = app.getWidth();
        int h = app.getHeight();
        SDL_Color uiCol{255,255,255,255};
        if (font) {
            renderText(renderer, font, "Team1: " + std::to_string(team1.score), 10, 10, uiCol);
            renderText(renderer, font, "Team2: " + std::to_string(team2.score), 10, 40, uiCol);
            int minutes = static_cast<int>(gameTime) / 60;
            int seconds = static_cast<int>(gameTime) % 60;
            char buf[32];
            sprintf(buf, "%02d:%02d", minutes, seconds);
            renderText(renderer, font, buf, w - 100, 10, uiCol);
        }

        SDL_RenderPresent(renderer);
            SDL_Delay(16);
    }

    // cleanup TTF resources
    if (font) TTF_CloseFont(font);
    TTF_Quit();

    return 0;
}
