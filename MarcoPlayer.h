#pragma once
#include "Player.h"

class MarcoPlayer : public Player
{
public:
    MarcoPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 0; }

protected:
    // Dual fire: after each forward shot, immediately fire one bullet backward.
    void onShotFired(float x, float y, bool facingRight) override;

    // Piercing knife that instakills ShieldedSoldier by bypassing their shield.
    Weapon* createKnife() const override;
};
