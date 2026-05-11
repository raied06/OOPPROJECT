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

void Projectile::applyScreenClamp(float cameraX)
{
    if (positionX + entityWidth < cameraX || positionX > cameraX + 1600.0f)
        deactivateEntity();
}

bool Projectile::dispatchHit(Entity* target)
{
    return target->receiveProjectileHit(damage, fromPlayer);
}

void Projectile::checkEntityCollisions()
{
    if (!entities || !isActive) return;

    int n = entities->getCount();
    for (int i = 0; i < n; i++) {
        Entity* target = entities->getEntity(i);
        if (!target || !target->getIsActive() || target == this) continue;

        if (isOverlapping(target)) {
            // Virtual dispatch — subclasses (e.g. FlameProjectile) route the
            // hit through a different damage channel so things like the mummy's
            // fire-only kill rule can be expressed cleanly.
            bool accepted = dispatchHit(target);
            if (accepted) {
                deactivateEntity();
                return;
            }
        }
    }
}
