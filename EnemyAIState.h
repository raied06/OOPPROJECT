#pragma once

class Enemy; // Forward decleration to avoid circular dependancy i.e, Enemy.h include this file and this will include Enemy.h

//                                                                  ABSTRACT BASE CLASS
class EnemyAIState
{
public:
    virtual ~EnemyAIState() {}

    // Called once immediately after the state becomes active.
    virtual void enter(Enemy& enemy) = 0;
    // Called every frame. Returns the next state to transition into, or nullptr to remain in the current state.
    virtual EnemyAIState* update(Enemy& enemy, float dt) = 0;
    // Called once just before the state is replaced or destroyed.
    virtual void exit(Enemy& enemy) = 0;
};


//                         PATROL STATE
// The enemy walks back and forth with pausees, if player gets in detection range, they transition to chase state
// ─────────────────────────────────────────────────────────────────────────────
class PatrolState : public EnemyAIState
{
    float patrolTimer;      // time elapsed walking in the current direction
    float patrolDuration;   // time before turning around
    bool  walkingRight;

public:
    // Patrol duration i.e, how many seconds before reversing (default 4s)
    explicit PatrolState(float duration = 4.0f);

    void enter(Enemy& enemy) override;
    EnemyAIState* update(Enemy& enemy, float dt) override;
    void exit(Enemy& enemy) override;
};

// CHASE STATE
// The enemy moves toward the player every frame.
//  If Player enters attackRange, enemy transitions to AttackState
//  If Player leaves detectionRange, enemy transitions to PatrolState
// ─────────────────────────────────────────────────────────────────────────────
class ChaseState : public EnemyAIState
{
public:
    void enter(Enemy& enemy) override;
    EnemyAIState* update(Enemy& enemy, float dt) override;
    void exit(Enemy& enemy) override;
};


// ═════════════════════════════════════════════════════════════════════════════
// ParachuteState  —  used by Paratrooper while descending
// ═════════════════════════════════════════════════════════════════════════════
// Gravity is already reduced on the Paratrooper itself. This state just waits
// until the enemy touches the ground, then hands off to PatrolState.
// ─────────────────────────────────────────────────────────────────────────────
class ParachuteState : public EnemyAIState
{
public:
    void enter(Enemy& enemy) override;
    EnemyAIState* update(Enemy& enemy, float dt) override;
    void exit(Enemy& enemy) override;
};


//                      ATTACK STATE
// The enemy stops moving and fires continuously towards the pllayer with a cooldown until the player leaves attack range.
// If player runs away, then transitions to chaseState.
class AttackState : public EnemyAIState
{
    float attackCooldown;   // seconds between shots
    float cooldownTimer;    // countdown to next shot

public:
    // attackCooldown: seconds between shots (default 1.2 s)
    explicit AttackState(float attackCooldown = 1.2f, int = 0);

    void enter(Enemy& enemy) override;
    EnemyAIState* update(Enemy& enemy, float dt) override;
    void exit(Enemy& enemy) override;
};
