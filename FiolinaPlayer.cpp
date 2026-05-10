#include "FiolinaPlayer.h"
#include "MarcoPlayer.h"
#include "MeleeWeapon.h"
#include "ProjectileWeapon.h"
#include <iostream>

FiolinaPlayer::FiolinaPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 4) // faster but fragile
{
    moveSpeed    = 360.0f;
    jumpStrength = -760.0f;
    // Fiolina buff: 10% higher fire rate. Base 0.25s ÷ 1.1 ≈ 0.227s.
    setPistol(0.227f);
    giveDefaultKnife();          // 25%-weaker knife — see createKnife() below
    // Fiolina weakness: 2 less grenades than base 10 = 8.
    grenadeCount = 8;
    loadAllWeaponSprites(
        "Sprites/Fiolina_Pistol.png",
        "Sprites/Fiolina_MachineGun.png",
        "Sprites/Fiolina_Rifle.png",
        "Sprites/Fiolina_Knife.png",
        "Sprites/Fiolina_Fire.png"
    );
    std::cout << "Character: Fiolina Germi (weapon master + SUPERCHARGED)\n";
}

// ── Fiolina's knife: 25% less damage than base 2 → 1 HP ───────────────────────
Weapon* FiolinaPlayer::createKnife() const
{
    return new MeleeWeapon(0.5f, 1, 90.0f);
}

// ── SUPERCHARGED power-up: halve cooldown after every shot ────────────────────
void FiolinaPlayer::onShotFired(float /*x*/, float /*y*/, bool /*facingRight*/)
{
    if (!isPowerUpActive()) return;
    Weapon* w = getWeapon();
    if (w) w->halveCooldown();
}

// ── Weapon pickup buff: 50% more ammo than the base loadout ──────────────────
void FiolinaPlayer::equipWeapon(Weapon* newWeapon)
{
    // Run the base equip first so the slot is replaced and ownership transfers.
    Player::equipWeapon(newWeapon);
    // If it's a projectile weapon with finite ammo, top it up by 50%.
    auto* pw = dynamic_cast<ProjectileWeapon*>(newWeapon);
    if (pw && pw->getAmmo() > 0)
        pw->addAmmo(pw->getAmmo() / 2);
}

Player* FiolinaPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new MarcoPlayer(x, y, lvl, em); // wraps back to Marco
}

Player* FiolinaPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new FiolinaPlayer(x, y, lvl, em);
}
