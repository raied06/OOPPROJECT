#include "Player.h"
#include "EntityManager.h"
#include "ProjectileWeapon.h"
#include <iostream>

static const float PLAYER_H = 150.0f;
// used to assign a same hitbox to every animation state as well as it is a fix height of every player (Marco, Tarma etc)

Player::Player(float x, float y, const Level* lvl, EntityManager* em, int hp)
    : Soldier(x, y, 1.0f, PLAYER_H, hp, lvl),
    jumpHeldLastFrame(false),
    fireHeldLastFrame(false),
    weapon(nullptr),
    entities(em)
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

    // Default weapon: Pistol (infinite ammo, 1 damage, ~3 shots/sec, straight)
    weapon = new ProjectileWeapon(
        0.35f,                    // cooldown (seconds between shots)
        1,                        // damage per bullet
        -1,                       // infinite ammo
        600.0f,                   // bullet speed px/s
        false,                    // straight shot (no gravity)
        sf::Color(255, 255, 100), // pale-yellow bullet
        lvl
    );
}

Player::~Player()
{
    delete weapon;
    weapon = nullptr;
    // entities is non-owning — do NOT delete it here
}

void Player::equipWeapon(Weapon* newWeapon)
{
    delete weapon;
    weapon = newWeapon; // take ownership
}

void Player::handleInput()
{
    using sf::Keyboard;
    using sf::Mouse;

    // ── Movement ─────────────────────────────────────────────────────────────
    bool left  = Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left);
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

    // ── Shooting ─────────────────────────────────────────────────────────────
    // Left mouse button fires (one shot per click — edge detected).
    // Z key is a keyboard fallback for mouse-less testing.
    // Direction = whichever way the player is currently facing.
    // TODO (Phase 2): replace with mouse-cursor angle aiming once window
    //                 reference is threaded through to Player.
    bool fireHeld = Mouse::isButtonPressed(Mouse::Left)
                 || Keyboard::isKeyPressed(Keyboard::Z);

    if (fireHeld && !fireHeldLastFrame) {
        if (weapon && entities) {
            // Spawn bullet at the front edge of the player, mid-height
            float spawnX = facingRight
                ? positionX + entityWidth + 2.0f
                : positionX - 4.0f;
            float spawnY = positionY + entityHeight * 0.45f;

            weapon->fire(spawnX, spawnY, facingRight, true, *entities);
        }
    }
    fireHeldLastFrame = fireHeld;
}

void Player::update(float dt)
{
    if (!isActive) return;

    handleInput();

    // Tick weapon cooldown
    if (weapon) weapon->update(dt);

    // Soldier::update handles gravity, movement, tile collision,
    // and calls updateDamageTimers (via DamageableEntity base)
    Soldier::update(dt);
}
