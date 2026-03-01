#include "../include/SDLFramework.h"
#include "../include/Field.h"
#include "../include/Obstacle.h"
#include "../include/Ball.h"
#include "../include/Team.h"
#include "../include/Menu.h"
#include "../include/HUD.h"
#include "../include/AIAgent.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================================
// Game state
// ============================================================================
enum GameMode {
    MODE_VS_AI,   // Player vs AI (single player controls Team 1)
    MODE_PVP      // Player vs Player (local 2-player)
};

// Reset positions after a goal
static void resetAfterGoal(Team &team1, Team &team2, Ball &ball, const Field &field) {
    // Team 1 on left side
    team1.resetPositions(
        Vector(field.getWidth() * 0.2f, field.getHeight() * 0.35f),
        Vector(field.getWidth() * 0.2f, field.getHeight() * 0.65f)
    );
    // Team 2 on right side
    team2.resetPositions(
        Vector(field.getWidth() * 0.8f, field.getHeight() * 0.35f),
        Vector(field.getWidth() * 0.8f, field.getHeight() * 0.65f)
    );
    // Ball to center
    ball.reset(Vector(field.getWidth() / 2.0f, field.getHeight() / 2.0f), Vector(0, 0));
}

// ============================================================================
// Main
// ============================================================================
int main(int argc, char** argv) {
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif
    SDL_Log("=== Sigma Strikers starting ===");

    SDLFramework app;
    if (!app.init("Sigma Strikers", 1280, 720)) {
        SDL_Log("Failed to initialize SDL framework");
        return 1;
    }
    SDL_Log("SDL Framework initialized successfully");

    // ---- Main Menu Loop ----
    GameMode gameMode = MODE_VS_AI;
    bool wantToPlay = false;

    while (true) {
        MainMenuChoice choice = showMainMenu(app);
        if (choice == MENU_PLAY) {
            gameMode = MODE_VS_AI;
            wantToPlay = true;
            break;
        } else if (choice == MENU_PVP) {
            gameMode = MODE_PVP;
            wantToPlay = true;
            break;
        } else if (choice == MENU_TUTORIAL) {
            showTutorial(app);
            continue;
        } else if (choice == MENU_SETTINGS) {
            showSettingsMenu(app);
            continue;
        } else {
            break; // quit
        }
    }

    if (!wantToPlay) return 0;

    // ---- Initialize Game Objects ----
    Field field(40.0f, 20.0f);
    // place a couple of fixed obstacles on the pitch for testing
    field.addObstacle(Obstacle(Vector(field.getWidth() * 0.5f,
                                      field.getHeight() * 0.5f),
                               4.0f, 4.0f));
    field.addObstacle(Obstacle(Vector(field.getWidth() * 0.2f,
                                      field.getHeight() * 0.2f),
                               1.25f, 3.0f));
    field.addObstacle(Obstacle(Vector(field.getWidth() * 0.8f,
                                      field.getHeight() * 0.8f),
                               1.25f, 3.0f));
    field.addObstacle(Obstacle(Vector(field.getWidth() * 0.75f,
                                      field.getHeight() * 0.25f),
                               4.0f, 1.0f));
    field.addObstacle(Obstacle(Vector(field.getWidth() * 0.25f,
                                      field.getHeight() * 0.75f),
                               4.0f, 1.0f));
    // Ball starts at center, stationary
    Ball ball(
        Vector(field.getWidth() / 2.0f, field.getHeight() / 2.0f),
        Vector(0, 0),
        0.5f
    );

    // Team 1 (Blue, left side) - WASD + E to swap
    KeyBindings kb1 = {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDLK_e};
    Team team1(
        Vector(field.getWidth() * 0.2f, field.getHeight() * 0.35f),
        Vector(field.getWidth() * 0.2f, field.getHeight() * 0.65f),
        kb1
    );

    // Team 2 (Red, right side) - Arrow keys + Right Shift to swap
    KeyBindings kb2 = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDLK_RSHIFT};
    Team team2(
        Vector(field.getWidth() * 0.8f, field.getHeight() * 0.35f),
        Vector(field.getWidth() * 0.8f, field.getHeight() * 0.65f),
        kb2
    );

    // HUD
    HUD hud;
    if (!hud.init("assets/fonts/mohave-semibold.otf", 24)) {
        SDL_Log("Warning: HUD font failed to load");
    }

    // AI Agents
    // ai1 controls Team 1's inactive player (in all modes)
    // ai2 controls Team 2 entirely in VS_AI mode, or just inactive player in PvP
    AIAgent ai1(0.7f);  // Team 1 AI
    AIAgent ai2(0.8f);  // Team 2 AI (slightly faster reaction for full AI team)

    // Game timer
    float matchTime = (float)gSettings.matchDuration;
    float goalMessageTimer = 0.0f;
    std::string goalMessage;
    bool gameOver = false;

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();
    SDL_Event e;

    // ---- Game Loop ----
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;

            if (!gameOver) {
                // Team 1 always player-controlled
                team1.handleEvent(e);

                // Team 2: player-controlled in PvP, AI in vs-AI mode
                if (gameMode == MODE_PVP) {
                    team2.handleEvent(e);
                }
            }

            // R to restart after game over
            if (gameOver && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
                // Reset everything
                team1.score = 0;
                team2.score = 0;
                matchTime = (float)gSettings.matchDuration;
                gameOver = false;
                goalMessageTimer = 0;
                resetAfterGoal(team1, team2, ball, field);
            }
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f; // cap delta time
        lastTicks = now;

        if (!gameOver) {
            // Update timer
            matchTime -= dt;
            if (matchTime <= 0.0f) {
                matchTime = 0.0f;
                gameOver = true;
                if (team1.score > team2.score) {
                    goalMessage = "TEAM 1 WINS!";
                } else if (team2.score > team1.score) {
                    goalMessage = "TEAM 2 WINS!";
                } else {
                    goalMessage = "DRAW!";
                }
                goalMessageTimer = 99999.0f; // show forever until restart
            }

            // Goal message countdown
            if (goalMessageTimer > 0) {
                goalMessageTimer -= dt;
            }

            const Uint8 *keys = SDL_GetKeyboardState(NULL);

            // Update Team 1 (active player is always human-controlled)
            team1.update(dt, keys, &field);
            // AI controls Team 1's inactive player (support)
            ai1.update(dt, team1.getInactivePlayer(), ball, field, true);

            // Update Team 2
            if (gameMode == MODE_PVP) {
                // Human controls active player of Team 2
                team2.update(dt, keys, &field);
                // AI controls Team 2's inactive player
                ai2.update(dt, team2.getInactivePlayer(), ball, field, false);
            } else {
                // Full AI: updateTeam handles both players with Active/Support
                // roles, passing logic, and steering behaviors
                ai2.updateTeam(dt, team2, ball, field, false, team1);

                // Handle passing: when AI decides to pass, apply force to ball
                if (ai2.didJustPass()) {
                    float d1 = (team2.p1.pos - ball.pos).length();
                    float d2 = (team2.p2.pos - ball.pos).length();
                    Player &passer   = (d1 <= d2) ? team2.p1 : team2.p2;
                    Player &receiver = (d1 <= d2) ? team2.p2 : team2.p1;

                    Vector passDir = (receiver.pos - ball.pos).normalized();
                    float passDist = (receiver.pos - ball.pos).length();
                    float passSpeed = std::min(25.0f, std::max(12.0f, passDist * 1.2f));
                    ball.vel = passDir * passSpeed;
                }

                // Handle shooting: when AI decides to shoot, launch ball at goal
                if (ai2.didJustShoot()) {
                    Vector target = ai2.getShotTarget();
                    Vector shotDir = (target - ball.pos).normalized();
                    float shotDist = (target - ball.pos).length();
                    // Shot speed: faster than pass, scales with distance
                    float shotSpeed = std::min(30.0f, std::max(18.0f, shotDist * 1.5f));
                    ball.vel = shotDir * shotSpeed;
                }
            }

            // ---- Player-to-player collision resolution ----
            // Prevents all 4 players from overlapping each other
            resolveAllPlayerCollisions(team1, team2);

            // Update ball physics
            ball.update(dt);

            // ---- Multi-pass collision resolution ----
            // Run 3 iterations so that if a player pushes the ball into a wall,
            // the wall pushes it back, and then player collision can fix it again.
            // This prevents the ball getting permanently stuck between players & walls.
            int goalResult = 0;
            for (int iter = 0; iter < 3; ++iter) {
                // Ball-player collisions (all 4 players)
                ball.handlePlayerCollision(team1.p1, team1.p1.radius);
                ball.handlePlayerCollision(team1.p2, team1.p2.radius);
                ball.handlePlayerCollision(team2.p1, team2.p1.radius);
                ball.handlePlayerCollision(team2.p2, team2.p2.radius);

                // player-obstacle resolution
                field.handlePlayerCollision(team1.p1);
                field.handlePlayerCollision(team1.p2);
                field.handlePlayerCollision(team2.p1);
                field.handlePlayerCollision(team2.p2);

                // Ball-wall/obstacle collisions & goal detection
                int res = field.handleCollision(ball);
                if (res != 0) goalResult = res;
            }
            if (goalResult == 1) {
                // Left goal - Team 2 scores
                team2.score++;
                goalMessage = "TEAM 2 SCORES!";
                goalMessageTimer = 2.0f;
                resetAfterGoal(team1, team2, ball, field);
            } else if (goalResult == 2) {
                // Right goal - Team 1 scores
                team1.score++;
                goalMessage = "TEAM 1 SCORES!";
                goalMessageTimer = 2.0f;
                resetAfterGoal(team1, team2, ball, field);
            }

        }

        // ---- Render ----
        SDL_SetRenderDrawColor(app.getRenderer(), 20, 20, 40, 255);
        SDL_RenderClear(app.getRenderer());

        // Field
        field.render(app.getRenderer(), app.getWidth(), app.getHeight(),
                     app.getFieldTexture());

        // Teams with their colors
        SDL_Color team1Active   = {80, 140, 255, 255};   // bright blue
        SDL_Color team1Inactive = {40, 70, 100, 180};    // dim blue
        SDL_Color team2Inactive = {100, 70, 40, 200};    // dim red
        SDL_Color team2Active   = gameMode == MODE_VS_AI ? team2Inactive : SDL_Color{255, 100, 100, 255};    // bright red
        

        team1.render(app.getRenderer(), field, app.getWidth(), app.getHeight(),
                     team1Active, team1Inactive, app.getPlayerTexture());
        team2.render(app.getRenderer(), field, app.getWidth(), app.getHeight(),
                     team2Active, team2Inactive, app.getPlayerTexture());

        // Ball
        ball.render(app.getRenderer(), field, app.getWidth(), app.getHeight(),
                    app.getBallTexture());

        // HUD (scores + timer)
        hud.render(app.getRenderer(), app.getWidth(), app.getHeight(),
                   team1.score, team2.score, matchTime);

        // Goal / Game Over message
        if (goalMessageTimer > 0) {
            hud.renderMessage(app.getRenderer(), app.getWidth(), app.getHeight(),
                              goalMessage);
            if (gameOver) {
                // Also show restart instruction
                SDL_Color white = {200, 200, 200, 255};
                // Small text below the message
                SDL_SetRenderDrawBlendMode(app.getRenderer(), SDL_BLENDMODE_BLEND);
                // We'll use HUD's renderMessage for now, it shows the main message
                // The "Press R to restart" is handled via a secondary call
            }
        }

        // If game over, show restart text
        if (gameOver) {
            hud.renderMessage(app.getRenderer(), app.getWidth(), app.getHeight(),
                              goalMessage);
        }

        SDL_RenderPresent(app.getRenderer());
        SDL_Delay(16);
    }

    return 0;
}