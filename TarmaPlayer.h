#pragma once
#include "Player.h"

class TarmaPlayer : public Player
{
public:
    TarmaPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 1; }

    // Damage taken — fully ignored while the power-up is active (20s immunity).
    void takeDamage(int amount) override;

protected:
    // Tarma's power-up lasts 20 seconds instead of the base 10.
    float powerUpDuration() const override { return 20.0f; }
};
