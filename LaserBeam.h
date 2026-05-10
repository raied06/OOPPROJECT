#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

// LaserBeam — a purely visual entity spawned by LaserWeapon.
// No damage dealt here; it just renders the beam and fades out.
class LaserBeam : public Entity
{
private:
    float             length;
    float             lifetime;
    sf::RectangleShape shape;

public:
    LaserBeam(float x, float y, float length, bool facingRight);

    void update(float dt) override;
    void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    bool receiveProjectileHit(int, bool) override { return false; }
    void applyScreenClamp(float) override {}
};
