#pragma once
#include "Weapon.h"

// MeleeWeapon — instant-hit attack, no projectile spawned.
// fire() scans all entities within range on the facing side and deals damage.
class MeleeWeapon : public Weapon
{
private:
    int   damage;
    float range;    // horizontal reach in pixels
    float vRange;   // vertical tolerance in pixels (half-height of the swing arc)
    bool  piercing; // if true, calls receiveMeleeHit (bypasses shields)

public:
    MeleeWeapon(float cooldown, int damage, float range, float vRange = 80.0f, bool piercing = false);

    void fire(float x, float y,
              bool  facingRight,
              bool  fromPlayer,
              EntityManager& em) override;
};
