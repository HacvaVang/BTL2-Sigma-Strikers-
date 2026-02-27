#include "../include/AIAgent.h"
#include <cmath>
#include <algorithm>

AIAgent::AIAgent(float reaction) : reactionSpeed(reaction) {}

void AIAgent::update(float dt, Player &aiPlayer, const Ball &ball,
                     const Field &field, bool isLeftSide) {
    // Determine defensive home position based on which side the team plays
    float homeX, homeY;
    if (isLeftSide) {
        homeX = field.getWidth() * 0.25f;  // left quarter
    } else {
        homeX = field.getWidth() * 0.75f;  // right quarter
    }
    homeY = field.getHeight() / 2.0f; // center vertically

    // Decide behavior based on ball distance
    Vector toBall = ball.pos - aiPlayer.pos;
    float distToBall = toBall.length();

    Vector target;

    // If ball is on our side or close, chase it
    bool ballOnOurSide = isLeftSide ? (ball.pos.x < field.getWidth() / 2.0f)
                                     : (ball.pos.x > field.getWidth() / 2.0f);

    if (ballOnOurSide || distToBall < 8.0f) {
        // Chase the ball - try to get behind it relative to our goal
        // so we push it toward the opponent's goal
        if (isLeftSide) {
            // We want to be to the left of the ball, push it right
            target = Vector(ball.pos.x - 1.5f, ball.pos.y);
        } else {
            // We want to be to the right of the ball, push it left
            target = Vector(ball.pos.x + 1.5f, ball.pos.y);
        }

        // If we're already behind the ball, just go straight at it
        bool behindBall = isLeftSide ? (aiPlayer.pos.x < ball.pos.x - 0.5f)
                                      : (aiPlayer.pos.x > ball.pos.x + 0.5f);
        if (behindBall || distToBall < 2.0f) {
            target = ball.pos;
        }
    } else {
        // Ball is far and on opponent's side - hold defensive position
        // but track ball's Y position somewhat
        target = Vector(homeX, ball.pos.y * 0.5f + homeY * 0.5f);
    }

    // Move toward target
    Vector toTarget = target - aiPlayer.pos;
    float distToTarget = toTarget.length();

    if (distToTarget > 0.3f) {
        Vector dir = toTarget.normalized();
        float moveSpeed = aiPlayer.speed * reactionSpeed;

        // Slow down when approaching target
        if (distToTarget < 2.0f) {
            moveSpeed *= (distToTarget / 2.0f);
        }

        aiPlayer.pos += dir * moveSpeed * dt;
    }

    // Clamp to field bounds
    aiPlayer.pos.x = std::max(aiPlayer.radius, std::min(aiPlayer.pos.x, field.getWidth() - aiPlayer.radius));
    aiPlayer.pos.y = std::max(aiPlayer.radius, std::min(aiPlayer.pos.y, field.getHeight() - aiPlayer.radius));
}
