#include "DamageableEntity.h"

DamageableEntity::DamageableEntity(float x, float y, float w, float h, int hp)
    : Entity(x, y, w, h),
      maxHP(hp),
      currentHP(hp),
      invincibilityTimer(0.0f),
      flashTimer(0.0f)
{}

// ─────────────────────────────────────────────────────────────────────────────

void DamageableEntity::takeDamage(int amount)
{
    if (!isActive)                return; // already dead
    if (invincibilityTimer > 0.0f) return; // invincibility window — ignore hit
    if (amount <= 0)              return; // sanity

    currentHP -= amount;

    if (currentHP <= 0) {
        currentHP = 0;
        deactivateEntity(); // mark for removal on the next cleanup pass
    }
    else {
        // Start the 1-second invincibility window and reset the flicker phase.
        invincibilityTimer = INVINCIBILITY_DURATION;
        flashTimer         = 0.0f;
    }
}

void DamageableEntity::heal(int amount)
{
    if (!isActive || amount <= 0) return;
    currentHP += amount;
    if (currentHP > maxHP) currentHP = maxHP;
}

// ─────────────────────────────────────────────────────────────────────────────

void DamageableEntity::updateDamageTimers(float dt)
{
    if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= dt;
        flashTimer         += dt;
        if (invincibilityTimer < 0.0f) {
            invincibilityTimer = 0.0f;
            flashTimer         = 0.0f;
        }
    }
}

void DamageableEntity::applyDamageFlash(sf::Sprite& sprite) const
{
    if (invincibilityTimer > 0.0f) {
        // Alternate between a translucent red tint and normal every FLASH_INTERVAL.
        int phase = static_cast<int>(flashTimer / FLASH_INTERVAL);
        if (phase % 2 == 0)
            sprite.setColor(sf::Color(255, 60, 60, 180)); // red, semi-transparent
        else
            sprite.setColor(sf::Color(255, 255, 255, 255)); // normal
    }
    else {
        sprite.setColor(sf::Color(255, 255, 255, 255)); // always restore to normal
    }
}
