#include "EnemyAIState.h"
#include "Enemy.h"

// PatrolState

PatrolState::PatrolState(float duration): patrolTimer(0.0f), patrolDuration(duration), walkingRight(true){}

void PatrolState::enter(Enemy& enemy)
{
    walkingRight = true;
    patrolTimer  = 0.0f;
}

EnemyAIState* PatrolState::update(Enemy& enemy, float dt)
{
    // Transition Check
    if (enemy.distanceToPlayer() < enemy.getDetectionRange())
        return new ChaseState();

    // Patrol movement
    patrolTimer += dt;
    if (patrolTimer >= patrolDuration) {
        walkingRight = !walkingRight;
        patrolTimer  = 0.0f;
    }

    if (walkingRight)
        enemy.moveRight();
    else             
        enemy.moveLeft();

    return 
        nullptr; // stay in PatrolState
}

void PatrolState::exit(Enemy& enemy)
{
    enemy.stopMoving();
}


// ChaseState

void ChaseState::enter(Enemy& /*Whatever enemy (decided later)*/) {}

EnemyAIState* ChaseState::update(Enemy& enemy, float dt)
{
    float dist = enemy.distanceToPlayer();

    //Transition checks
    if (dist < enemy.getAttackRange())
        return new AttackState();

    if (dist > enemy.getDetectionRange() * 1.3f) // 30 % hysteresis to avoid ping-pong
        return new PatrolState();

    // Chase movement
    enemy.moveTowardPlayer();

    return nullptr; // Continue chase
}

void ChaseState::exit(Enemy& enemy)
{
    enemy.stopMoving();
}

// AttackState

AttackState::AttackState(float cooldown, int /*bMax*/) : attackCooldown(cooldown), cooldownTimer(0.0f) // fire immediately
{}

void AttackState::enter(Enemy& enemy)
{
    enemy.stopMoving();
    cooldownTimer = 0.0f; // shoot right away on entry
}

EnemyAIState* AttackState::update(Enemy& enemy, float dt)
{
    float dist = enemy.distanceToPlayer();

    // Player moved out of attack range — give chase again.
    if (dist > enemy.getAttackRange() * 1.5f)
        return new ChaseState();

    // Stay stopped and face the player every frame.
    enemy.stopMoving();

    // Fire on cooldown — indefinitely while player is in range.
    cooldownTimer -= dt;
    if (cooldownTimer <= 0.0f) {
        enemy.fireWeapon();
        cooldownTimer = attackCooldown;
    }

    return nullptr;
}

void AttackState::exit(Enemy& /*enemy*/) {}


// ParachuteState

void ParachuteState::enter(Enemy& enemy)
{
    enemy.stopMoving();
}

EnemyAIState* ParachuteState::update(Enemy& enemy, float dt)
{
    // Just wait until gravity brings it down
    if (enemy.isOnGround())
        return new PatrolState();

    return nullptr;
}

void ParachuteState::exit(Enemy& /*enemy*/) {}


