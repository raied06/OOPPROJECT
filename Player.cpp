#include "Player.h"
#include "EntityManager.h"
#include "ProjectileWeapon.h"
#include "MeleeWeapon.h"
#include <iostream>

static const float PLAYER_H = 150.0f;

// ── Character stats table ─────────────────────────────────────────────────────
// Order MUST match CharacterType enum: Marco, Tarma, Eri, Fiolina
const CharacterStats Player::CHARACTER_TABLE[] =
{
    //  name              sprite path                       hp  speed   jump     pistolCD  placeholder tint
    { "Marco Rossi",    "Sprites/Characters/marco.png",    5,  300.f,  -700.f,  0.35f,   sf::Color(255, 220, 180) }, // peach
    { "Tarma Roving",   "Sprites/Characters/tarma.png",    6,  260.f,  -650.f,  0.40f,   sf::Color(180, 200, 255) }, // light-blue
    { "Eri Kasamoto",   "Sprites/Char.png",                5,  300.f,  -700.f,  0.35f,   sf::Color(255, 255, 255) }, // no tint (real sprite)
    { "Fiolina Germi",  "Sprites/Characters/fio.png",      4,  360.f,  -760.f,  0.28f,   sf::Color(200, 255, 200) }, // pale green
};

Player::Player(float x, float y, const Level* lvl, EntityManager* em, int hp)
    : Soldier(x, y, 1.0f, PLAYER_H, hp, lvl),
    jumpHeldLastFrame(false),
    fireHeldLastFrame(false),
    knifeHeldLastFrame(false),
    switchHeldLastFrame(false),
    currentCharacter(CharacterType::Eri),
    activeSlot(0),
    prevSlot(0),
    devMode(false),
    entities(em)
{
    for (int i = 0; i < SLOT_COUNT; i++)
        weaponSlots[i] = nullptr;

    // Slot 0: Pistol — actual cooldown is set by applyCharacter() below.
    weaponSlots[0] = new ProjectileWeapon(
        0.35f, 1, -1, 600.0f, false, sf::Color(255, 255, 100), lvl
    );

    // Apply Eri's stats and load her sprite (existing Char.png)
    applyCharacter(CharacterType::Eri);
}

// ── Character system ─────────────────────────────────────────────────────────

void Player::applyCharacter(CharacterType c)
{
    currentCharacter = c;
    const CharacterStats& s = CHARACTER_TABLE[static_cast<int>(c)];

    // Patch movement / health stats on the Soldier base.
    moveSpeed    = s.moveSpeed;
    jumpStrength = s.jumpStrength;
    maxHP        = s.maxHP;
    currentHP    = s.maxHP;   // full HP on character switch

    // Replace the pistol with one tuned to this character's fire rate.
    delete weaponSlots[0];
    weaponSlots[0] = new ProjectileWeapon(
        s.pistolCooldown, 1, -1, 600.0f, false, sf::Color(255, 255, 100), level
    );

    // Load the new sprite. Falls back to a tinted placeholder if the file
    // is missing (the user will drop in real sprites later).
    if (texture.loadFromFile(s.spritePath)) {
        sprite.setTexture(texture, true);
        sprite.setColor(sf::Color::White);
    }
    else {
        std::cout << "Player: missing sprite \"" << s.spritePath
                  << "\" — using tinted placeholder.\n";
        sprite.setColor(s.placeholderTint);
    }

    float scale = PLAYER_H / static_cast<float>(texture.getSize().y);
    entityWidth = static_cast<float>(texture.getSize().x) * scale;
    sprite.setScale(scale, scale);
    setBaseScale(scale, scale);

    std::cout << "Character: " << s.name
              << "  (HP " << maxHP
              << ", speed " << moveSpeed
              << ", pistolCD " << s.pistolCooldown << "s)\n";
}

const char* Player::getCharacterName() const
{
    return CHARACTER_TABLE[static_cast<int>(currentCharacter)].name;
}

Player::~Player()
{
    for (int i = 0; i < SLOT_COUNT; i++) {
        delete weaponSlots[i];
        weaponSlots[i] = nullptr;
    }
    // entities is non-owning — do NOT delete
}

// ── Private helpers ───────────────────────────────────────────────────────────

void Player::equipSlot(int slot)
{
    if (slot < 0 || slot >= SLOT_COUNT) return;
    if (!weaponSlots[slot]) return;
    if (slot != 4) prevSlot = slot; // knife (4) doesn't update prevSlot
    activeSlot = slot;
}

