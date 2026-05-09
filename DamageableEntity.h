#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

// Sits between Entity and everything that can be hurt:
//   Soldier (and therefore Player), Enemy, Vehicle.
//
// Responsibilities:
//   - HP tracking (currentHP / maxHP)
//   - Invincibility window after a hit (1 second, per project spec)
//   - Red-flash visual feedback during that window
//
// DamageableEntity is still abstract: update() and render() remain
// pure virtual so concrete subclasses must implement them.

class DamageableEntity : public Entity
{
protected:
    int   maxHP;
    int   currentHP;
    float invincibilityTimer;   // counts DOWN from INVINCIBILITY_DURATION after a hit
    float flashTimer;           // accumulates UP; used to produce the flicker phase

    static constexpr float INVINCIBILITY_DURATION = 1.0f;  // seconds
    static constexpr float FLASH_INTERVAL         = 0.08f; // flicker period (seconds)

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
