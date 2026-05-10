#include "TarmaPlayer.h"
#include "EriPlayer.h"
#include <iostream>

TarmaPlayer::TarmaPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 6)  // tankier than Marco
{
    moveSpeed    = 260.0f;       // slower but tougher
    jumpStrength = -650.0f;
    setPistol(0.40f);
    loadAllWeaponSprites(
        "Sprites/Tarma_Pistol.png",
        "Sprites/Tarma_MachineGun.png",
        "Sprites/Tarma_Rifle.png",
        "Sprites/Tarma_Knife.png",
        "Sprites/Tarma_Fire.png"
    );
    std::cout << "Character: Tarma Roving\n";
}

Player* TarmaPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new EriPlayer(x, y, lvl, em);
}

Player* TarmaPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new TarmaPlayer(x, y, lvl, em);
}
