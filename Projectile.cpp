#include "Projectile.h"

Projectile::Projectile(float x, float y, float w, float h,
                       int damage, bool fromPlayer, const Level* lvl)
    : Entity(x, y, w, h),
      damage(damage),
      fromPlayer(fromPlayer),
      level(lvl)
{}
