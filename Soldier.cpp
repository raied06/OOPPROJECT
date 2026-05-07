#include "Soldier.h"
#include <iostream>

Soldier::Soldier(float x, float y, float w, float h, const Level* lvl)
    : Entity(x, y, w, h),
    level(lvl),
    onGround(false),
    facingRight(true),
    moveSpeed(300.0f),
    jumpStrength(-700.0f),
    gravity(1500.0f),
    maxFallSpeed(1200.0f)
{
    // Derived classes (Player, enemies) load their own texture.
    // Soldier itself is abstract enough that we don't force a sprite here.
    // If you ever instantiate Soldier directly for testing,
    // just call sprite.setTexture(texture) after loading.
}

Soldier::~Soldier()
{
    // texture and sprite are stack members - SFML cleans them up automatically.
    // level is non-owning, do NOT delete it here.
}

// ------------------------------------------------------------
// MOVEMENT INTENT - called by Player (or AI) before update()
// ------------------------------------------------------------

void Soldier::moveLeft()
{
    velocityX = -moveSpeed;
    facingRight = false;
}

void Soldier::moveRight()
{
    velocityX = moveSpeed;
    facingRight = true;
}

void Soldier::stopHorizontal()
{
    velocityX = 0.0f;
}

void Soldier::jump()
{
    // Ignore if already airborne - no bunny hop
    if (!onGround) return;

    velocityY = jumpStrength;
    onGround = false;
}

// ------------------------------------------------------------
// COLLISION RESOLUTION - separate axis, order matters
// ------------------------------------------------------------

void Soldier::resolveHorizontal()
{
    // We check the leading edge only - left side when moving left,
    // right side when moving right. The inset on Y is 2px so we
    // don't falsely collide with the floor tile while walking along it.
    float topY = positionY + 2.0f;
    float bottomY = positionY + entityHeight - 2.0f;

    if (velocityX < 0.0f) {
        // Moving left: check left wall
        if (level->checkLeftWall(positionX, topY, bottomY)) {
            // Figure out which tile column we're touching and snap right edge flush
            int col = (int)(positionX / level->getCellSize());
            positionX = (col + 1) * (float)level->getCellSize(); // right edge of that tile
            velocityX = 0.0f;
        }
    }
    else if (velocityX > 0.0f) {
        // Moving right: check right wall
        float rightEdge = positionX + entityWidth;
        if (level->checkRightWall(rightEdge, topY, bottomY)) {
            int col = (int)(rightEdge / level->getCellSize());
            positionX = col * (float)level->getCellSize() - entityWidth;
            velocityX = 0.0f;
        }
    }
}

void Soldier::resolveVertical(float dt)
{
    // Inset the X range by 2px on each side - same reason as above,
    // prevents the player from "catching" on tile corners mid-air.
    float leftX = positionX + 2.0f;
    float rightX = positionX + entityWidth - 2.0f;

    if (velocityY > 0.0f) {
        // Falling: check ground under feet
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            // Snap feet exactly to top of the tile
            positionY = hitRow * (float)level->getCellSize() - entityHeight;
            velocityY = 0.0f;
            onGround = true;
        }
        else {
            onGround = false;
        }
    }
    else if (velocityY < 0.0f) {
        // Rising (jumping): check ceiling above head
        float headY = positionY - 1.0f;
        int hitRow;
        if (level->checkCeiling(headY, leftX, rightX, hitRow)) {
            // Snap top of entity to bottom of the ceiling tile
            positionY = (hitRow + 1) * (float)level->getCellSize();
            velocityY = 0.0f; // kill upward momentum - you bonked your head
        }
    }
    else {
        // velocityY == 0: verify we're still on solid ground,
        // because a tile could've been destroyed under us (future feature)
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (!level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            onGround = false; // walked off a ledge
        }
    }
}

// ------------------------------------------------------------
// UPDATE - gravity -> move -> collide, in that order
// ------------------------------------------------------------

void Soldier::update(float dt)
{
    if (!isActive) return;

    // --- Gravity ---
    // Always accumulate downward acceleration, every frame, no exceptions.
    // If you skip this when onGround == true the character floats on slopes.
    velocityY += gravity * dt;

    // Clamp to terminal velocity. Without this, a lag spike can push
    // velocityY high enough to tunnel straight through a tile in one frame.
    if (velocityY > maxFallSpeed) {
        velocityY = maxFallSpeed;
    }

    // --- Apply horizontal movement then resolve X collisions ---
    positionX += velocityX * dt;
    resolveHorizontal();

    // --- Apply vertical movement then resolve Y collisions ---
    positionY += velocityY * dt;
    resolveVertical(dt);

    // --- Sync sprite to world position ---
    // Note: sprite.setPosition takes SCREEN coords - camera offset applied in render()
    sprite.setTextureRect(sf::IntRect(0, 0,
        (int)texture.getSize().x,
        (int)texture.getSize().y));

    // Flip sprite horizontally when facing left, without getGlobalBounds
    if (facingRight) {
        sprite.setScale(sprite.getScale().x < 0
            ? -sprite.getScale().x
            : sprite.getScale().x,
            sprite.getScale().y);
    }
    else {
        sprite.setScale(sprite.getScale().x > 0
            ? -sprite.getScale().x
            : sprite.getScale().x,
            sprite.getScale().y);
    }
}

void Soldier::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;

    // World position minus camera offset = screen position
    sprite.setPosition(positionX - cameraX, positionY - cameraY);
    window.draw(sprite);
}