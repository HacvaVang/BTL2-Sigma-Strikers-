#include "../include/AIAgent.h"
#include <cmath>
#include <algorithm>

// ============================================================================
// Construction
// ============================================================================
AIAgent::AIAgent(float reaction)
    : reactionSpeed(reaction),
      activeState(AIState::CHASE_BALL),
      supportState(AIState::FIND_SPACE),
      justPassed(false),
      justShot(false),
      shotTarget(),
      passCooldown(0.0f),
      shotCooldown(0.0f),
      possessionTimer(0.0f) {}

// ============================================================================
// Player-to-Player collision resolution
// ============================================================================
void resolvePlayerCollisions(Player &a, Player &b) {
    Vector diff = b.pos - a.pos;
    float dist  = diff.length();
    float minDist = a.radius + b.radius;

    if (dist < minDist && dist > 0.001f) {
        Vector normal = diff.normalized();
        float overlap = minDist - dist;
        // Push each player half the overlap distance apart
        a.pos -= normal * (overlap * 0.5f);
        b.pos += normal * (overlap * 0.5f);
    }
}

void resolveAllPlayerCollisions(Team &team1, Team &team2) {
    // All unique pairs among the 4 players:
    resolvePlayerCollisions(team1.p1, team1.p2);
    resolvePlayerCollisions(team2.p1, team2.p2);
    resolvePlayerCollisions(team1.p1, team2.p1);
    resolvePlayerCollisions(team1.p1, team2.p2);
    resolvePlayerCollisions(team1.p2, team2.p1);
    resolvePlayerCollisions(team1.p2, team2.p2);
}

