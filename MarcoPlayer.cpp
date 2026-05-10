#include "MarcoPlayer.h"
#include "TarmaPlayer.h"
#include <iostream>

MarcoPlayer::MarcoPlayer(float x, float y, const Level* lvl, EntityManager* em)
    : Player(x, y, lvl, em, 5)
{
    moveSpeed    = 300.0f;
    jumpStrength = -700.0f;
    setPistol(0.35f);
    loadAllWeaponSprites(
        "Sprites/Marco_Pistol.png",
        "Sprites/Marco_MachineGun.png",
        "Sprites/Marco_Rifle.png",
        "Sprites/Marco_Knife.png",
        "Sprites/Marco_Fire.png"
    );
    std::cout << "Character: Marco Rossi\n";
}

Player* MarcoPlayer::createNext(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new TarmaPlayer(x, y, lvl, em);
}

Player* MarcoPlayer::createSelf(float x, float y, const Level* lvl, EntityManager* em) const
{
    return new MarcoPlayer(x, y, lvl, em);
}
