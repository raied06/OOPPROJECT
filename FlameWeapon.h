#pragma once
#include "Weapon.h"

class Level;

// FlameWeapon — rapid-fire short-range flame shots.
class FlameWeapon : public Weapon
{
private:
    int          damage;
    float        maxRange;
    const Level* level;

public:
    FlameWeapon(float cooldown, int damage, const Level* lvl, float maxRange = 180.0f);

    void fire(float x, float y, bool facingRight, bool fromPlayer,
              EntityManager& em) override;
};