// ============================================================================
// Utility helpers
// ============================================================================
static float clampF(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

bool AIAgent::hasPossession(const Player &player, const Ball &ball) const {
    float contactDist = player.radius + ball.radius + 0.3f;
    return (player.pos - ball.pos).length() < contactDist;
}

void AIAgent::clampToField(Player &player, const Field &field) const {
    player.pos.x = clampF(player.pos.x, player.radius,
                           field.getWidth() - player.radius);
    player.pos.y = clampF(player.pos.y, player.radius,
                           field.getHeight() - player.radius);
}

void AIAgent::seekWithArrival(float dt, Player &player, const Vector &target,
                              float slowRadius) const {
    Vector toTarget = target - player.pos;
    float dist = toTarget.length();

    if (dist < 0.2f) return;

    Vector dir = toTarget.normalized();
    float moveSpeed = player.speed * reactionSpeed;

    if (dist < slowRadius) {
        moveSpeed *= (dist / slowRadius);
    }

    player.pos += dir * moveSpeed * dt;
}

float AIAgent::pointToSegmentDistance(const Vector &point,
                                     const Vector &segA,
                                     const Vector &segB) const {
    Vector ab = segB - segA;
    Vector ap = point - segA;
    float t = ab.dot(ap) / std::max(0.0001f, ab.dot(ab));
    t = clampF(t, 0.0f, 1.0f);
    Vector closest = segA + ab * t;
    return (point - closest).length();
}

bool AIAgent::isPassingLaneClear(const Vector &from, const Vector &to,
                                 const Team &opponentTeam,
                                 float clearance) const {
    float d1 = pointToSegmentDistance(opponentTeam.p1.pos, from, to);
    float d2 = pointToSegmentDistance(opponentTeam.p2.pos, from, to);
    return (d1 > clearance && d2 > clearance);
}

// ============================================================================
// Shooting Angle Calculation
// ============================================================================
// Calculates the angle (in radians) that the goal opening subtends from the
// shooter's viewpoint.  A larger angle means a better/easier shot.
//
//   θ = atan2(dy_top, dx) - atan2(dy_bot, dx)
//
// where top/bot are the two goal-post positions.
float AIAgent::calculateShootingAngle(const Vector &shooterPos,
                                      const Field &field,
                                      bool isLeftSide) const {
    // We shoot at the OPPONENT'S goal
    float goalX = isLeftSide ? field.getWidth() : 0.0f;
    float goalTop = field.getGoalTop();
    float goalBot = field.getGoalBottom();

    Vector postTop(goalX, goalTop);
    Vector postBot(goalX, goalBot);

    Vector toTop = postTop - shooterPos;
    Vector toBot = postBot - shooterPos;

    float angleTop = std::atan2(toTop.y, toTop.x);
    float angleBot = std::atan2(toBot.y, toBot.x);

    float angle = std::abs(angleTop - angleBot);
    // Normalize to [0, PI]
    if (angle > 3.14159f) angle = 2.0f * 3.14159f - angle;

    return angle;
}

// Check if a shot line is clear of opponents
bool AIAgent::isShotClear(const Vector &from, const Vector &target,
                          const Team &opponentTeam, float clearance) const {
    float d1 = pointToSegmentDistance(opponentTeam.p1.pos, from, target);
    float d2 = pointToSegmentDistance(opponentTeam.p2.pos, from, target);
    return (d1 > clearance && d2 > clearance);
}

// Find the best shot target within the goal opening
// Samples several points along the goal and picks the one with the best
// "gap" from opponent players.
Vector AIAgent::findBestShotTarget(const Vector &shooterPos,
                                   const Field &field,
                                   bool isLeftSide,
                                   const Team &opponentTeam) const {
    float goalX = isLeftSide ? field.getWidth() : 0.0f;
    float goalTop = field.getGoalTop();
    float goalBot = field.getGoalBottom();

    Vector bestTarget(goalX, field.getHeight() / 2.0f);
    float bestScore = -9999.0f;

    // Sample 9 points across the goal opening
    int samples = 9;
    for (int i = 0; i < samples; ++i) {
        float t = (float)i / (float)(samples - 1);
        float y = goalTop + t * (goalBot - goalTop);
        Vector candidate(goalX, y);

        // Score = min distance of this shot line from opponents
        // Higher = the shot is harder to block
        float d1 = pointToSegmentDistance(opponentTeam.p1.pos, shooterPos, candidate);
        float d2 = pointToSegmentDistance(opponentTeam.p2.pos, shooterPos, candidate);
        float oppClearance = std::min(d1, d2);

        // Prefer center of goal slightly (easier to score)
        float centerY = (goalTop + goalBot) / 2.0f;
        float centerBonus = 1.0f - std::abs(y - centerY) / ((goalBot - goalTop) / 2.0f);
        centerBonus *= 2.0f;

        float score = oppClearance + centerBonus;

        if (score > bestScore) {
            bestScore = score;
            bestTarget = candidate;
        }
    }

    return bestTarget;
}

// ============================================================================
// Role Assignment
// ============================================================================
int AIAgent::assignRoles(const Team &team, const Ball &ball) const {
    float d1 = (team.p1.pos - ball.pos).length();
    float d2 = (team.p2.pos - ball.pos).length();
    return (d1 <= d2) ? 0 : 1;
}

// ============================================================================
// Main team update
// ============================================================================
void AIAgent::updateTeam(float dt, Team &team, const Ball &ball,
                         const Field &field, bool isLeftSide,
                         const Team &opponentTeam) {
    if (passCooldown > 0.0f) passCooldown -= dt;
    if (shotCooldown > 0.0f) shotCooldown -= dt;
    justPassed = false;
    justShot   = false;

    int activeIdx = assignRoles(team, ball);
    Player &active  = (activeIdx == 0) ? team.p1 : team.p2;
    Player &support = (activeIdx == 0) ? team.p2 : team.p1;

    updateActive(dt, active, support, ball, field, isLeftSide, opponentTeam);
    updateSupport(dt, support, active, ball, field, isLeftSide, opponentTeam);

    clampToField(active, field);
    clampToField(support, field);
}

// ============================================================================
// Active Player update  (now includes SHOOT decision)
// ============================================================================
void AIAgent::updateActive(float dt, Player &active, Player &support,
                           const Ball &ball, const Field &field,
                           bool isLeftSide, const Team &opponentTeam) {
    bool possess = hasPossession(active, ball);

    if (possess) {
        possessionTimer += dt;
    } else {
        possessionTimer = 0.0f;
    }

    float goalX = isLeftSide ? field.getWidth() : 0.0f;
    float goalY = field.getHeight() / 2.0f;
    Vector goalCenter(goalX, goalY);

    if (!possess) {
        // ---- CHASE BALL ----
        activeState = AIState::CHASE_BALL;
        chaseBall(dt, active, ball, field, isLeftSide);
    } else {
        // We have the ball — decide: SHOOT, PASS, or DRIBBLE
        float distToGoal = (active.pos - goalCenter).length();

        // 1. Evaluate shooting opportunity
        float shootAngle = calculateShootingAngle(active.pos, field, isLeftSide);
        Vector bestShotTarget = findBestShotTarget(active.pos, field, isLeftSide, opponentTeam);
        bool shotClear = isShotClear(active.pos, bestShotTarget, opponentTeam, 1.5f);

        // Shooting thresholds:
        //   - angle > 0.15 rad (~8.6°)  = decent angle
        //   - angle > 0.30 rad (~17°)   = good angle
        //   - distToGoal < 15m          = in shooting range
        bool goodShot = (shootAngle > 0.15f) && shotClear && (distToGoal < 18.0f);
        bool greatShot = (shootAngle > 0.30f) && shotClear && (distToGoal < 12.0f);

        // 2. Evaluate passing opportunity
        float distToSupport = (active.pos - support.pos).length();
        bool laneClear = isPassingLaneClear(active.pos, support.pos, opponentTeam, 2.5f);
        bool supportAheadOfUs;
        if (isLeftSide) {
            supportAheadOfUs = (support.pos.x > active.pos.x + 2.0f);
        } else {
            supportAheadOfUs = (support.pos.x < active.pos.x - 2.0f);
        }
        float distOpp1 = (active.pos - opponentTeam.p1.pos).length();
        float distOpp2 = (active.pos - opponentTeam.p2.pos).length();
        float closestOpp = std::min(distOpp1, distOpp2);
        bool underPressure = (closestOpp < 4.0f);

        bool shouldPass = (passCooldown <= 0.0f) &&
                           laneClear &&
                           (distToSupport > 4.0f && distToSupport < 25.0f) &&
                           (supportAheadOfUs || (underPressure && distToSupport > 5.0f)) &&
                           possessionTimer > 0.3f;

        // ---- Decision priority: SHOOT > PASS > DRIBBLE ----
        if ((greatShot || (goodShot && underPressure)) &&
            shotCooldown <= 0.0f && possessionTimer > 0.2f) {
            // SHOOT!
            activeState = AIState::SHOOT;
            justShot    = true;
            shotTarget  = bestShotTarget;
            shotCooldown = 2.0f;
            possessionTimer = 0.0f;
        } else if (shouldPass) {
            activeState = AIState::PASS;
            justPassed  = true;
            passCooldown = 1.5f;
            possessionTimer = 0.0f;
        } else {
            // Dribble — but steer toward a better shooting position
            activeState = AIState::DRIBBLE;
            dribble(dt, active, ball, field, isLeftSide);
        }
    }
}

// ============================================================================
// Chase Ball
// ============================================================================
void AIAgent::chaseBall(float dt, Player &player, const Ball &ball,
                        const Field &field, bool isLeftSide) {
    Vector target;
    float approachOffset = 1.5f;
    if (isLeftSide) {
        target = Vector(ball.pos.x - approachOffset, ball.pos.y);
    } else {
        target = Vector(ball.pos.x + approachOffset, ball.pos.y);
    }

    float distToBall = (player.pos - ball.pos).length();
    bool behindBall = isLeftSide ? (player.pos.x < ball.pos.x - 0.5f)
                                  : (player.pos.x > ball.pos.x + 0.5f);
    if (behindBall || distToBall < 2.0f) {
        target = ball.pos;
    }

    seekWithArrival(dt, player, target, 2.0f);
}

// ============================================================================
// Dribble — now steers toward a position with a better shooting angle
// ============================================================================
void AIAgent::dribble(float dt, Player &player, const Ball &ball,
                      const Field &field, bool isLeftSide) {
    float goalX = isLeftSide ? field.getWidth() : 0.0f;
    float goalY = field.getHeight() / 2.0f;
    Vector goalCenter(goalX, goalY);

    // Move toward a point between current position and goal
    // but biased toward the lateral center of the field to open up the angle
    float targetY = goalY * 0.7f + player.pos.y * 0.3f; // drift toward center-Y
    float targetX;
    if (isLeftSide) {
        targetX = player.pos.x + 3.0f; // advance right
        targetX = std::min(targetX, field.getWidth() - 3.0f);
    } else {
        targetX = player.pos.x - 3.0f; // advance left
        targetX = std::max(targetX, 3.0f);
    }

    Vector target(targetX, targetY);
    seekWithArrival(dt, player, target, 2.0f);
}

// ============================================================================
// Pass execution
// ============================================================================
void AIAgent::executePass(Ball &ball, const Player &passer, const Player &receiver) {
    float k = 1.2f;
    Vector passDir = (receiver.pos - ball.pos).normalized();
    float passDist = (receiver.pos - ball.pos).length();
    float passSpeed = std::min(25.0f, std::max(12.0f, passDist * k));
    ball.vel = passDir * passSpeed;
}

// ============================================================================
// Support Player update
// ============================================================================
void AIAgent::updateSupport(float dt, Player &support, const Player &active,
                            const Ball &ball, const Field &field,
                            bool isLeftSide, const Team &opponentTeam) {
    Vector optimalPos = findOptimalReceivingPoint(support, active, ball, field,
                                                  isLeftSide, opponentTeam);
    float distToOptimal = (support.pos - optimalPos).length();

    if (distToOptimal > 1.0f) {
        supportState = AIState::FIND_SPACE;
        seekWithArrival(dt, support, optimalPos, 3.0f);
    } else {
        supportState = AIState::WAIT_RECEIVE;
        Vector toBall = (ball.pos - support.pos).normalized();
        support.pos += toBall * 0.5f * dt;
    }
}

// ============================================================================
// Find optimal receiving position
// ============================================================================
Vector AIAgent::findOptimalReceivingPoint(const Player &support,
                                          const Player &active,
                                          const Ball &ball,
                                          const Field &field,
                                          bool isLeftSide,
                                          const Team &opponentTeam) const {
    float bestScore = -9999.0f;
    Vector bestPos = support.pos;

    float goalX = isLeftSide ? field.getWidth() : 0.0f;
    float goalY = field.getHeight() / 2.0f;

    float margin = 2.0f;
    float stepX = 3.0f;
    float stepY = 3.0f;

    float searchMinX = margin;
    float searchMaxX = field.getWidth() - margin;
    float searchMinY = margin;
    float searchMaxY = field.getHeight() - margin;

    for (float cx = searchMinX; cx <= searchMaxX; cx += stepX) {
        for (float cy = searchMinY; cy <= searchMaxY; cy += stepY) {
            Vector candidate(cx, cy);

            float dOpp1 = (candidate - opponentTeam.p1.pos).length();
            float dOpp2 = (candidate - opponentTeam.p2.pos).length();
            float oppScore = std::min(dOpp1, dOpp2);

            bool laneClear = isPassingLaneClear(ball.pos, candidate, opponentTeam, 2.0f);
            float laneScore = laneClear ? 5.0f : -10.0f;

            float advanceScore;
            if (isLeftSide) {
                advanceScore = cx / field.getWidth();
            } else {
                advanceScore = 1.0f - (cx / field.getWidth());
            }
            advanceScore *= 8.0f;

            float dToActive = (candidate - active.pos).length();
            float spreadScore = (dToActive > 6.0f) ? 3.0f : (dToActive / 6.0f) * 3.0f;

            float dToBall = (candidate - ball.pos).length();
            float reachScore = (dToBall < 20.0f) ? 2.0f : -2.0f * (dToBall / field.getWidth());

            float dToGoalCenter = std::abs(cy - goalY);
            float angleScore = (1.0f - dToGoalCenter / (field.getHeight() / 2.0f)) * 3.0f;

            // New: bonus for positions that have a good shooting angle
            float shotAngle = calculateShootingAngle(candidate, field, isLeftSide);
            float shotAngleScore = shotAngle * 5.0f; // radians -> score

            float totalScore = oppScore + laneScore + advanceScore +
                               spreadScore + reachScore + angleScore + shotAngleScore;

            if (totalScore > bestScore) {
                bestScore = totalScore;
                bestPos = candidate;
            }
        }
    }

    return bestPos;
}

// ============================================================================
// Legacy single-player update (backward compatibility)
// ============================================================================
void AIAgent::update(float dt, Player &aiPlayer, const Ball &ball,
                     const Field &field, bool isLeftSide) {
    float homeX = isLeftSide ? field.getWidth() * 0.25f : field.getWidth() * 0.75f;
    float homeY = field.getHeight() / 2.0f;

    Vector toBall = ball.pos - aiPlayer.pos;
    float distToBall = toBall.length();

    bool ballOnOurSide = isLeftSide ? (ball.pos.x < field.getWidth() / 2.0f)
                                     : (ball.pos.x > field.getWidth() / 2.0f);

    Vector target;
    if (ballOnOurSide || distToBall < 8.0f) {
        if (isLeftSide) {
            target = Vector(ball.pos.x - 1.5f, ball.pos.y);
        } else {
            target = Vector(ball.pos.x + 1.5f, ball.pos.y);
        }
        bool behindBall = isLeftSide ? (aiPlayer.pos.x < ball.pos.x - 0.5f)
                                      : (aiPlayer.pos.x > ball.pos.x + 0.5f);
        if (behindBall || distToBall < 2.0f) {
            target = ball.pos;
        }
    } else {
        target = Vector(homeX, ball.pos.y * 0.5f + homeY * 0.5f);
    }

    seekWithArrival(dt, aiPlayer, target, 2.0f);
    clampToField(aiPlayer, field);
}
