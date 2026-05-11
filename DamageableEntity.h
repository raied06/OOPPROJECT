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

   
    // Call once per frame to update timers.
    void updateDamageTimers(float dt);
    void applyDamageFlash(sf::Sprite& sprite) const;

public:
    DamageableEntity(float x, float y, float w, float h, int hp);
    virtual ~DamageableEntity() {}

    // Applies damage unless the entity is currently invincible.
    // Triggers invincibility time, calls deactivateEntity() on death.
    virtual void takeDamage(int amount);

    bool isInvincible() const;
    int getHP() const;
    int getMaxHP() const;
    void heal(int amount);

    // Will be used by child classes
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) = 0;
};
