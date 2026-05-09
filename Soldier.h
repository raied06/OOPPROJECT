#pragma once
#include "DamageableEntity.h"
#include "Level.h"

// Soldier is anything that walks and obeys gravity base.
// Player inherits from this. (Enemy grunts will too)

class Soldier : public DamageableEntity
{
protected:
    const Level* level;  // not owned by Soldier, Level is owned by PlayState so will not be deleted here (aggregation)

    bool  onGround; //Used to answer two things, 1.To check if jump is allowed or not, 2.Is player standing on the ground?
    bool  facingRight;
    float baseScaleX; // Magnitude of horizontal scale
    float baseScaleY; // magnitude of verticle scale

    float moveSpeed;      // how many pixels/sec (horizontally only)
    float jumpStrength;   // instantaneous upward velocity (would always be negative)
    float gravity;        // pixels/sec^2 (downward acceleration)
    float maxFallSpeed;   // terminal velocity clamp

    sf::Texture texture;
    sf::Sprite  sprite;

    void resolveHorizontal();
    // Calculates the checks of Horizontal Movement (wall collisions etc) 

    void resolveVertical(float dt); // Gravity is handeled in this func
    // Calculates the checks of vertical movement 

public:
    Soldier(float x, float y, float w, float h, int hp, const Level* lvl);
    virtual ~Soldier();

    virtual void update(float dt) override;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    // Below functions are used by the derived classes (players) bcz they will get inputs, not Soldier
    void moveLeft();
    void moveRight();
    void stopHorizontal();
    void jump();

    void setBaseScale(float x, float y);

    bool isOnGround() const { return onGround; }

    // Accepts hits from enemy projectiles only.
    virtual bool receiveProjectileHit(int damage, bool fromPlayer) override
    {
        if (fromPlayer) return false; // friendly fire — ignore
        takeDamage(damage);
        return true;
    }
};