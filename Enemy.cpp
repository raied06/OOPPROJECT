#include "Enemy.h"
#include "Player.h"
#include "EntityManager.h"
#include "Weapon.h"
#include <iostream>


Enemy::Enemy(float x, float y, float targetH,
             int hp,
             const char*    spritePath,
             const Level*   lvl,
             Player*        p,
             EntityManager* em)
    : DamageableEntity(x, y, 1.0f, targetH, hp),
      level(lvl),
      player(p),
      entities(em),
      currentState(nullptr),
      weapon(nullptr),
      facingRight(false),
      baseScaleX(1.0f),
      baseScaleY(1.0f),
      onGround(false),
      moveSpeed(120.0f),
      gravity(1500.0f),
      maxFallSpeed(1200.0f),
      detectionRange(450.0f),
      attackRange(250.0f)
{
    texture.loadFromFile(spritePath);
    loadTexture(targetH);

    currentState = new PatrolState();
    currentState->enter(*this);
}

Enemy::~Enemy()
{
    if (currentState) {
        currentState->exit(*this);
        delete currentState;
        currentState = nullptr;
    }
    delete weapon;
    weapon = nullptr;
    // level, player, entities are not owned here (aggregation) so they will not be deleted.
}

bool Enemy::loadTexture(float targetH)
{
    sprite.setTexture(texture);

    float scale = targetH / static_cast<float>(texture.getSize().y);
    entityWidth = static_cast<float>(texture.getSize().x) * scale;
    baseScaleX  = scale;
    baseScaleY  = scale;
    sprite.setScale(baseScaleX, baseScaleY);

    return true;
}

void Enemy::transitionTo(EnemyAIState* newState)
{
    if (currentState) {
        currentState->exit(*this);
        delete currentState;
    }
    currentState = newState;
    if (currentState)
        currentState->enter(*this);
}

float Enemy::distanceToPlayer() const
{
    if (!player) 
        return 999999.0f;
    float dx = player->getPosX() - positionX;
    float dy = player->getPosY() - positionY;
    // TO calculate the root, I am using Newton-Raphson method so that even the numbers which are not perfect squares
    // can easily be found. Using the formula of it and after simplifying it we get formula:
    //					nextGuess = (previousGuess + originalValue/previousGuess)/2
    // Doing it for 50 iterations to obtain more efficient results for very large numbers.
    double guess = (dx * dx + dy * dy) / 2.0;
    for (int i = 0; i < 50; i++) {
        guess = (guess / 2.0) + ((dx * dx + dy * dy) / (2.0 * guess));
    }
    return guess;
}

float Enemy::getDetectionRange() const
{
    return detectionRange;
}

float Enemy::getAttackRange() const
{
    return attackRange;
}

bool Enemy::isOnGround() const
{
    return onGround;
}

Player* Enemy::getPlayer() const
{
    return player;
}

float Enemy::getWidth() const
{
    return entityWidth;
}

float Enemy::getVelocityY() const
{
    return velocityY;
}

void Enemy::moveLeft()
{
    velocityX   = -moveSpeed;
    facingRight = false;
}

void Enemy::moveRight()
{
    velocityX   = moveSpeed;
    facingRight = true;
}

void Enemy::moveTowardPlayer()
{
    if (!player) 
        return;
    if (player->getPosX() < positionX) 
        moveLeft();
    else                                
        moveRight();
}

void Enemy::stopMoving()
{
    velocityX = 0.0f;
}

void Enemy::fireWeapon()
{
    if (!weapon || !entities) return;

    // Face the player before shooting
    if (player)
        facingRight = (player->getPosX() >= positionX);

    // Firing from the entity's middle
    weapon->fire(
        positionX + entityWidth  * 0.5f,
        positionY + entityHeight * 0.5f,
        facingRight,
        false,      // NOT from player → won't damage the player's own entity
        *entities
    );
}

void Enemy::resolveHorizontal()
{
    float topY    = positionY + 2.0f;
    float bottomY = positionY + entityHeight - 2.0f;

    if (velocityX < 0.0f) {
        if (level->checkLeftWall(positionX, topY, bottomY)) {
            int col = (int)(positionX / level->getCellSize());
            positionX = (col + 1) * (float)(level->getCellSize());
            velocityX  = 0.0f;
        }
    }
    else if (velocityX > 0.0f) {
        float rightEdge = positionX + entityWidth;
        if (level->checkRightWall(rightEdge, topY, bottomY)) {
            int col = (int)(rightEdge / level->getCellSize());
            positionX = col * (float)(level->getCellSize()) - entityWidth;
            velocityX  = 0.0f;
        }
    }
}

void Enemy::resolveVertical()
{
    float leftX  = positionX + 2.0f;
    float rightX = positionX + entityWidth - 2.0f;

    if (velocityY > 0.0f) {
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
            positionY = hitRow * (float)(level->getCellSize()) - entityHeight;
            velocityY = 0.0f;
            onGround  = true;
        }
        else {
            onGround = false;
        }
    }
    else if (velocityY < 0.0f) {
        float headY = positionY - 1.0f;
        int hitRow;
        if (level->checkCeiling(headY, leftX, rightX, hitRow)) {
            positionY = (hitRow + 1) * static_cast<float>(level->getCellSize());
            velocityY = 0.0f;
        }
    }
    else {
        // Check we haven't walked off a ledge.
        float feetY = positionY + entityHeight + 1.0f;
        int hitRow;
        if (!level->checkGroundBelow(feetY, leftX, rightX, hitRow))
            onGround = false;
    }
}

void Enemy::update(float dt)
{
    if (!isActive) return;

    // Update the invincibility and flash if got a git
    updateDamageTimers(dt);

    // Update Weapon cooldown (if any)
    if (weapon) 
        weapon->update(dt);

    // Run the enemy AI (change the enemy state if certain conditions hold)
    if (currentState) {
        EnemyAIState* next = currentState->update(*this, dt);
        if (next) 
            transitionTo(next);
    }

    // Running the physics for enemy (same as Soldier)
    velocityY += gravity * dt;
    if (velocityY > maxFallSpeed) velocityY = maxFallSpeed;

    positionX += velocityX * dt;

    // Clamp to left world edge (mirrors the fix in Soldier).
    if (positionX < 0.0f) {
        positionX = 0.0f;
        if (velocityX < 0.0f) velocityX = 0.0f;
    }

    resolveHorizontal();

    positionY += velocityY * dt;
    resolveVertical();
}

void Enemy::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;

    float screenX = positionX - cameraX;
    float screenY = positionY - cameraY;

    if (screenX + entityWidth  < 0.0f || screenX > 1600.0f) return;
    if (screenY + entityHeight < 0.0f || screenY > 900.0f)  return;

    applyDamageFlash(sprite);

    if (!facingRight) {
        sprite.setScale( baseScaleX, baseScaleY);
        sprite.setPosition(screenX, screenY);
    }
    else {
        sprite.setScale(-baseScaleX, baseScaleY);
        sprite.setPosition(screenX + entityWidth, screenY);
    }
    window.draw(sprite);
}

void Enemy::applyScreenClamp(float cameraX)
{
    // Don't clamp enemies that haven't been reached by the camera yet.
    if (positionX > cameraX + 1600.0f) return;

    float rightLimit = cameraX + 1600.0f - entityWidth;
    if (positionX > rightLimit) {
        positionX = rightLimit;
        velocityX = 0.0f;
    }
}

bool Enemy::receiveProjectileHit(int damage, bool fromPlayer)
{
    if (!fromPlayer)
        return false;
    takeDamage(damage);
    return true;
}
