#include "FiolinaPlayer.h"
#include "MarcoPlayer.h"
#include <iostream>

FiolinaPlayer::FiolinaPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 4)  // faster but fragile
{
    moveSpeed    = 360.0f;
    jumpStrength = -760.0f;      // highest jump
    setPistol(0.28f);            // fastest pistol fire rate
    loadAllWeaponSprites(
        "Sprites/Fiolina_Pistol.png",
        "Sprites/Fiolina_MachineGun.png",
        "Sprites/Fiolina_Rifle.png",   // no Rifle sprite — falls back to pistol
        "Sprites/Fiolina_Knife.png",   // no Knife sprite — falls back to pistol
        "Sprites/Fiolina_Fire.png"
    );
    std::cout << "Character: Fiolina Germi\n";
}

Player* FiolinaPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new MarcoPlayer(x, y, lvl, em); // wraps back to Marco
}

Player* FiolinaPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new FiolinaPlayer(x, y, lvl, em);
}
