#pragma once
#include "Entity.h"
#include "Level.h"

// Soldier is the "anything that walks and obeys gravity" base.
// Player inherits from this. Enemy grunts will too.
// It does NOT know about keyboard input - that's Player's job.

class Soldier : public Entity
{
protected:
    const Level* level;  // non-owning. Level is owned by PlayState.

    bool  onGround;
    bool  facingRight;
    float baseScaleX; // Magnitude of horizontal scale
    float baseScaleY; // magnitude of verticl scale

    float moveSpeed;      // pixels/sec horizontal
    float jumpStrength;   // instantaneous upward velocity (negative value)
    float gravity;        // pixels/sec^2 downward acceleration
    float maxFallSpeed;   // terminal velocity clamp - prevents tunneling at high dt

    sf::Texture texture;
    sf::Sprite  sprite;

    // Separates horizontal and vertical resolution so we don't get
    // corner-sticking or wall-clipping artifacts.
    void resolveHorizontal();
    void resolveVertical(float dt);

public:
    Soldier(float x, float y, float w, float h, const Level* lvl);
    virtual ~Soldier();

    // Soldier provides a default update. Player can override if needed.
    virtual void update(float dt) override;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) override;

    // Called by derived classes (Player) to set intent each frame.
    // Soldier doesn't poll input itself.
    void moveLeft();
    void moveRight();
    void stopHorizontal();
    void jump();           // only fires if onGround == true

    void setBaseScale(float x, float y);

    bool isOnGround() const { return onGround; }
};