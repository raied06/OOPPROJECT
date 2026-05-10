#pragma once
#include "Player.h"

class FiolinaPlayer : public Player
{
public:
    FiolinaPlayer(float x, float y, const Level* lvl, EntityManager* em);

    Player* createNext(float x, float y, const Level* lvl, EntityManager* em) const override;
    Player* createSelf(float x, float y, const Level* lvl, EntityManager* em) const override;
    int     getCharacterIdx() const override { return 3; }
};
