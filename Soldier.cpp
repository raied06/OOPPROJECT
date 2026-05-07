#include "Soldier.h"
#include <iostream>

Soldier::Soldier(float x, float y, float w, float h, const Level* lvl)
    : Entity(x, y, w, h),
    level(lvl),
    onGround(false),
    facingRight(true),
    baseScaleX(1.0f),
    baseScaleY(1.0f),
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

void Soldier::setBaseScale(float x, float y)
{
    this->baseScaleX = x;
    this->baseScaleY = y;
}

// ------------------------------------------------------------
// COLLISION RESOLUTION - separate axis, order matters
// ------------------------------------------------------------

void Soldier::resolveHorizontal()
{
    float topY = positionY + 2.0f;
    float bottomY = positionY + entityHeight - 2.0f;

    if (velocityX < 0.0f) {
        if (level->checkLeftWall(positionX, topY, bottomY)) {
            int col = (int)(positionX / level->getCellSize());
            positionX = (col + 1) * (float)level->getCellSize();
            velocityX = 0.0f;
        }
    }
    else if (velocityX > 0.0f) {
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
    float leftX = positionX + 2.0f;
    float rightX = positionX + entityWidth - 2.0f;

    if (velocityY > 0.0f) {
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            positionY = hitRow * (float)level->getCellSize() - entityHeight;
            velocityY = 0.0f;
            onGround = true;
        }
        else {
            onGround = false;
        }
    }
    else if (velocityY < 0.0f) {
        float headY = positionY - 1.0f;
        int hitRow;
        if (level->checkCeiling(headY, leftX, rightX, hitRow)) {
            positionY = (hitRow + 1) * (float)level->getCellSize();
            velocityY = 0.0f;
        }
    }
    else {
        // velocityY == 0: check we haven't walked off a ledge
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (!level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            onGround = false;
        }
    }
}

// ------------------------------------------------------------
// UPDATE - gravity -> move -> collide, in that order
// ------------------------------------------------------------

void Soldier::update(float dt)
{
    if (!isActive) return;

    // Gravity — always accumulate, every frame
    velocityY += gravity * dt;
    if (velocityY > maxFallSpeed) velocityY = maxFallSpeed;

    // Horizontal move → wall collision
    positionX += velocityX * dt;

    // *** THE BUG FIX ***
    // Clamp to world left edge HERE, before resolveVertical runs.
    // If positionX is negative when resolveVertical fires, column
    // indices go to 0 via C++ truncation-toward-zero, causing wrong
    // tile lookups → phantom ground detection → player floats in air.
    if (positionX < 0.0f) {
        positionX = 0.0f;
        if (velocityX < 0.0f) velocityX = 0.0f;
    }

    resolveHorizontal();  // wall collision (was wrongly commented out by partner)

    // Vertical move → ground/ceiling collision
    positionY += velocityY * dt;
    resolveVertical(dt);
}

void Soldier::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;

    // Set scale based on direction. We always SET, never toggle - that's
    // why the old code flickered. baseScaleX is the positive magnitude
    // we registered once in Player::Player(); we just choose the sign here.
    if (facingRight) {
        sprite.setScale(baseScaleX, baseScaleY);
        sprite.setPosition(positionX - cameraX, positionY - cameraY);
    }
    else {
        // When scaleX is negative, SFML mirrors the sprite around its origin (top-left).
        // That pushes the visible pixels to the LEFT of the origin, so the sprite
        // appears at [positionX - entityWidth, positionX]. We shift right by
        // entityWidth so the visible sprite sits over the same hitbox region.
        sprite.setScale(-baseScaleX, baseScaleY);
        sprite.setPosition(positionX - cameraX + entityWidth, positionY - cameraY);
    }

    window.draw(sprite);
}