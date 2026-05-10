#pragma once
#include "Player.h"

class EriPlayer : public Player
{
public:
    EriPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 2; }

protected:
    // Eri weakness: cannot use a melee attack at all.
    Weapon* createKnife() const override { return nullptr; }

    // Eri buff: 50% bigger grenade blast radius (120 × 1.5 = 180).
    float grenadeBlastRadius() const override { return 180.0f; }

    // Power-up: while active, every grenade throw spawns a bonus grenade
    // (extra one flies 40% farther) at the cost of a single grenade.
    int grenadesPerThrow() const override { return isPowerUpActive() ? 2 : 1; }
};
