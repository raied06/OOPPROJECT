#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
// ABSTRACT CLASS

// Anything that can be damaged is Damageable entity. Like, soldier, enemy and vehicles.
// This class is used to track health (current hp, max hp), invincibility time after taking 
// a hit, and shows the red flash when entity gets hit.
class DamageableEntity : public Entity
{
protected:
    int   maxHP;
    int   currentHP;
    float invincibilityTimer;   // counts down from INVINCIBILITY_DURATION(1 sec) after a hit
    float flashTimer;           // starts from 0 and counts up, (goes upto 1 sec max)

    static const float INVINCIBILITY_DURATION;  // seconds
    static const float FLASH_INTERVAL; // flicker period (seconds)

    // ── Helpers for subclasses ────────────────────────────────────────────────
    // Call once per frame at the top of your update() to tick timers.
    void updateDamageTimers(float dt);

    // Call just before window.draw(sprite) to apply / remove the red tint.
    // Resets the sprite colour to white when no longer invincible.
    void applyDamageFlash(sf::Sprite& sprite) const;

public:
    DamageableEntity(float x, float y, float w, float h, int hp);
    virtual ~DamageableEntity() {}

    // Applies damage unless the entity is currently invincible.
    // Triggers invincibility window; calls deactivateEntity() on death.
    virtual void takeDamage(int amount);

    bool isInvincible() const { return invincibilityTimer > 0.0f; }
    int  getHP()        const { return currentHP; }
    int  getMaxHP()     const { return maxHP; }
    void heal(int amount);

    // Still pure virtual — this class is abstract.
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) = 0;
};