void Player::giveAllWeapons()
{
    // Slot 1: Heavy Machine Gun — fast fire, straight shots
    if (!weaponSlots[1])
        weaponSlots[1] = new ProjectileWeapon(
            0.08f, 1, -1, 700.0f, false, sf::Color(255, 200, 50), level
        );
    // Slot 2: Rocket Launcher — slow, high damage, ballistic arc
    if (!weaponSlots[2])
        weaponSlots[2] = new ProjectileWeapon(
            1.2f, 8, -1, 400.0f, true, sf::Color(255, 100, 50), level
        );
    // Slot 3: Grenade Launcher — ballistic arc, area feel
    if (!weaponSlots[3])
        weaponSlots[3] = new ProjectileWeapon(
            0.8f, 5, -1, 350.0f, true, sf::Color(100, 200, 50), level
        );
    // Slot 4: Knife — melee, 90 px reach, 0.4 s cooldown
    if (!weaponSlots[4])
        weaponSlots[4] = new MeleeWeapon(0.4f, 3, 90.0f);
}

// ── Public interface ──────────────────────────────────────────────────────────

void Player::equipWeapon(Weapon* newWeapon)
{
    delete weaponSlots[0];
    weaponSlots[0] = newWeapon; // takes ownership
    activeSlot = 0;
}

void Player::takeDamage(int amount)
{
    if (devMode) return; // immortal in developer mode
    DamageableEntity::takeDamage(amount);
}

void Player::toggleDevMode()
{
    devMode = !devMode;
    if (devMode) {
        giveAllWeapons();
        std::cout << "[DEV] Developer mode ON — immortal, all weapons unlocked\n"
                  << "      1=Pistol  2=HMG  3=Rocket  4=Grenade  X=Knife\n";
    }
    else {
        activeSlot = 0; // back to pistol when dev mode is off
        std::cout << "[DEV] Developer mode OFF\n";
    }
}

void Player::handleInput()
{
    using sf::Keyboard;
    using sf::Mouse;

    // ── Developer mode toggle ─────────────────────────────────────────────────
    // Static so the flag persists between frames (edge-detect for F1)
    static bool RHeldLastFrame = false;
    bool RHeld = Keyboard::isKeyPressed(Keyboard::R);
    if (RHeld && !RHeldLastFrame)
        toggleDevMode();
    RHeldLastFrame = RHeld;

    // ── Weapon slot selection (dev mode only) ─────────────────────────────────
    if (devMode) {
        if (Keyboard::isKeyPressed(Keyboard::Num1)) equipSlot(0);
        if (Keyboard::isKeyPressed(Keyboard::Num2)) equipSlot(1);
        if (Keyboard::isKeyPressed(Keyboard::Num3)) equipSlot(2);
        if (Keyboard::isKeyPressed(Keyboard::Num4)) equipSlot(3);
    }

    // ── Knife attack (dev mode, X key, edge-detected) ─────────────────────────
    // Knife fires instantly and returns to the previous ranged weapon.
    bool knifeHeld = devMode && Keyboard::isKeyPressed(Keyboard::X);
    if (knifeHeld && !knifeHeldLastFrame && weaponSlots[4] && entities) {
        float swingX = facingRight
            ? positionX + entityWidth
            : positionX;
        float swingY = positionY + entityHeight * 0.5f;
        weaponSlots[4]->fire(swingX, swingY, facingRight, true, *entities);
        equipSlot(prevSlot); // auto-return to ranged weapon
        std::cout << "[DEV] Knife! Returned to slot " << prevSlot << "\n";
    }
    knifeHeldLastFrame = knifeHeld;

    // ── Movement ──────────────────────────────────────────────────────────────
    bool left  = Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left);
    bool right = Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right);
    bool jumpHeld = Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Space);

    if (left && !right)       moveLeft();
    else if (right && !left)  moveRight();
    else                      stopHorizontal();

    if (jumpHeld && !jumpHeldLastFrame) jump();
    jumpHeldLastFrame = jumpHeld;

    // ── Character switch (Z key, edge-detected) ──────────────────────────────
    // Cycles Marco → Tarma → Eri → Fiolina → Marco …
    bool switchHeld = Keyboard::isKeyPressed(Keyboard::Z);
    if (switchHeld && !switchHeldLastFrame) {
        int next = (static_cast<int>(currentCharacter) + 1)
                   % static_cast<int>(CharacterType::COUNT);
        applyCharacter(static_cast<CharacterType>(next));
    }
    switchHeldLastFrame = switchHeld;

    // ── Shooting (LMB only — Z is now reserved for character switching) ─────
    bool fireHeld = Mouse::isButtonPressed(Mouse::Left);

    if (fireHeld && !fireHeldLastFrame) {
        Weapon* w = weaponSlots[activeSlot];
        if (w && entities) {
            float spawnX = facingRight
                ? positionX + entityWidth + 2.0f
                : positionX - 4.0f;
            float spawnY = positionY + entityHeight * 0.45f;
            w->fire(spawnX, spawnY, facingRight, true, *entities);
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

    // Tick weapon cooldowns
    for (int i = 0; i < SLOT_COUNT; i++)
        if (weaponSlots[i]) weaponSlots[i]->update(dt);

    Soldier::update(dt);
}
