#include "LaserWeapon.h"
#include "LaserBeam.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Level.h"

LaserWeapon::LaserWeapon(float cooldown, int damage, const Level* lvl, float maxRange)
    : Weapon(cooldown), damage(damage), maxRange(maxRange), level(lvl)
{}

void LaserWeapon::fire(float x, float y, bool facingRight, bool fromPlayer,
                        EntityManager& em)
{
    if (!canFire()) return;
    cooldownTimer = cooldown;

    // ── Find beam length (first solid tile in facing direction) ───────────────
    float beamLen = maxRange;
    float step    = 8.0f; // scan step in pixels
    float scanX   = x;

    for (float d = step; d <= maxRange; d += step) {
        scanX = facingRight ? x + d : x - d;
        if (level->isSolidAtPixel(scanX, y)) {
            beamLen = d;
            break;
        }
    }

    // ── Instant-hit: damage every entity inside the beam rectangle ────────────
    float beamLeft  = facingRight ? x          : x - beamLen;
    float beamRight = facingRight ? x + beamLen : x;
    float vRange    = 30.0f; // half-height tolerance

    int n = em.getCount();
    for (int i = 0; i < n; i++) {
        Entity* e = em.getEntity(i);
        if (!e || !e->getIsActive()) continue;

        float ex = e->getPosX() + e->getEntityWidth()  * 0.5f;
        float ey = e->getPosY() + e->getEntityHeight() * 0.5f;

        bool inX = (ex >= beamLeft && ex <= beamRight);
        bool inY = (ey >= y - vRange && ey <= y + vRange);
        if (inX && inY)
            e->receiveProjectileHit(9999, fromPlayer);
    }

    // ── Spawn visual beam ─────────────────────────────────────────────────────
    em.add(new LaserBeam(x, y, beamLen, facingRight));
}
