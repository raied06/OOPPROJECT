#include "ShieldedSoldier.h"
#include "ProjectileWeapon.h"
#include "Player.h"

// ── Tuning constants ──────────────────────────────────────────────────────────
static constexpr float SHIELDED_TARGET_H     = 150.0f; // matches player height
static constexpr int   SHIELDED_HP           = 5;
static constexpr float SHIELDED_MOVE_SPEED   = 90.0f;
static constexpr float SHIELDED_DETECT_RANGE = 400.0f;
static constexpr float SHIELDED_ATTACK_RANGE = 220.0f;

static constexpr float PISTOL_COOLDOWN = 1.4f;
static constexpr int   PISTOL_DAMAGE   = 1;
static constexpr float PISTOL_SPEED    = 480.0f;
// ─────────────────────────────────────────────────────────────────────────────

ShieldedSoldier::ShieldedSoldier(float x, float y,
                                 const Level*   lvl,
                                 Player*        p,
                                 EntityManager* em)
    : Enemy(x, y,
            SHIELDED_TARGET_H,
            SHIELDED_HP,
            "Sprites/Enemies/shielded_soldier.png",
            lvl, p, em)
{
    moveSpeed      = SHIELDED_MOVE_SPEED;
    detectionRange = SHIELDED_DETECT_RANGE;
    attackRange    = SHIELDED_ATTACK_RANGE;

    weapon = new ProjectileWeapon(
        PISTOL_COOLDOWN,
        PISTOL_DAMAGE,
        -1,
        PISTOL_SPEED,
        false,
        sf::Color(200, 200, 255),  // light-blue bullet
        lvl
    );
}

// ── Shield logic ──────────────────────────────────────────────────────────────

bool ShieldedSoldier::playerIsInFront() const
{
    if (!player) return false;

    // Player centre X vs. our centre X.
    float myMidX     = positionX + entityWidth * 0.5f;
    float playerMidX = player->getPosX() + player->getEntityWidth() * 0.5f;

    // Shield faces the direction the soldier is facing.
    // "In front" means the player is on the same side as our face.
    if (facingRight)
        return playerMidX > myMidX;  // player is to the right
    else
        return playerMidX < myMidX;  // player is to the left
}

bool ShieldedSoldier::receiveMeleeHit(int /*damage*/, bool fromPlayer)
{
    if (!fromPlayer) return false;
    // Piercing knife ignores the shield and kills instantly.
    DamageableEntity::takeDamage(9999);
    return true;
}

void ShieldedSoldier::takeDamage(int amount)
{
    // Shield blocks frontal hits.  We treat ALL incoming damage here as a
    // horizontal projectile from the player unless the soldier is not facing
    // the player — in which case the shot came from behind and goes through.
    // Explosion / fall damage can be added later by passing a DamageType enum.
    if (playerIsInFront()) {
        // Shield absorbs the hit — play a visual flash but deal 0 hp loss.
        // We still call the parent so the invincibility timer resets properly,
        // but we pass 0 damage.
        DamageableEntity::takeDamage(0);
        return;
    }

    // Not shielded on this side — take full damage.
    DamageableEntity::takeDamage(amount);
}
