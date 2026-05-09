#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// Zombie  —  infected infantry that transforms the player on touch
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 5
//   Weapon        : Pistol (slow fire, 1 dmg)
//   Movement speed: 80 px/s  (shambling gait)
//   Detection     : 500 px  (attracted to any movement)
//   Attack range  : 240 px  (opens fire from medium range)
//   Contact range : ~60 px  (melee contact damage + transform trigger)
//
// Special mechanic:
//   When a Zombie overlaps the player it deals contact damage every 0.8 s.
//   (TransformationState is wired in once that class is implemented.)
//   Spawned in batches of 3–5 by PlayState.
// ─────────────────────────────────────────────────────────────────────────────
class Zombie : public Enemy
{
private:
    float contactCooldown;   // time left before next melee hit is allowed
    static constexpr float CONTACT_INTERVAL = 0.8f;
    static constexpr int   CONTACT_DAMAGE   = 2;
    static constexpr float CONTACT_RANGE    = 70.0f; // px — centre-to-centre

public:
    Zombie(float x, float y,
           const Level*   lvl,
           Player*        p,
           EntityManager* em);

    virtual ~Zombie() {}

    // Override to add melee contact check on top of normal AI.
    virtual void update(float dt) override;
};
