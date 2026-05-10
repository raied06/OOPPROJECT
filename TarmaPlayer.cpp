#include "TarmaPlayer.h"
#include "EriPlayer.h"
#include <iostream>

TarmaPlayer::TarmaPlayer(float x, float y, const Level* lvl, EntityManager* em)
    // Tarma weakness: 20% less HP on foot. Base 5 × 0.8 = 4.
    : Player(x, y, lvl, em, 4)
{
    // Tarma weakness: 20% slower on foot. Base 300 × 0.8 = 240.
    moveSpeed    = 240.0f;
    jumpStrength = -650.0f;
    // Base pistol fire rate on foot (no buff/weakness): 0.25s = 4 shots/sec.
    setPistol(0.25f);
    giveDefaultKnife();
    // Base grenade count: 10.
    grenadeCount = 10;
    loadAllWeaponSprites(
        "Sprites/Tarma_Pistol.png",
        "Sprites/Tarma_MachineGun.png",
        "Sprites/Tarma_Rifle.png",
        "Sprites/Tarma_Knife.png",
        "Sprites/Tarma_Fire.png"
    );
    std::cout << "Character: Tarma Roving (slower + frail on foot, 20s immunity)\n";
}

// ── Tarma's special: full damage immunity during 20s power-up ─────────────────
void TarmaPlayer::takeDamage(int amount)
{
    if (isPowerUpActive()) {
        // Totally immune while power-up is active.
        return;
    }
    Player::takeDamage(amount);
}

Player* TarmaPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new EriPlayer(x, y, lvl, em);
}

Player* TarmaPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new TarmaPlayer(x, y, lvl, em);
}
