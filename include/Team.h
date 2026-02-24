#ifndef TEAMS_H
#define TEAMS_H

#include "SDLFramework.h"
#include "Vector.h"
#include "Field.h"

// ---------------------------------------------------------------------------
// Player & Team classes
// ---------------------------------------------------------------------------

// Represents a controllable player on the field.  Movement is driven by
// WASD according to the keyboard state provided in update().  Units are
// metres for position.  The player may optionally be constrained by a
// Field reference.
class Player {
public:
    Vector pos;      // world coordinates (metres)
    float speed;     // metres per second

    Player(const Vector &start = Vector(), float spd = 20.0f)
        : pos(start), speed(spd) {}

    void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH,
                SDL_Color color) const;
};

// A team consists of exactly two players and a numeric score.  One player
// is "active" at a time; pressing the swap key switches control to the
// other member.  Scoring and other game logic may be added externally.
class Team {
public:
    Player p1;
    Player p2;
    int activeIndex; // 0 or 1
    int score;

    Team(const Vector &start1 = Vector(), const Vector &start2 = Vector())
        : p1(start1), p2(start2), activeIndex(0), score(0) {}

    // call from main loop to update the currently active player
    void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
    void handleEvent(const SDL_Event &e);    // swap on 'E' key
    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH) const;

    Player &getActivePlayer();
    const Player &getActivePlayer() const;
    void swapActive();
};

#endif // TEAMS_H