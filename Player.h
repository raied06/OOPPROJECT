#pragma once
#include "Soldier.h"

class EntityManager;
class Weapon;

// Player — abstract base for all playable characters.
// Each concrete subclass (MarcoPlayer, TarmaPlayer, etc.) sets its own
// stats in its constructor and implements createNext() / createSelf()
// for runtime-polymorphism-based character switching.
class Player : public Soldier
{
protected:
    // Weapon slots — subclasses populate slot 0 (pistol) in their constructor.
    // 0=Pistol  1=HMG  2=Rocket  3=Grenade  4=Knife  5=Flame  6=Laser
    static constexpr int SLOT_COUNT = 7;
    Weapon* weaponSlots[SLOT_COUNT];

    EntityManager* entities; // non-owning — used to spawn projectiles + knife scan

    // Loads all 5 weapon sprites. Missing sprites fall back to pistol automatically.
    void loadAllWeaponSprites(const char* pistolPath,
                               const char* machineGunPath,
                               const char* riflePath,
                               const char* knifePath,
                               const char* firePath);

    // Helper: replaces slot 0 (called by subclasses to set pistol cooldown).
    void setPistol(float cooldown);

private:
    bool jumpHeldLastFrame;
    bool fireHeldLastFrame;
    bool knifeHeldLastFrame;

    int activeSlot;
    int prevSlot; // restored after knife swing

    bool devMode;  // F1/R toggles; grants immortality + all weapons

    // Per-character weapon sprite textures (loaded by loadAllWeaponSprites)
    sf::Texture texPistol;
    sf::Texture texMachineGun;
    sf::Texture texRifle;
    sf::Texture texKnife;
    sf::Texture texFire;

    float fireSpriteTimer;   // counts down; >0 means show fire sprite
    float knifeSpriteTimer;  // counts down; >0 means show knife sprite

    static constexpr float FIRE_ANIM_DURATION  = 0.15f;
    static constexpr float KNIFE_ANIM_DURATION = 0.35f;

    // Returns the texture to show based on active slot (no timers considered)
    const sf::Texture* texForSlot(int slot) const;
    // Swaps sprite to the correct texture for current state
    void updateActiveSprite();

    void giveAllWeapons();
    void equipSlot(int slot);

public:
    Player(float x, float y, const Level* lvl, EntityManager* em, int hp);
    virtual ~Player();

    Player(const Player&)            = delete;
    Player& operator=(const Player&) = delete;

    void handleInput();
    virtual void update(float dt) override;

    // ── Runtime-polymorphism character switching ──────────────────────────────
    // createNext() returns a NEW heap-allocated player of the next character
    // in the cycle. Called by PlayState — no if/else needed at the call site.
    virtual Player* createNext(float x, float y,
                               const Level* lvl, EntityManager* em) const = 0;
    // createSelf() recreates the same character type (used by respawn).
    virtual Player* createSelf(float x, float y,
                               const Level* lvl, EntityManager* em) const = 0;
    // Returns a fixed int (0-3) identifying the character (used by PlayState
    // to remember which type to respawn without storing a dead pointer).
    virtual int getCharacterIdx() const = 0;

    // Immortality — no-op while devMode is on.
    virtual void takeDamage(int amount) override;

    void toggleDevMode();
    bool isDevMode() const { return devMode; }

    // Pickup: replaces the pistol slot and takes ownership of newWeapon.
    void    equipWeapon(Weapon* newWeapon);
    Weapon* getWeapon() const { return weaponSlots[activeSlot]; }

    virtual void applyScreenClamp(float cameraX) override;
};
