#pragma once
#include "Enemy.h"

//   HP : 2
//   Weapon : Rocket Launcher (1 rocket, 3.0 s cooldown, 5 dmg, ballistic)
//   Movement speed: 70 px/s

class BazookaSoldier : public Enemy
{
public:
    BazookaSoldier(float x, float y,
                   const Level*   lvl,
                   Player*        p,
                   EntityManager* em);

    virtual ~BazookaSoldier() {}

    int getScoreValue() const override;
};
