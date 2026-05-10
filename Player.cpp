#include "Player.h"
#include "EntityManager.h"
#include "ProjectileWeapon.h"
#include "MeleeWeapon.h"
#include "FlameWeapon.h"
#include "LaserWeapon.h"
#include <iostream>

static const float PLAYER_H = 150.0f;

// ── Constructor / Destructor ──────────────────────────────────────────────────

Player::Player(float x, float y, const Level* lvl, EntityManager* em, int hp)
    : Soldier(x, y, 1.0f, PLAYER_H, hp, lvl),
    jumpHeldLastFrame(false),
    fireHeldLastFrame(false),
    knifeHeldLastFrame(false),
    activeSlot(0),
    prevSlot(0),
    devMode(false),
    fireSpriteTimer(0.0f),
    knifeSpriteTimer(0.0f),
    entities(em)
{
    for (int i = 0; i < SLOT_COUNT; i++)
        weaponSlots[i] = nullptr;
    // Slot 0 (pistol) is created by each concrete subclass via setPistol().
}

Player::~Player()
{
    for (int i = 0; i < SLOT_COUNT; i++) {
        delete weaponSlots[i];
        weaponSlots[i] = nullptr;
    }
}

// ── Protected helpers (used by subclasses) ────────────────────────────────────

void Player::loadAllWeaponSprites(const char* pistolPath,
                                   const char* machineGunPath,
                                   const char* riflePath,
                                   const char* knifePath,
                                   const char* firePath)
{
    // Pistol is mandatory — always exists, used as fallback for missing sprites.
    if (!texPistol.loadFromFile(pistolPath))
        std::cout << "Player: MISSING pistol sprite: " << pistolPath << "\n";

    if (!texMachineGun.loadFromFile(machineGunPath)) {
        std::cout << "Player: no MachineGun sprite — using pistol\n";
        texMachineGun = texPistol;
    }
    if (!texRifle.loadFromFile(riflePath)) {
        std::cout << "Player: no Rifle sprite — using pistol\n";
        texRifle = texPistol;
    }
    if (!texKnife.loadFromFile(knifePath)) {
        std::cout << "Player: no Knife sprite — using pistol\n";
        texKnife = texPistol;
    }
    if (!texFire.loadFromFile(firePath)) {
        std::cout << "Player: no Fire sprite — using pistol\n";
        texFire = texPistol;
    }

    // Lock physics dimensions to the pistol sprite so hitbox never changes.
    float scale = PLAYER_H / static_cast<float>(texPistol.getSize().y);
    entityWidth  = static_cast<float>(texPistol.getSize().x) * scale;
    setBaseScale(scale, scale);

    // Start showing the pistol sprite.
    sprite.setTexture(texPistol, true);
    sprite.setScale(scale, scale);
    sprite.setColor(sf::Color::White);
}

const sf::Texture* Player::texForSlot(int slot) const
{
    // Slot → sprite mapping as per spec:
    //   0 Pistol        → Pistol
    //   1 HMG           → MachineGun
    //   2 Rocket        → Rifle
    //   3 Grenade       → Rifle
    //   5 Flame Shot    → Rifle
    //   6 Laser Gun     → Rifle
    switch (slot) {
        case 1:  return &texMachineGun;
        case 2:  return &texRifle;
        case 3:  return &texRifle;
        case 5:  return &texRifle;
        case 6:  return &texRifle;
        default: return &texPistol;
    }
}

void Player::updateActiveSprite()
{
    const sf::Texture* chosen;

    if (knifeSpriteTimer > 0.0f)
        chosen = &texKnife;
    else if (fireSpriteTimer > 0.0f)
        chosen = &texFire;
    else
        chosen = texForSlot(activeSlot);

    if (sprite.getTexture() != chosen) {
        sprite.setTexture(*chosen, true);

        // Recompute scale so every texture fills exactly PLAYER_H in height.
        // entityWidth stays locked to the pistol sprite for physics — only the
        // visual scale changes here.
        float sy = PLAYER_H / static_cast<float>(chosen->getSize().y);
        float sx = PLAYER_H / static_cast<float>(chosen->getSize().y);
        setBaseScale(sx, sy);
        sprite.setScale(sx, sy);
    }
}

