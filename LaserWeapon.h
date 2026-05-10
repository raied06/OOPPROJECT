#pragma once
#include "Weapon.h"

class Level;

// LaserWeapon — instant-hit beam weapon.
// fire() scans horizontally for the first solid tile, damages all entities
// in the beam's path, then spawns a LaserBeam entity for the visual.
class LaserWeapon : public Weapon
{
private:
    int          damage;
    float        maxRange;  // pixels
    const Level* level;

public:
    LaserWeapon(float cooldown, int damage, const Level* lvl, float maxRange = 1200.0f);

    void fire(float x, float y, bool facingRight, bool fromPlayer,
              EntityManager& em) override;
};
