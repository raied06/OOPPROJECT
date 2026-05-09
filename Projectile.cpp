#include "Projectile.h"
#include "EntityManager.h"

Projectile::Projectile(float x, float y, float w, float h,
                       int damage, bool fromPlayer,
                       const Level* lvl, EntityManager* em)
    : Entity(x, y, w, h),
      damage(damage),
      fromPlayer(fromPlayer),
      level(lvl),
      entities(em)
{}

void Projectile::checkEntityCollisions()
{
    if (!entities || !isActive) return;

    int n = entities->getCount();
    for (int i = 0; i < n; i++) {
        Entity* target = entities->getEntity(i);
        if (!target || !target->getIsActive() || target == this) continue;

        if (isOverlapping(target)) {
            // receiveProjectileHit() is virtual — Enemy accepts player bullets,
            // Soldier/Player accepts enemy bullets, everything else returns false.
            bool accepted = target->receiveProjectileHit(damage, fromPlayer);
            if (accepted) {
                deactivateEntity();
                return;
            }
        }
    }
}
