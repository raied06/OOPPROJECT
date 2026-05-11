#pragma once
#include "Enemy.h"
#include <SFML/Graphics.hpp>


class Paratrooper : public Enemy
{
private:
    bool hasLanded;
    bool hasDeparted; // true once the fall has begun
    sf::Texture rebelTexture;   // swapped on landing

public:
    Paratrooper(float x, float y, const Level* lvl, Player* p, EntityManager* em);

    virtual ~Paratrooper() {}

    virtual void update(float dt) override;

    int getScoreValue() const override;
};
