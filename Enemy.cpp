#include "Enemy.h"
#include "Player.h"
#include "EntityManager.h"
#include "Weapon.h"
#include <cmath>
#include <iostream>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────

Enemy::Enemy(float x, float y, float targetH,
             int hp,
             const char*    spritePath,
             sf::Color      fallbackColor,
             const Level*   lvl,
             Player*        p,
             EntityManager* em)
    : DamageableEntity(x, y, 1.0f, targetH, hp), // width=1 placeholder; fixed in tryLoadTexture
      level(lvl),
      player(p),
      entities(em),
      currentState(nullptr),
      weapon(nullptr),
      hasTexture(false),
      placeholderColor(fallbackColor),
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
    hasTexture = tryLoadTexture(spritePath, targetH);

    // If no sprite loaded, give the placeholder box a sensible width.
    if (!hasTexture)
        entityWidth = targetH * 0.55f;

    // Begin in PatrolState. Subclasses can call transitionTo() in their own
    // constructor to start in a different state.
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
    // level, player, entities are non-owning — do NOT delete them.
}

// ─────────────────────────────────────────────────────────────────────────────
// Texture loading
// ─────────────────────────────────────────────────────────────────────────────

bool Enemy::tryLoadTexture(const char* path, float targetH)
{
    if (!texture.loadFromFile(path)) {
        std::cout << "Enemy: no sprite at \"" << path
                  << "\" — using placeholder rectangle.\n";
        return false;
    }

    sprite.setTexture(texture);

    float scale = targetH / static_cast<float>(texture.getSize().y);
    entityWidth = static_cast<float>(texture.getSize().x) * scale;
    baseScaleX  = scale;
    baseScaleY  = scale;
    sprite.setScale(baseScaleX, baseScaleY);

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// AI state helpers
// ─────────────────────────────────────────────────────────────────────────────

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
    if (!player) return 999999.0f;
    float dx = player->getPosX() - positionX;
    float dy = player->getPosY() - positionY;
    return std::sqrt(dx * dx + dy * dy);
}

// ─────────────────────────────────────────────────────────────────────────────
// Movement — called by AI states
// ─────────────────────────────────────────────────────────────────────────────

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
    if (!player) return;
    if (player->getPosX() < positionX) moveLeft();
    else                                moveRight();
}

void Enemy::stopMoving()
{
    velocityX = 0.0f;
}

void Enemy::fireWeapon()
{
    if (!weapon || !entities) return;

    // Face the player before shooting.
    if (player)
        facingRight = (player->getPosX() >= positionX);

    // Spawn projectile from the enemy's center.
    weapon->fire(
        positionX + entityWidth  * 0.5f,
        positionY + entityHeight * 0.5f,
        facingRight,
        false,      // NOT from player → won't damage the player's own entity
        *entities
    );
}

// ─────────────────────────────────────────────────────────────────────────────
// Physics — identical axis-separation logic as Soldier, kept separate per UML
// ─────────────────────────────────────────────────────────────────────────────

void Enemy::resolveHorizontal()
{
    float topY    = positionY + 2.0f;
    float bottomY = positionY + entityHeight - 2.0f;

    if (velocityX < 0.0f) {
        if (level->checkLeftWall(positionX, topY, bottomY)) {
            int col    = static_cast<int>(positionX / level->getCellSize());
            positionX  = (col + 1) * static_cast<float>(level->getCellSize());
            velocityX  = 0.0f;
        }
    }
    else if (velocityX > 0.0f) {
        float rightEdge = positionX + entityWidth;
        if (level->checkRightWall(rightEdge, topY, bottomY)) {
            int col    = static_cast<int>(rightEdge / level->getCellSize());
            positionX  = col * static_cast<float>(level->getCellSize()) - entityWidth;
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
            positionY = hitRow * static_cast<float>(level->getCellSize()) - entityHeight;
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

// ─────────────────────────────────────────────────────────────────────────────
// Update
// ─────────────────────────────────────────────────────────────────────────────

void Enemy::update(float dt)
{
    if (!isActive) return;

    // 1. Tick damage timers (invincibility, flash).
    updateDamageTimers(dt);

    // 2. Tick weapon cooldown.
    if (weapon) weapon->update(dt);

    // 3. Run AI — may transition to a new state.
    if (currentState) {
        EnemyAIState* next = currentState->update(*this, dt);
        if (next) transitionTo(next);
    }

    // 4. Physics — gravity → horizontal move → wall resolution → vertical move → ground resolution.
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

// ─────────────────────────────────────────────────────────────────────────────
// Render
// ─────────────────────────────────────────────────────────────────────────────

void Enemy::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    if (!isActive) return;

    float screenX = positionX - cameraX;
    float screenY = positionY - cameraY;

    // Basic frustum cull — skip if completely off-screen.
    if (screenX + entityWidth  < 0.0f || screenX > 1600.0f) return;
    if (screenY + entityHeight < 0.0f || screenY > 900.0f)  return;

    if (hasTexture) {
        applyDamageFlash(sprite);

        if (facingRight) {
            sprite.setScale( baseScaleX, baseScaleY);
            sprite.setPosition(screenX, screenY);
        }
        else {
            // Mirror: shift right by width so hitbox stays aligned.
            sprite.setScale(-baseScaleX, baseScaleY);
            sprite.setPosition(screenX + entityWidth, screenY);
        }
        window.draw(sprite);
    }
    else {
        // ── Coloured placeholder rectangle ───────────────────────────────────
        sf::RectangleShape rect(sf::Vector2f(entityWidth, entityHeight));
        rect.setPosition(screenX, screenY);

        if (isInvincible()) {
            // Flicker using the same phase logic as applyDamageFlash.
            int phase = static_cast<int>(flashTimer / 0.08f);
            rect.setFillColor(phase % 2 == 0
                ? sf::Color(255, 60, 60, 180)
                : placeholderColor);
        }
        else {
            rect.setFillColor(placeholderColor);
        }

        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2.0f);
        window.draw(rect);

        // Small direction indicator so you can tell which way it's facing.
        sf::RectangleShape arrow(sf::Vector2f(entityWidth * 0.25f, entityHeight * 0.1f));
        arrow.setFillColor(sf::Color::White);
        float arrowX = facingRight
            ? screenX + entityWidth * 0.7f
            : screenX + entityWidth * 0.05f;
        arrow.setPosition(arrowX, screenY + entityHeight * 0.45f);
        window.draw(arrow);
    }
}
