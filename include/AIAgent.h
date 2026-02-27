#pragma once

#include "Vector.h"
#include "Team.h"
#include "Ball.h"
#include "Field.h"

// Simple AI agent that controls the inactive (non-player-controlled) member
// of a team.  The AI chases the ball when it is nearby, or moves toward a
// defensive position when the ball is far away.
class AIAgent {
public:
    AIAgent(float reactionSpeed = 0.7f);

    // Update the inactive player of each team to act autonomously.
    // For each team, the inactive player moves according to the AI logic.
    void update(float dt, Player &aiPlayer, const Ball &ball, const Field &field, bool isLeftSide);

private:
    float reactionSpeed; // 0.0 (slow) to 1.0 (instant) reaction multiplier
};
