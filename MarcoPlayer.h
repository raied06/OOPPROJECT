#pragma once
#include "Player.h"

class MarcoPlayer : public Player
{
public:
    MarcoPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 0; }
};
