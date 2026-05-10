#include "MarcoPlayer.h"
#include "TarmaPlayer.h"
#include "MeleeWeapon.h"
#include <iostream>

MarcoPlayer::MarcoPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 5)
{
    moveSpeed    = 300.0f;
    jumpStrength = -700.0f;
    // Marco buff: 25% higher fire rate.
    // Base cooldown 0.25s ÷ 1.25 = 0.20s.
    setPistol(0.20f);
    giveDefaultKnife();          // piercing knife — see createKnife() below
    // Marco weakness: 2 less grenades than base 10 = 8.
    grenadeCount = 8;
    loadAllWeaponSprites(
        "Sprites/Marco_Pistol.png",
        "Sprites/Marco_MachineGun.png",
        "Sprites/Marco_Rifle.png",
        "Sprites/Marco_Knife.png",
        "Sprites/Marco_Fire.png"
    );
    std::cout << "Character: Marco Rossi (fast fire + piercing knife)\n";
}

// ── Marco's special: dual fire (only while power-up active) ───────────────────
void MarcoPlayer::onShotFired(float x, float y, bool facingRight)
{
    if (!isPowerUpActive()) return;     // only during the 10s power-up window
    int slot = getActiveSlot();
    if (slot != 0 && slot != 1) return; // pistol and HMG only

    Weapon* w = getWeapon();
    if (!w || !entities) return;

    // Reset cooldown so the backward shot passes the canFire() check,
    // then fire in the opposite direction.
    w->clearCooldown();
    w->fire(x, y, !facingRight, true, *entities);
}

// ── Marco's permanent buff: knife pierces shields ─────────────────────────────
Weapon* MarcoPlayer::createKnife() const
{
    // damage 2 (base), piercing=true → bypasses ShieldedSoldier's shield.
    return new MeleeWeapon(0.5f, 2, 90.0f, 80.0f, true);
}

Player* MarcoPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new TarmaPlayer(x, y, lvl, em);
}

Player* MarcoPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new MarcoPlayer(x, y, lvl, em);
}
