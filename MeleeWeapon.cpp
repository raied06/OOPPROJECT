#include "MeleeWeapon.h"
#include "EntityManager.h"
#include "Entity.h"

MeleeWeapon::MeleeWeapon(float cooldown, int damage, float range, float vRange)
    : Weapon(cooldown), damage(damage), range(range), vRange(vRange)
{}

void MeleeWeapon::fire(float x, float y, bool facingRight, bool fromPlayer,
                       EntityManager& em)
{
    if (!canFire()) return;
    cooldownTimer = cooldown;

    for (int i = 0; i < em.getCount(); i++) {
        Entity* e = em.getEntity(i);
        if (!e || !e->getIsActive()) continue;

        // Use target centre for a fairer hit box
        float ex = e->getPosX() + e->getEntityWidth()  * 0.5f;
        float ey = e->getPosY() + e->getEntityHeight() * 0.5f;

        float dx = ex - x;
        float dy = ey - y;

        // Must be on the facing side and within horizontal range
        if ( facingRight && (dx < 0.0f || dx > range))  continue;
        if (!facingRight && (dx > 0.0f || -dx > range)) continue;
        // Must be within the vertical swing arc
        if (dy < -vRange || dy > vRange)                 continue;

        e->receiveProjectileHit(damage, fromPlayer);
    }
}
