#ifndef TEAMS_H
#define TEAMS_H

#include "SDLFramework.h"
#include "Vector.h"
#include "Field.h"
#include "Ball.h"

// ---------------------------------------------------------------------------
// Player & Team classes
// ---------------------------------------------------------------------------

// Key binding structure for different control schemes
struct KeyBindings {
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Keycode swap; // this is SDLK_ not scancode
};

// Represents a controllable player on the field.
class Player {
public:
    Vector pos;      // world coordinates (metres)
    float speed;     // metres per second
    float radius;    // collision radius in metres

    Player(const Vector &start = Vector(), float spd = 20.0f, float rad = 0.8f)
        : pos(start), speed(spd), radius(rad) {}

    // Update with specific key bindings (for PvP support)
    void update(float dt, const Uint8 *keyState, const KeyBindings &keys,
                const Field *bounds = nullptr);

    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH,
                SDL_Color color) const;
};

// A team consists of exactly two players and a numeric score.  One player
// is "active" at a time; pressing the swap key switches control to the
// other member.
class Team {
public:
    Player p1;
    Player p2;
    int activeIndex; // 0 or 1
    int score;
    KeyBindings keys;

    Team(const Vector &start1 = Vector(), const Vector &start2 = Vector(),
         KeyBindings kb = {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDLK_e})
        : p1(start1), p2(start2), activeIndex(0), score(0), keys(kb) {}

    // call from main loop to update the currently active player
    void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
    void handleEvent(const SDL_Event &e);
    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH,
                SDL_Color activeColor, SDL_Color inactiveColor) const;

    Player &getActivePlayer();
    const Player &getActivePlayer() const;
    Player &getInactivePlayer();
    const Player &getInactivePlayer() const;
    void swapActive();

    // Reset both players to their starting positions
    void resetPositions(const Vector &start1, const Vector &start2);
};

#endif // TEAMS_H