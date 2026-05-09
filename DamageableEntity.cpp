#include "DamageableEntity.h"

const float DamageableEntity::INVINCIBILITY_DURATION = 1.0f;
const float DamageableEntity::FLASH_INTERVAL = 0.08f;

DamageableEntity::DamageableEntity(float x, float y, float w, float h, int hp)
    : Entity(x, y, w, h),
    maxHP(hp),
    currentHP(hp),
    invincibilityTimer(0.0f),
    flashTimer(0.0f)
{
}

void DamageableEntity::takeDamage(int amount)
{
    if (!isActive)
        return; // already dead
    if (invincibilityTimer > 0.0f)
        return; // invincibility time (ignoring any hit taken)
    if (amount <= 0)
        return;

    currentHP -= amount;

    if (currentHP <= 0) {
        currentHP = 0;
        deactivateEntity(); // mark for removal on the next cleanup pass
    }
    else {
        // Start the 1-second invincibility window and reset the flicker phase.
        invincibilityTimer = INVINCIBILITY_DURATION;
        flashTimer = 0.0f;
    }
}

void DamageableEntity::heal(int amount)
{
    if (!isActive || amount <= 0)
        return;
    currentHP += amount;
    if (currentHP > maxHP)
        currentHP = maxHP;
}

// ─────────────────────────────────────────────────────────────────────────────

void DamageableEntity::updateDamageTimers(float dt)
{
    if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= dt;
        flashTimer += dt;
        if (invincibilityTimer < 0.0f) {
            invincibilityTimer = 0.0f;
            flashTimer = 0.0f;
        }
    }
}

void DamageableEntity::applyDamageFlash(sf::Sprite& sprite) const
{
    if (invincibilityTimer > 0.0f) {
        // Red color and normal color alternate every 80 milliseconds
        // When phase is even, red color appears and if odd, then normal.
        // Phase working is like when flashTimer is less then 80 miliseconds, it gives 0 which is even so red color appears.
        // When flashTimer increases that 80 mili sec but is less than 160 mili sec, colorCheck is always 1, so normal color appears,
        // when flash is between 160-240 mili sec, again red color and so on......
        int colorCheck = (int)(flashTimer / FLASH_INTERVAL);
        if (colorCheck % 2 == 0)
            sprite.setColor(sf::Color(255, 60, 60, 180)); // red, semi-transparent
        else
            sprite.setColor(sf::Color(255, 255, 255, 255)); // normal
    }
    else {
        sprite.setColor(sf::Color(255, 255, 255, 255)); // always restore to normal
    }
}
