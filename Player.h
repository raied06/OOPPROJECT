#pragma once
#include "Soldier.h"

class EntityManager;
class Weapon;

// ── Character identity ────────────────────────────────────────────────────────
// The four playable Metal Slug characters. COUNT is sentinel for cycling.
enum class CharacterType { Marco, Tarma, Eri, Fiolina, COUNT };

// Stat block for one playable character — table lives in Player.cpp.
struct CharacterStats
{
    const char* name;
    const char* spritePath;
    int         maxHP;
    float       moveSpeed;       // px/s
    float       jumpStrength;    // negative = upward
    float       pistolCooldown;  // seconds between pistol shots
    sf::Color   placeholderTint; // sprite tint shown when sprite file is missing
};

class Player : public Soldier
{
private:
    bool jumpHeldLastFrame;
    bool fireHeldLastFrame;
    bool knifeHeldLastFrame;   // edge-detect for X key knife attack
    bool switchHeldLastFrame;  // edge-detect for Z key character switch

    // ── Character state ──────────────────────────────────────────────────────
    CharacterType currentCharacter;
    static const CharacterStats CHARACTER_TABLE[
        static_cast<int>(CharacterType::COUNT)];

    // Loads a character's stats and sprite onto this Player instance.
    void applyCharacter(CharacterType c);

    // ── Weapon slots ─────────────────────────────────────────────────────────
    // 0 = Pistol   (always available)
    // 1 = HMG      (dev mode)
    // 2 = Rocket   (dev mode)
    // 3 = Grenade  (dev mode)
    // 4 = Knife    (dev mode — melee, instant-return after swing)
    static constexpr int SLOT_COUNT = 5;
    Weapon* weaponSlots[SLOT_COUNT]; // OWNED
    int activeSlot;
    int prevSlot; // slot to restore after a knife swing

    // ── Developer mode ────────────────────────────────────────────────────────
    bool devMode; // F1 toggles; grants immortality + all weapons

    EntityManager* entities; // non-owning — only used to spawn projectiles

    // Creates weapon instances for slots 1-4 (called when dev mode is enabled).
    void giveAllWeapons();

    // Sets activeSlot; remembers previous slot so knife can return to it.
    void equipSlot(int slot);

public:
    Player(float x, float y, const Level* lvl, EntityManager* em, int hp = 5);
    virtual ~Player();

    Player(const Player&)            = delete;
    Player& operator=(const Player&) = delete;

    void handleInput();
    virtual void update(float dt) override;

    // Active character info — used by HUD / debug printouts.
    CharacterType getCharacter()     const { return currentCharacter; }
    const char*   getCharacterName() const;

    // Swap the pistol slot on pickup — Player takes ownership of newWeapon.
    void    equipWeapon(Weapon* newWeapon);
    Weapon* getWeapon() const { return weaponSlots[activeSlot]; }

    // Immortality — takeDamage is a no-op while devMode is on.
    virtual void takeDamage(int amount) override;

    void toggleDevMode();
    bool isDevMode() const { return devMode; }

    virtual void applyScreenClamp(float cameraX) override;
};
