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

    // existing update kept for backwards compatibility (uses WASD)
    void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
    // low‑level movement function used by teams with custom controls
    void move(float dx, float dy, float dt, const Field *bounds = nullptr);
    // rendering: color is only used when tex is null.  If tex is provided
    // it will be drawn centered on the player's position and tinted with
    // SDL_SetTextureColorMod.
    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH,
                SDL_Color color, SDL_Texture *tex = nullptr) const;
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
    SDL_Color teamColor;   // base colour for rendering

    // control scheme for the whole team
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode swapKey;

    Team(const Vector &start1 = Vector(), const Vector &start2 = Vector(),
         SDL_Scancode up_ = SDL_SCANCODE_W, SDL_Scancode down_ = SDL_SCANCODE_S,
         SDL_Scancode left_ = SDL_SCANCODE_A, SDL_Scancode right_ = SDL_SCANCODE_D,
         SDL_Scancode swap_ = SDL_SCANCODE_E,
         SDL_Color color_ = SDL_Color{200,50,50,255})
        : p1(start1), p2(start2), activeIndex(0), score(0),
          teamColor(color_),
          up(up_), down(down_), left(left_), right(right_), swapKey(swap_) {}

    // call from main loop to update the currently active player
    void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
    void handleEvent(const SDL_Event &e);    // swap on key defined by swapKey
    // rendering: optional texture; if null use simple circle
    void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH,
                SDL_Texture *playerTex = nullptr) const;

    Player &getActivePlayer();
    const Player &getActivePlayer() const;
    void swapActive();
};

#endif // TEAMS_H

// // A team consists of exactly two players and a numeric score.  One player
// // is "active" at a time; pressing the swap key switches control to the
// // other member.  Scoring and other game logic may be added externally.
// class Team {
// public:
//     Player p1;
//     Player p2;
//     int activeIndex; // 0 or 1
//     int score;

//     Team(const Vector &start1 = Vector(), const Vector &start2 = Vector())
//         : p1(start1), p2(start2), activeIndex(0), score(0) {}

//     // call from main loop to update the currently active player
//     void update(float dt, const Uint8 *keyState, const Field *bounds = nullptr);
//     void handleEvent(const SDL_Event &e);    // swap on 'E' key
//     void render(SDL_Renderer *renderer, const Field &field, int screenW, int screenH) const;

//     Player &getActivePlayer();
//     const Player &getActivePlayer() const;
//     void swapActive();
// };