void Player::setPistol(float cooldown)
{
    delete weaponSlots[0];
    weaponSlots[0] = new ProjectileWeapon(
        cooldown, 1, -1, 600.0f, false, sf::Color(255, 255, 100), level
    );
}

// ── Private helpers ───────────────────────────────────────────────────────────

void Player::equipSlot(int slot)
{
    if (slot < 0 || slot >= SLOT_COUNT || !weaponSlots[slot]) return;
    if (slot != 4) prevSlot = slot; // knife never overwrites prevSlot
    activeSlot = slot;
}

void Player::giveAllWeapons()
{
    // slot 0 = pistol already set by subclass

    if (!weaponSlots[1]) // HMG
        weaponSlots[1] = new ProjectileWeapon(
            0.08f, 1, -1, 700.0f, false, sf::Color(255, 200, 50), level);

    if (!weaponSlots[2]) // Rocket Launcher — 100 px explosion
        weaponSlots[2] = new ProjectileWeapon(
            1.2f, 8, -1, 400.0f, true, sf::Color(255, 100, 50), level, 100.0f);

    if (!weaponSlots[3]) // Grenade Launcher — 120 px explosion
        weaponSlots[3] = new ProjectileWeapon(
            0.8f, 5, -1, 350.0f, true, sf::Color(100, 200, 50), level, 120.0f);

    if (!weaponSlots[4]) // Knife — melee, 90 px reach
        weaponSlots[4] = new MeleeWeapon(0.4f, 3, 90.0f);

    if (!weaponSlots[5]) // Flame Shot
        weaponSlots[5] = new FlameWeapon(0.1f, 2, level, 180.0f);

    if (!weaponSlots[6]) // Laser Gun
        weaponSlots[6] = new LaserWeapon(1.0f, 6, level, 1200.0f);
}

// ── Public interface ──────────────────────────────────────────────────────────

void Player::equipWeapon(Weapon* newWeapon)
{
    delete weaponSlots[0];
    weaponSlots[0] = newWeapon;
    activeSlot = 0;
}

void Player::takeDamage(int amount)
{
    if (devMode) return;
    DamageableEntity::takeDamage(amount);
}

void Player::toggleDevMode()
{
    devMode = !devMode;
    if (devMode) {
        giveAllWeapons();
        std::cout << "[DEV] ON — immortal, all weapons unlocked\n"
                  << "      1=Pistol 2=HMG 3=Rocket 4=Grenade 5=Flame 6=Laser  X=Knife\n";
    }
    else {
        activeSlot = 0;
        std::cout << "[DEV] OFF\n";
    }
}

