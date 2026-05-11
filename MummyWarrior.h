#pragma once
#include "Enemy.h"

// ═════════════════════════════════════════════════════════════════════════════
// MummyWarrior  —  undead melee enemy that crumbles and resurrects
// ═════════════════════════════════════════════════════════════════════════════
// Stats:
//   HP            : 5  (per resurrection cycle)
//   Weapon        : None — melee contact only
//   Movement speed: 70 px/s  (slow shamble)
//   Detection     : 480 px
//   Contact range : ~65 px  (deals damage + transform on overlap)
//
// Resurrection mechanic:
//   When HP reaches 0 the mummy "crumbles" (stays at position, stops moving,
//   becomes invincible) for RESURRECTION_DELAY seconds, then rises with full HP.
//   This can happen MAX_RESURRECTIONS times; the third death is permanent.
//
// (TransformationState on the player is a TODO — contact currently deals 2 HP.)
// ─────────────────────────────────────────────────────────────────────────────
class MummyWarrior : public Enemy
{
private:
    float resurrectionTimer;   // counts down; > 0 means crumbling
    int   resurrectionCount;   // how many times it has already risen

    float contactCooldown;

    static constexpr float RESURRECTION_DELAY = 3.0f;
    static constexpr int   MAX_RESURRECTIONS  = 2;   // rises twice; third death is final
    static constexpr float CONTACT_INTERVAL   = 0.8f;
    static constexpr int   CONTACT_DAMAGE     = 2;
    static constexpr float CONTACT_RANGE      = 65.0f;

public:
    MummyWarrior(float x, float y,
                 const Level*   lvl,
                 Player*        p,
                 EntityManager* em);

    virtual ~MummyWarrior() {}

    // Intercepts death to trigger resurrection instead of deactivation.
    virtual void takeDamage(int amount) override;

    // Mummy can ONLY be permanently killed by fire or explosions.
    // Both bypass the resurrection cycle.
    virtual bool receiveFireHit(int damage, bool fromPlayer) override;
    virtual bool receiveExplosionHit(int damage, bool fromPlayer) override;

    // Ticks the resurrection timer and handles melee contact.
    virtual void update(float dt) override;

    int getScoreValue() const override { return 400; }
};
