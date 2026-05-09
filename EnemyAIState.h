#pragma once

// Forward declaration prevents circular include (Enemy.h includes this file;
// EnemyAIState only needs Enemy as a reference parameter).
class Enemy;

// ═════════════════════════════════════════════════════════════════════════════
// EnemyAIState  —  abstract base for the State Pattern
// ═════════════════════════════════════════════════════════════════════════════
// Each concrete state owns its own behaviour. The enemy calls update() each
// frame; if a transition is needed the state allocates the new state on the
// heap and returns it. The enemy then swaps it in (deletes the old one).
//
// Returning nullptr means "stay in this state".
// ─────────────────────────────────────────────────────────────────────────────
class EnemyAIState
{
public:
    virtual ~EnemyAIState() {}

    // Called once immediately after the state becomes active.
    virtual void enter(Enemy& enemy) = 0;

    // Called every frame. Returns the next state to transition to,
    // or nullptr to remain in the current state.
    virtual EnemyAIState* update(Enemy& enemy, float dt) = 0;

    // Called once just before the state is replaced/destroyed.
    virtual void exit(Enemy& enemy) = 0;
};


// ═════════════════════════════════════════════════════════════════════════════
// PatrolState
// ═════════════════════════════════════════════════════════════════════════════
// The enemy walks back-and-forth on a timer. If the player steps within
// detectionRange it transitions to ChaseState.
// ─────────────────────────────────────────────────────────────────────────────
class PatrolState : public EnemyAIState
{
    float patrolTimer;      // time elapsed walking in the current direction
    float patrolDuration;   // seconds before turning around
    bool  walkingRight;

public:
    // patrolDuration: how many seconds before reversing (default 2 s)
    explicit PatrolState(float duration = 2.0f);

    void           enter(Enemy& enemy) override;
    EnemyAIState*  update(Enemy& enemy, float dt) override;
    void           exit(Enemy& enemy) override;
};


// ═════════════════════════════════════════════════════════════════════════════
// ChaseState
// ═════════════════════════════════════════════════════════════════════════════
// The enemy moves toward the player every frame.
//   • Player enters attackRange  → AttackState
//   • Player leaves detectionRange → PatrolState
// ─────────────────────────────────────────────────────────────────────────────
class ChaseState : public EnemyAIState
{
public:
    void           enter(Enemy& enemy) override;
    EnemyAIState*  update(Enemy& enemy, float dt) override;
    void           exit(Enemy& enemy) override;
};


// ═════════════════════════════════════════════════════════════════════════════
// AttackState
// ═════════════════════════════════════════════════════════════════════════════
// The enemy stops moving and fires continuously on a cooldown while the player
// stays within attackRange. Transitions to ChaseState if the player escapes.
// ─────────────────────────────────────────────────────────────────────────────
class AttackState : public EnemyAIState
{
    float attackCooldown;   // seconds between shots
    float cooldownTimer;    // counts DOWN to next shot

public:
    // attackCooldown: seconds between shots (default 1.2 s)
    explicit AttackState(float attackCooldown = 1.2f, int /*unused*/ = 0);

    void           enter(Enemy& enemy) override;
    EnemyAIState*  update(Enemy& enemy, float dt) override;
    void           exit(Enemy& enemy) override;
};
