#pragma once
#include "Player.h"

class EriPlayer : public Player
{
public:
    EriPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 2; }
};
