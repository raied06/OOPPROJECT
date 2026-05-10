#include "EriPlayer.h"
#include "FiolinaPlayer.h"
#include <iostream>

EriPlayer::EriPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 5)
{
    moveSpeed    = 300.0f;
    jumpStrength = -700.0f;
    setPistol(0.35f);
    loadAllWeaponSprites(
        "Sprites/Eri_Pistol.png",
        "Sprites/Eri_MachineGun.png",
        "Sprites/Eri_Rifle.png",
        "Sprites/Eri_Knife.png",
        "Sprites/Eri_Fire.png"
    );
    std::cout << "Character: Eri Kasamoto\n";
}

Player* EriPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new FiolinaPlayer(x, y, lvl, em);
}

Player* EriPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new EriPlayer(x, y, lvl, em);
}
