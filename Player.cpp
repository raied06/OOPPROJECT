#include "Player.h"
#include <iostream>

// Width and height here are the logical hitbox size, not the raw texture size.
// You'll tweak these once you have a real sprite sheet loaded.
static const float PLAYER_W = 48.0f;
static const float PLAYER_H = 80.0f;

Player::Player(float x, float y, const Level* lvl)
    : Soldier(x, y, PLAYER_W, PLAYER_H, lvl),
    jumpHeldLastFrame(false)
{
    // Load the player sprite. Keep the path consistent with what PlayState used.
    if (!texture.loadFromFile("Sprites/Armaghan.png")) {
        std::cout << "ERROR: Player texture failed to load\n";
    }
    sprite.setTexture(texture);

    // Scale to fit the hitbox. Raw texture / hitbox size = scale factor.
    float scaleX = PLAYER_W / (float)texture.getSize().x;
    float scaleY = PLAYER_H / (float)texture.getSize().y;
    sprite.setScale(scaleX, scaleY);
}

Player::~Player()
{
    // texture + sprite are SFML stack members, cleaned up automatically.
}

void Player::handleInput()
{
    // Keyboard::isKeyPressed is the compliant method per the academic constraints.
    // event.key.code is banned - we don't use it here at all.
    using sf::Keyboard;

    bool left = Keyboard::isKeyPressed(Keyboard::A) ||
        Keyboard::isKeyPressed(Keyboard::Left);

    bool right = Keyboard::isKeyPressed(Keyboard::D) ||
        Keyboard::isKeyPressed(Keyboard::Right);

    bool jumpHeld = Keyboard::isKeyPressed(Keyboard::W) ||
        Keyboard::isKeyPressed(Keyboard::Space) ||
        Keyboard::isKeyPressed(Keyboard::Up);

    // Horizontal: left/right are mutually exclusive.
    // If both held at once (weird but possible), cancel out to stop.
    if (left && !right)       moveLeft();
    else if (right && !left)  moveRight();
    else                      stopHorizontal();

    // Jump: edge-triggered. Only fires the FIRST frame the key goes down.
    // jumpHeldLastFrame prevents: land -> still holding space -> auto-jump again.
    if (jumpHeld && !jumpHeldLastFrame) {
        jump(); // Soldier::jump() already guards against mid-air jumps
    }
    jumpHeldLastFrame = jumpHeld;
}

void Player::update(float dt)
{
    if (!isActive) return;

    // Input first, then physics. Order matters:
    // velocityX must be set BEFORE Soldier::update moves the position.
    handleInput();

    // Let Soldier handle gravity, movement, and all tile collision.
    Soldier::update(dt);
}