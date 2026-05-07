#include "Player.h"
#include <iostream>

// Target rendered height in pixels. Tune this one number to resize the player.
// Slightly less than one tile (64px) so they fit under low ceilings.
static const float PLAYER_H = 122.0f;

Player::Player(float x, float y, const Level* lvl)
    : Soldier(x, y, 1.0f, PLAYER_H, lvl),   // width=1 placeholder, fixed below
    jumpHeldLastFrame(false)
{
    if (!texture.loadFromFile("Sprites/Char.png")) {
        std::cout << "ERROR: Player texture failed to load\n";
    }
    sprite.setTexture(texture);

    // ONE scale factor derived from height — applied to BOTH axes.
    // This preserves the original artwork's proportions exactly.
    float uniformScale = PLAYER_H / (float)texture.getSize().y;
    float actualWidth = (float)texture.getSize().x * uniformScale;

    // Entity hitbox width is protected — derived class can set it directly.
    // We fix it here after we know the real rendered width.
    entityWidth = actualWidth;

    sprite.setScale(uniformScale, uniformScale);  // same scale on both axes
    setBaseScale(uniformScale, uniformScale);
}

Player::~Player() {}

void Player::handleInput()
{
    using sf::Keyboard;

    bool left = Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left);
    bool right = Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right);
    bool jumpHeld = Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Space);

    if (left && !right)      moveLeft();
    else if (right && !left) moveRight();
    else                     stopHorizontal();

    if (jumpHeld && !jumpHeldLastFrame) jump();
    jumpHeldLastFrame = jumpHeld;
}

void Player::update(float dt)
{
    if (!isActive) return;
    handleInput();
    Soldier::update(dt);
}