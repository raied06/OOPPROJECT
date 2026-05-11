#include "MummyWarrior.h"
#include "Player.h"

static constexpr float MUMMY_TARGET_H     = 150.0f;
static constexpr int   MUMMY_HP           = 5;
static constexpr float MUMMY_MOVE_SPEED   = 70.0f;
static constexpr float MUMMY_DETECT_RANGE = 480.0f;
static constexpr float MUMMY_ATTACK_RANGE = 65.0f; // attack range = contact range (melee only)

MummyWarrior::MummyWarrior(float x, float y,
                           const Level*   lvl,
                           Player*        p,
                           EntityManager* em)
    : Enemy(x, y,
            MUMMY_TARGET_H,
            MUMMY_HP,
            "Sprites/Enemies/Mummy.png",
            lvl, p, em),
      resurrectionTimer(0.0f),
      resurrectionCount(0),
      contactCooldown(0.0f)
{
    moveSpeed      = MUMMY_MOVE_SPEED;
    detectionRange = MUMMY_DETECT_RANGE;
    attackRange    = MUMMY_ATTACK_RANGE;

    // No ranged weapon — mummy is melee only.
    weapon = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Resurrection override
// ─────────────────────────────────────────────────────────────────────────────

void MummyWarrior::takeDamage(int amount)
{
    // Cannot be hurt while crumbling.
    if (resurrectionTimer > 0.0f) return;

    // Let the base class apply damage (it calls deactivateEntity() at 0 HP).
    Enemy::takeDamage(amount);

    // Bullets / knife can never kill the mummy permanently — it always
    // crumbles and rises again. The ONLY ways to make the kill stick are
    // fire and explosions, which call Enemy::takeDamage directly via
    // receiveFireHit / receiveExplosionHit and bypass this override.
    if (!isActive) {
        isActive          = true;         // undo the base-class deactivation
        currentHP         = maxHP;        // back to full health
        resurrectionTimer = RESURRECTION_DELAY;
        resurrectionCount++;
        invincibilityTimer = RESURRECTION_DELAY; // invincible while crumbling
        flashTimer         = 0.0f;
        stopMoving();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Fire / explosion bypass — these are the ONLY damage channels that can kill
// a mummy permanently. They skip the crumble-and-rise cycle entirely.
// ─────────────────────────────────────────────────────────────────────────────

bool MummyWarrior::receiveFireHit(int /*damage*/, bool fromPlayer)
{
    if (!fromPlayer) return false;
    // Disable any further resurrection AND clear the crumble invincibility,
    // then push HP straight to zero so the entity stays dead.
    resurrectionCount = MAX_RESURRECTIONS;
    resurrectionTimer = 0.0f;
    Enemy::takeDamage(9999);
    return true;
}

bool MummyWarrior::receiveExplosionHit(int /*damage*/, bool fromPlayer)
{
    if (!fromPlayer) return false;
    resurrectionCount = MAX_RESURRECTIONS;
    resurrectionTimer = 0.0f;
    Enemy::takeDamage(9999);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Update — resurrection tick + melee contact
// ─────────────────────────────────────────────────────────────────────────────

void MummyWarrior::update(float dt)
{
    if (!isActive) return;

    // ── Resurrection countdown ────────────────────────────────────────────────
    if (resurrectionTimer > 0.0f) {
        resurrectionTimer -= dt;
        if (resurrectionTimer < 0.0f) resurrectionTimer = 0.0f;
        // While crumbling: don't run AI or apply physics — just stand still.
        // Still tick base damage timers so the invincibility expires properly.
        updateDamageTimers(dt);
        return;
    }

    // ── Melee contact ─────────────────────────────────────────────────────────
    if (contactCooldown > 0.0f) contactCooldown -= dt;

    if (player && player->getIsActive() && contactCooldown <= 0.0f) {
        float myMidX = positionX + entityWidth  * 0.5f;
        float myMidY = positionY + entityHeight * 0.5f;
        float plMidX = player->getPosX() + player->getEntityWidth()  * 0.5f;
        float plMidY = player->getPosY() + player->getEntityHeight() * 0.5f;
        float dx = myMidX - plMidX;
        float dy = myMidY - plMidY;

        if (dx * dx + dy * dy < CONTACT_RANGE * CONTACT_RANGE) {
            player->takeDamage(CONTACT_DAMAGE);
            contactCooldown = CONTACT_INTERVAL;
            // TODO: trigger player TransformationState
        }
    }

    // ── Normal AI + physics ───────────────────────────────────────────────────
    Enemy::update(dt);
}