void Player::handleInput()
{
    using sf::Keyboard;
    using sf::Mouse;

    // ── Developer mode toggle (R key) ─────────────────────────────────────────
    static bool rHeldLast = false;
    bool rHeld = Keyboard::isKeyPressed(Keyboard::R);
    if (rHeld && !rHeldLast) toggleDevMode();
    rHeldLast = rHeld;

    // ── Weapon slot selection (dev mode only) ─────────────────────────────────
    if (devMode) {
        // Edge-detect each number key so one press = one swap, not a flood.
        static bool numHeld[7] = {};
        bool numNow[7] = {
            false,
            Keyboard::isKeyPressed(Keyboard::Num1),
            Keyboard::isKeyPressed(Keyboard::Num2),
            Keyboard::isKeyPressed(Keyboard::Num3),
            Keyboard::isKeyPressed(Keyboard::Num4),
            Keyboard::isKeyPressed(Keyboard::Num5),
            Keyboard::isKeyPressed(Keyboard::Num6),
        };
        if (numNow[1] && !numHeld[1]) { equipSlot(0); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        if (numNow[2] && !numHeld[2]) { equipSlot(1); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        if (numNow[3] && !numHeld[3]) { equipSlot(2); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        if (numNow[4] && !numHeld[4]) { equipSlot(3); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        if (numNow[5] && !numHeld[5]) { equipSlot(5); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        if (numNow[6] && !numHeld[6]) { equipSlot(6); fireSpriteTimer = 0.0f; knifeSpriteTimer = 0.0f; }
        for (int i = 1; i <= 6; i++) numHeld[i] = numNow[i];
    }

    // ── Knife attack (X key, dev mode, edge-detected) ─────────────────────────
    bool knifeHeld = devMode && Keyboard::isKeyPressed(Keyboard::X);
    if (knifeHeld && !knifeHeldLastFrame && weaponSlots[4] && entities) {
        float swingX = facingRight ? positionX + entityWidth : positionX;
        float swingY = positionY + entityHeight * 0.5f;
        weaponSlots[4]->fire(swingX, swingY, facingRight, true, *entities);
        knifeSpriteTimer = KNIFE_ANIM_DURATION;
        fireSpriteTimer  = 0.0f;
        equipSlot(prevSlot); // auto-return to ranged weapon
    }
    knifeHeldLastFrame = knifeHeld;

    // ── Movement ──────────────────────────────────────────────────────────────
    bool left     = Keyboard::isKeyPressed(Keyboard::A)     || Keyboard::isKeyPressed(Keyboard::Left);
    bool right    = Keyboard::isKeyPressed(Keyboard::D)     || Keyboard::isKeyPressed(Keyboard::Right);
    bool jumpHeld = Keyboard::isKeyPressed(Keyboard::W)     || Keyboard::isKeyPressed(Keyboard::Space);

    if (left && !right)       moveLeft();
    else if (right && !left)  moveRight();
    else                      stopHorizontal();

    if (jumpHeld && !jumpHeldLastFrame) jump();
    jumpHeldLastFrame = jumpHeld;

    // ── Shoot (LMB or Z key, edge-detected) ──────────────────────────────────
    bool fireHeld = Mouse::isButtonPressed(Mouse::Left)
                 || Keyboard::isKeyPressed(Keyboard::Z);
    if (fireHeld && !fireHeldLastFrame) {
        Weapon* w = weaponSlots[activeSlot];
        if (w && entities) {
            float spawnX = facingRight ? positionX + entityWidth + 2.0f : positionX - 4.0f;
            float spawnY = positionY + entityHeight * 0.45f;
            w->fire(spawnX, spawnY, facingRight, true, *entities);
            // Flash the fire sprite; knife animation takes priority so don't clobber it.
            if (knifeSpriteTimer <= 0.0f)
                fireSpriteTimer = FIRE_ANIM_DURATION;
        }
    }
    fireHeldLastFrame = fireHeld;
}

void Player::applyScreenClamp(float cameraX)
{
    if (positionX < cameraX) {
        positionX = cameraX;
        if (velocityX < 0.0f) velocityX = 0.0f;
    }
    if (positionX + entityWidth > cameraX + 1600.0f) {
        positionX = cameraX + 1600.0f - entityWidth;
        if (velocityX > 0.0f) velocityX = 0.0f;
    }
}

void Player::update(float dt)
{
    if (!isActive) return;
    handleInput();

    // Tick sprite animation timers.
    if (fireSpriteTimer  > 0.0f) fireSpriteTimer  -= dt;
    if (knifeSpriteTimer > 0.0f) knifeSpriteTimer -= dt;

    updateActiveSprite();

    for (int i = 0; i < SLOT_COUNT; i++)
        if (weaponSlots[i]) weaponSlots[i]->update(dt);
    Soldier::update(dt);
}
