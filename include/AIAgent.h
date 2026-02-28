#pragma once

#include "Vector.h"
#include "Team.h"
#include "Ball.h"
#include "Field.h"

// ============================================================================
// AI Agent with Active/Support role system, passing logic, and steering behaviors.
//
// Active Player : Bot closest to ball or holding ball -> chases/dribbles/passes.
// Support Player: Other bot -> finds open space for receiving passes.
// ============================================================================

// Possible states for an AI-controlled player
enum class AIState {
    // Active states
    CHASE_BALL,     // Move toward the ball
    DRIBBLE,        // Has possession, dribble toward goal or teammate
    PASS,           // Executing a pass to teammate
    SHOOT,          // Take a shot on goal

    // Support states
    FIND_SPACE,     // Move to an open position for receiving
    WAIT_RECEIVE,   // At position, facing ball, ready to receive

    // Common
    DEFEND          // Fall back to defensive position
};

class AIAgent {
public:
    AIAgent(float reaction = 0.8f);

    // Main update: controls BOTH players in a team using Active/Support roles.
    // Call this ONCE per team per frame (it handles role assignment internally).
    void updateTeam(float dt, Team &team, const Ball &ball, const Field &field,
                    bool isLeftSide, const Team &opponentTeam);

    // Legacy single-player update (kept for backward compatibility)
    void update(float dt, Player &aiPlayer, const Ball &ball,
                const Field &field, bool isLeftSide);

    // ---- Getters for debug / rendering ----
    AIState getActiveState()  const { return activeState; }
    AIState getSupportState() const { return supportState; }
    bool    didJustPass()     const { return justPassed; }
    bool    didJustShoot()    const { return justShot; }
    Vector  getShotTarget()   const { return shotTarget; }

private:
    float reactionSpeed;

    // Internal state tracking
    AIState activeState;
    AIState supportState;
    bool    justPassed;       // flag set for one frame after a pass
    bool    justShot;         // flag set for one frame after a shot
    Vector  shotTarget;       // where the shot is aimed
    float   passCooldown;     // seconds until next pass allowed
    float   shotCooldown;     // seconds until next shot allowed
    float   possessionTimer;  // how long active bot has been near ball

    // ---- Role Assignment ----
    // Returns index (0 or 1) indicating which player should be Active.
    int assignRoles(const Team &team, const Ball &ball) const;

    // ---- Active Player behaviors ----
    void updateActive(float dt, Player &active, Player &support,
                      const Ball &ball, const Field &field,
                      bool isLeftSide, const Team &opponentTeam);

    void chaseBall(float dt, Player &player, const Ball &ball,
                   const Field &field, bool isLeftSide);

    void dribble(float dt, Player &player, const Ball &ball,
                 const Field &field, bool isLeftSide);

    // Performs the pass: applies force to ball velocity.
    void executePass(Ball &ball, const Player &passer, const Player &receiver);

    // ---- Shooting logic ----
    // Calculate the shooting angle subtended by the goal opening from a position.
    // Returns angle in radians.
    float calculateShootingAngle(const Vector &shooterPos, const Field &field,
                                 bool isLeftSide) const;

    // Check if a shot can reach the goal without being blocked by opponents.
    bool isShotClear(const Vector &from, const Vector &target,
                     const Team &opponentTeam, float clearance = 1.5f) const;

    // Find the best point within the goal to shoot at (maximises gap from opponents).
    Vector findBestShotTarget(const Vector &shooterPos, const Field &field,
                              bool isLeftSide, const Team &opponentTeam) const;

    // ---- Support Player behaviors ----
    void updateSupport(float dt, Player &support, const Player &active,
                       const Ball &ball, const Field &field,
                       bool isLeftSide, const Team &opponentTeam);

    // Find the optimal receiving position (open, with clear passing lane).
    Vector findOptimalReceivingPoint(const Player &support, const Player &active,
                                    const Ball &ball, const Field &field,
                                    bool isLeftSide, const Team &opponentTeam) const;

    // ---- Utility helpers ----
    // Checks if the passing lane between two points is clear of opponents.
    bool isPassingLaneClear(const Vector &from, const Vector &to,
                            const Team &opponentTeam, float clearance = 2.0f) const;

    // Steer a player toward a target with arrival slowdown.
    void seekWithArrival(float dt, Player &player, const Vector &target,
                         float slowRadius = 3.0f) const;

    // Clamp player inside field boundaries.
    void clampToField(Player &player, const Field &field) const;

    // Check if player is "possessing" the ball (within contact distance).
    bool hasPossession(const Player &player, const Ball &ball) const;

    // Calculate distance from a point to a line segment (for lane clearance).
    float pointToSegmentDistance(const Vector &point, const Vector &segA,
                                const Vector &segB) const;
};

// ============================================================================
// Player-to-Player collision resolution (call after all movement each frame).
// Resolves overlapping players by pushing them apart along the collision normal.
// ============================================================================
void resolvePlayerCollisions(Player &a, Player &b);

// Convenience: resolve all pairs among 4 players.
void resolveAllPlayerCollisions(Team &team1, Team &team2);
