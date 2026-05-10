#include "EriPlayer.h"
#include "FiolinaPlayer.h"
#include <iostream>

EriPlayer::EriPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 5)
{
    moveSpeed    = 300.0f;
    jumpStrength = -700.0f;
    // Eri weakness: 20% lower fire rate. Base 0.25s ÷ 0.8 = 0.3125s.
    setPistol(0.3125f);
    giveDefaultKnife();          // createKnife() returns nullptr → no melee
    // Eri buff: carries double the grenades. Base 10 × 2 = 20.
    grenadeCount = 20;
    loadAllWeaponSprites(
        "Sprites/Eri_Pistol.png",
        "Sprites/Eri_MachineGun.png",
        "Sprites/Eri_Rifle.png",
        "Sprites/Eri_Knife.png",
        "Sprites/Eri_Fire.png"
    );
    std::cout << "Character: Eri Kasamoto (grenade specialist, no melee)\n";
}

Player* EriPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new FiolinaPlayer(x, y, lvl, em);
}

Player* EriPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new EriPlayer(x, y, lvl, em);
}
