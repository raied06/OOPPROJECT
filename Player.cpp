#include "Player.h"
#include <iostream>

static const float PLAYER_H = 150.0f;
// used to assign a same hitbox to every animation state as well as it is a fix height of every player (Marco, Tarma etc)

Player::Player(float x, float y, const Level* lvl)
    : Soldier(x, y, 1.0f, PLAYER_H, lvl),
    jumpHeldLastFrame(false)
{
    if (!texture.loadFromFile("Sprites/Char.png")) {
        std::cout << "ERROR: Player texture failed to load\n";
    }
    sprite.setTexture(texture);

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
    // The snap logic of player if it tries to enter the left side of the screen is in PlayState.cpp (update function)
    bool right = Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right);
    bool jumpHeld = Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Space);

    if (left && !right)      
        moveLeft();
    else if (right && !left) 
        moveRight();
    else                     
        stopHorizontal();

    if (jumpHeld && !jumpHeldLastFrame) 
        jump();
    jumpHeldLastFrame = jumpHeld;
}

void Player::update(float dt)
{
    if (!isActive) return;
    handleInput();
    Soldier::update(dt);
}