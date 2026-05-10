#pragma once
#include "Player.h"

class FiolinaPlayer : public Player
{
public:
    FiolinaPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 3; }

    // Weapon pickup buff: 50% more ammo than base when picking up a weapon.
    void equipWeapon(Weapon* newWeapon) override;

protected:
    // Fiolina weakness: knife deals 25% less damage. 2 × 0.75 ≈ 1 HP.
    Weapon* createKnife() const override;

    // Power-up: SUPERCHARGED — auto-fire on hold + halved per-shot cooldown.
    bool autoFireOnHold() const override { return isPowerUpActive(); }
    void onShotFired(float x, float y, bool facingRight) override;
};
