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
    {}

Soldier::~Soldier()
{
    // Textures and sprites are handeled by SFML itself, we don't need to clean them.
    // Level is not composed here, so no need to delete it here.
}

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
    if (!onGround) 
        return;

    velocityY = jumpStrength;
    onGround = false;
}

void Soldier::setBaseScale(float x, float y)
{
    this->baseScaleX = x;
    this->baseScaleY = y;
}

// COLLISION DETECTION LOGIC
void Soldier::resolveHorizontal()
{
    float topY = positionY + 2.0f;
    float bottomY = positionY + entityHeight - 2.0f;
    // This plus +2 and -2 is done because if it is not done then when player 
    // passes near a block (even on the ground), the collision detection is done by
    // corner of the sprite, and this falsely detects a block in front of the player
    // and player gets stuck at the block.

    if (velocityX < 0.0f) { // Means player is moving left
        if (level->checkLeftWall(positionX, topY, bottomY)) {
            int col = (int)(positionX / level->getCellSize());
            positionX = (col + 1) * (float)level->getCellSize(); 
            // This puts the player's left edge at the right side of the tile it entered and by
            // this, the player's left edge touches the right side of the block, but not gets inside it
            velocityX = 0.0f;
        }
    }
    else if (velocityX > 0.0f) {
        float rightEdge = positionX + entityWidth;
        if (level->checkRightWall(rightEdge, topY, bottomY)) {
            int col = (int)(rightEdge / level->getCellSize());
            // As here, we don't want the player to be inside the block which we are checking
            // and th epurpose of this check is the player should not enter the right block, 
            // so by this formula, player gets stopped exactly at the left edge of the block at the right
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
        float feetY = positionY + entityHeight;
        int hitRow = 0; // gets updated inside checkGroundBelow function as it is passed by reference
        if (level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            positionY = hitRow * (float)level->getCellSize() - entityHeight;
            // 384 pixels = 6 whole blocks
            // player is trying to move 2 pixels into the ground (block)
            // hitRow is returned with the value : 386 / 64 = 6.031 which becomes 6, so playerY+width again fixes to 
            // 6*64 = 384 pixels, and as render function isn't even calleed yet, the player visually never moves.
            velocityY = 0.0f;
            onGround = true;
        }
        else {
            onGround = false;
        }
    }
    else if (velocityY < 0.0f) {
        float headY = positionY - 1.0f;
        int hitRow = 0;
        if (level->checkCeiling(headY, leftX, rightX, hitRow)) {
            positionY = (hitRow + 1) * (float)level->getCellSize();
            velocityY = 0.0f;
        }
    }
    else { // means velocityY == 0, now checking if player fell of a block so that we should apply gravity to it.
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (!level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            onGround = false;
        }
    }
}

void Soldier::update(float dt)
{
    if (!isActive) return;

    // Applying Gravity as it acts at every time
/* HOW GRAVITY WORKS ?
   Although gravity is called every frame regardless of the player's position (on ground or in air), and it does
   increase the velocityY of the player, which should move player towards the bottom and it actually is done, but in
   the same frame, the function "resolveVertical" is called and it applies the check under the condition 
   "if (velocityY > 0.0f), and then if ground is detected below the player's feet, it instantly snaps the player back
   to the ground as render function isn't called yet, the player never moves visually on the screen into the ground.
   And if ground isn't detected, onGround is false and player isn't snapped back onto the ground, so when render function
   is called, it moves player into the ground."*/

    velocityY += gravity * dt;
    if (velocityY > maxFallSpeed) velocityY = maxFallSpeed;

    // Applying Horizontal Movement
    positionX += velocityX * dt;
    resolveHorizontal();

    // Applying Vertical Movement
    positionY += velocityY * dt;
    resolveVertical(dt);
}

void Soldier::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;

    if (facingRight) {
        sprite.setScale(baseScaleX, baseScaleY);
        sprite.setPosition(positionX - cameraX, positionY - cameraY);
    }
    else {
        sprite.setScale(-baseScaleX, baseScaleY);
        sprite.setPosition(positionX - cameraX + entityWidth, positionY - cameraY);
    }

    window.draw(sprite);
}