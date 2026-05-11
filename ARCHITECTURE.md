# Metal Slug OOP — Architecture & Code Walkthrough

> A complete tour of every file in the project, what it does, and how it links to the rest.
>
> Read it top-down for the big picture; jump to any section by ctrl-F.

---

## 1. Overview

The project is a side-scrolling action game in the style of Metal Slug, built in C++20 with SFML 2.6.1. It runs at 1600×900, 60 FPS, on a single thread.

- **State machine** at the top: a `Game` object owns one `GameStateManager`; the manager holds a stack of `GameState*`. The first state is `MenuState`; pressing `1` pushes `PlayState`.
- **Entities** are everything that moves, shoots, takes damage, or lives in the world. They share a common abstract base `Entity` and are owned by an `EntityManager`.
- **Three parallel inheritance trees** sprout from `Entity`: living things (Soldier/Player and Enemy), projectiles, and short-lived effects (Explosion, LaserBeam).
- **All collision and damage dispatch is virtual** — no `dynamic_cast`, no `if/else` chains on type. Each entity decides for itself what to do with an incoming hit.

### 1.1 Class hierarchy at a glance

```
Entity (abstract)
│
├── DamageableEntity (abstract)   ── HP + invincibility + flash
│   │
│   ├── Soldier (abstract)        ── gravity, jump, tile collision
│   │   └── Player (abstract)     ── input, weapon slots, grenades, power-up
│   │       ├── MarcoPlayer       ── dual fire, piercing knife
│   │       ├── TarmaPlayer       ── 20s damage immunity
│   │       ├── EriPlayer         ── double grenades, no melee
│   │       └── FiolinaPlayer     ── auto-fire, +50% ammo on pickup
│   │
│   └── Enemy (abstract)          ── AI state, weapon, contextual pointers
│       ├── RebelSoldier          ── 2 HP, pistol
│       ├── ShieldedSoldier       ── 5 HP, frontal shield
│       ├── BazookaSoldier        ── 2 HP, rocket launcher
│       ├── GrenadeSoldier        ── 2 HP, ballistic grenades
│       ├── Paratrooper           ── 2 HP, drops with parachute
│       ├── Zombie                ── 5 HP, contact damage
│       ├── MummyWarrior          ── 5 HP × 3 lives (resurrects)
│       └── Martian               ── 3 HP, two-phase (fly → ground)
│
├── Projectile (abstract)         ── damage, fromPlayer flag, collision dispatch
│   ├── StraightProjectile        ── constant velocity (pistol, HMG)
│   ├── BallisticProjectile       ── gravity arc, area explosion (rockets, grenades)
│   └── FlameProjectile           ── range-limited fire (FlameWeapon)
│
├── Explosion                     ── area-damage burst, lives ~0.3 s
└── LaserBeam                     ── purely visual, fades out

EnemyAIState (abstract)           ── State Pattern for AI
├── PatrolState
├── ChaseState
├── ParachuteState
└── AttackState

Weapon (abstract)                 ── cooldown bookkeeping + virtual fire()
├── ProjectileWeapon              ── pistol, HMG, rocket, grenade
├── MeleeWeapon                   ── knife (overlap scan, no projectile)
├── FlameWeapon                   ── stream of FlameProjectiles
└── LaserWeapon                   ── instant beam, spawns LaserBeam
```

### 1.2 OOP patterns used

| Pattern | Where to find it |
|---|---|
| **Inheritance** | Every tree above |
| **Polymorphism** | `Entity::update`, `Entity::receiveProjectileHit`, `Player::createNext`, `Weapon::fire` |
| **Pure virtual / abstract classes** | `Entity`, `DamageableEntity`, `Soldier`, `Player`, `Enemy`, `Weapon`, `Projectile`, `EnemyAIState` |
| **Composition** | `Enemy` owns `EnemyAIState*` + `Weapon*`; `Player` owns 7 `Weapon*` slots; `PlayState` owns `Level*` + `EntityManager*` |
| **Aggregation** | `Soldier`/`Enemy` aggregate `Level*` (read-only, not owned); `Enemy` aggregates `Player*` and `EntityManager*` |
| **State Pattern** | `EnemyAIState` → Patrol/Chase/Attack/Parachute |
| **Factory Method** | `Player::createNext` / `createSelf` — used for character switching and respawn without `switch`/`if-chain` |
| **Template Method (informal)** | `Enemy::update` ticks AI then runs physics; subclasses extend |
| **Rule of Three/Five** | `Level`, `EntityManager`, `Enemy`, `Player` all `delete` copy ctor/assignment (own raw memory) |
| **Virtual destructors** | Every abstract base — prevents leaks via base pointer |

### 1.3 Header restriction

A hard rule on this project: the only non-SFML standard header allowed in `.h` files is `<iostream>`. No `<string>`, `<vector>`, `<cstdio>`, `<sstream>`, etc. This is why:

- All dynamic-size buffers are raw arrays (`EntityManager::entities`, `Level::lvl`, `Player::weaponSlots[7]`)
- All string handling uses `sf::String` (concatenation) and a hand-written `intToBuf` helper in `PlayState.cpp` for integer formatting

---

## 2. Entry Point & Game Loop

### `main.cpp` — 7 lines
```cpp
int main() {
    Game engine;
    engine.run();
    return 0;
}
```
Just constructs `Game` and starts it. All real logic is delegated.

### `Game.h` / `Game.cpp` — the application shell

Owns:
- `sf::RenderWindow window` — the SFML window (1600×900, vsync on, framerate cap 60)
- `GameStateManager stateManager` — stack of active game states
- `sf::Clock clock` — delta-time source

`Game::Game()` creates the window and pushes the initial `MenuState`.

`Game::run()` is the **canonical SFML game loop**:
```
while window open:
    dt = clock.restart()
    poll all events → currentState->handleInput(event, window)
    currentState->update(dt)
    window.clear()
    currentState->render(window)
    window.display()
```
This loop is unchanged from textbook SFML — all interesting work happens inside the current state.

### `GameState.h` — the interface

```cpp
class GameState {
public:
    virtual ~GameState() {}
    virtual void handleInput(sf::Event&, sf::RenderWindow&) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow&) = 0;
};
```
Three pure virtual methods, virtual destructor. Anything that wants to be a screen (menu, gameplay, game-over) implements this.

### `GameStateManager.h` / `.cpp` — stack of states

Internally a **raw growable array** of `GameState*` (no `std::stack`). Tracks `topIndex`.
- `pushState(GameState*)` — takes ownership, grows the array if needed
- `popState()` — `delete`s the top state
- `getCurrentState()` — peek (returns top pointer, no ownership transfer)
- Destructor deletes every remaining state and the array

### `MenuState.h` / `.cpp` — the start screen

Holds a reference to the manager (so it can push `PlayState` when the user picks a mode) and a reference to the window (so it can close on the "campaign" option which isn't implemented yet).

Behaviour:
- Renders `Sprites/Starting Menu.png` scaled to fill the window
- Key `1` → pushes a new `PlayState` (survival)
- Key `2` → sets a 2.5s `closeTimer`, shows "THIS MODE DOES NOT EXIST YET" in big red text, then closes the window
- Loads `Sprites/hud.ttf` for the message — same fallback chain as the HUD

---

## 3. Entity System

### `Entity.h` / `Entity.cpp` — the root of everything

Common state every visible thing has:
```cpp
protected:
    float positionX, positionY;     // top-left world coords
    float entityWidth, entityHeight;
    float velocityX, velocityY;
    bool  isActive;                 // false → will be deleted by removeDead()
```

Pure virtual:
- `update(float dt)` — advances state this frame
- `render(sf::RenderWindow&, float cameraX, float cameraY)` — draws to the window

Concrete virtual (default no-ops, subclasses opt in):
- `receiveProjectileHit(int damage, bool fromPlayer)` → returns `true` if the hit landed. **This is the heart of collision dispatch** — every projectile asks every entity if the hit was accepted.
- `receiveMeleeHit(int, bool)` — defaults to routing through `receiveProjectileHit`; `ShieldedSoldier` overrides separately so Marco's piercing knife can bypass the shield
- `receiveFireHit(int, bool)` — defaults same as projectile; `MummyWarrior` overrides because fire is one of the only things that kills a mummy permanently
- `receiveExplosionHit(int, bool)` — same idea, mummy override
- `onPlayerRespawn(Player*)` — called on every entity when the player changes; `Enemy` overrides to retarget its AI
- `applyScreenClamp(float cameraX)` — `Enemy` overrides to stop walking off-screen; `Projectile` overrides to despawn when off-screen
- `getScoreValue() const` — returns 0 by default; each `Enemy` subclass returns its kill value

Helper:
- `isOverlapping(const Entity*) const` — AABB intersection test

### `DamageableEntity.h` / `.cpp`

Sits between `Entity` and {Soldier, Enemy}. Adds:
- `int maxHP, currentHP`
- `float invincibilityTimer` (1 second after a hit)
- `float flashTimer` (drives a 0.08 s red-tint flicker during invincibility)

Key methods:
- `takeDamage(int)` (virtual) — does nothing if currently invincible; otherwise subtracts HP, resets `invincibilityTimer`, and calls `deactivateEntity()` at 0 HP
- `updateDamageTimers(float dt)` (protected) — every subclass calls this at the top of its own `update()`
- `applyDamageFlash(sf::Sprite&)` (protected) — tints the sprite red during the flicker phase

Subclasses override `takeDamage` to add behaviour (e.g. `Player::takeDamage` becomes a no-op during dev mode; `MummyWarrior::takeDamage` triggers resurrection).

### `Soldier.h` / `Soldier.cpp`

Adds **physics** for anything that walks:
- Gravity, max fall speed, jump strength
- `onGround` flag, `facingRight` flag
- `resolveHorizontal()` — checks left/right walls via `Level`, snaps to wall on collision
- `resolveVertical(float dt)` — gravity, ground detection, ceiling detection (axis-separated collision)
- `moveLeft()`, `moveRight()`, `stopHorizontal()`, `jump()`
- Owns `sf::Texture texture` and `sf::Sprite sprite` for rendering

Also overrides `receiveProjectileHit`:
```cpp
if (fromPlayer) return false;   // friendly fire — ignore
takeDamage(damage);
return true;
```
This means **Player (which inherits Soldier) only takes damage from enemy bullets**, never its own.

### `Entitymanager.h` / `.cpp`

Owns every entity. Raw growable `Entity** entities` array — manual doubling when full.

- `add(Entity*)` — takes ownership; caller must not delete afterwards
- `updateAll(float dt)` — calls `update(dt)` on each active entity
- `renderAll(window, camX, camY)` — calls `render` on each active entity
- `removeDead()` — sweeps the array, `delete`s any entity where `isActive == false`, compacts the survivors

Destructor deletes every remaining entity plus the array itself. Copy ctor / assignment are `= delete`d — double-free is impossible.

---

## 4. Player System

### `Player.h` / `Player.cpp` — abstract base

Inherits `Soldier`. **Cannot be instantiated** because of two pure virtual factory methods:
```cpp
virtual Player* createNext(...) const = 0;   // → next character in the cycle
virtual Player* createSelf(...) const = 0;   // → same character (for respawn)
virtual int     getCharacterIdx() const = 0; // 0..3
```
Each concrete subclass returns a heap-allocated instance — this is how character switching avoids any `switch`/`if` chain in `PlayState`.

#### Weapon slots
```cpp
static constexpr int SLOT_COUNT = 7;
Weapon* weaponSlots[SLOT_COUNT];
// 0=Pistol  1=HMG  2=Rocket  3=Grenade  4=Knife  5=Flame  6=Laser
```
Slot 0 (pistol) is always populated by the subclass constructor via `setPistol(cooldown)`. Slot 4 is filled by `giveDefaultKnife()` which calls the virtual `createKnife()` (so each character can return their own knife variant or `nullptr` for Eri). Slots 1, 2, 3, 5, 6 are populated by `giveAllWeapons()` when dev mode is toggled.

#### Per-character virtuals (the "trait" hooks)
```cpp
virtual int   startingGrenades()      const { return 10;     }
virtual float grenadeBlastRadius()    const { return 120.f;  }
virtual float powerUpDuration()       const { return 10.0f;  }
virtual bool  autoFireOnHold()        const { return false;  }
virtual int   grenadesPerThrow()      const { return 1;      }
virtual Weapon* createKnife()         const { return new MeleeWeapon(0.5f, 2, 90.0f); }
virtual void  onShotFired(float, float, bool) {}
virtual void  onPowerUpActivated()    {}
```
Override the ones that differ; the rest take base values. This is **textbook polymorphism** — the base class shape doesn't change.

#### Throwable grenade
`throwGrenade(float vxMultiplier)` spawns a `BallisticProjectile` with a 20-damage explosion. Eri's power-up overrides `grenadesPerThrow()` to return 2, so each press launches an extra one.

#### Power-up
- Activated by Q key, lasts `powerUpDuration()` seconds (10 base, 20 for Tarma)
- Marco: dual fire (back-shot after each forward shot)
- Tarma: damage immunity
- Eri: doubled grenades per throw
- Fiolina: auto-fire on hold + cooldown halved after every shot

#### Dev mode (R key)
Grants immortality (`takeDamage` early returns) and populates all 7 weapon slots. Numbered 1–4 keys swap slots; X swings the knife with auto-return to the previous slot.

#### Input layout
```
A/Left   move left          W/Space  jump
D/Right  move right         Z        cycle character
LMB      fire active weapon X        knife (dev only)
G        throw grenade      R        toggle dev mode
Q        activate power-up  1-4      pick weapon (dev only)
```

### The four concrete characters

| File | Speed | HP | Pistol CD | Grenades | Special trait |
|---|---|---|---|---|---|
| `MarcoPlayer` | 300 | 5 | 0.20 s (+25 %) | 8 | Piercing knife; **power-up = dual fire** (forward + back) |
| `TarmaPlayer` | 240 (−20 %) | 4 (−20 %) | 0.25 s | 10 | **Power-up = 20 s damage immunity** |
| `EriPlayer` | 300 | 5 | 0.3125 s (−20 %) | 20 (×2) | No knife; +50 % grenade blast radius; **power-up = double grenades per throw** |
| `FiolinaPlayer` | 360 | 4 | 0.227 s (+10 %) | 8 | 25 % weaker knife; +50 % ammo on weapon pickup; **power-up = auto-fire + halved CD per shot** |

Each subclass `createNext()` returns the next in the cycle: Marco → Tarma → Eri → Fiolina → Marco.

### Character switch flow (`PlayState::handleInput`)
1. Player is alive, presses Z
2. `Player* next = player->createNext(currentX, currentY, level, entities)` — pure virtual factory
3. `respawnPrototype` is updated to a fresh `next->createSelf(...)` so respawn recreates the new character on death
4. Old player marked inactive, new one added to manager and pointed at by `player`
5. Every entity is sent `onPlayerRespawn(player)` so enemy AI retargets

---

## 5. Weapon System

### `Weapon.h` / `Weapon.cpp` — abstract base

- `float cooldown, cooldownTimer`
- `void update(float dt)` — ticks `cooldownTimer` down
- `bool canFire() const` — returns `cooldownTimer <= 0`
- `void clearCooldown()` — used by Marco's dual fire
- `void halveCooldown()` — used by Fiolina's supercharged power-up
- `virtual void fire(x, y, facingRight, fromPlayer, em) = 0`

Non-copyable (subclasses own heap resources).

### `ProjectileWeapon.h` / `.cpp` — the workhorse

One class covers pistol, HMG, rocket, grenade. Parametrised by:
- `damage`, `ammo` (`-1` = infinite), `projectileSpeed`
- `gravityAffected` — `false` → spawns `StraightProjectile`, `true` → spawns `BallisticProjectile`
- `projectileColor` — passes through to the projectile
- `explosionRadius` — for rocket/grenade (0 for bullets)

`fire()` builds the right projectile type with the right velocity components, adds it to the EntityManager, then resets the cooldown.

### `MeleeWeapon.h` / `.cpp` — no projectile

Knife. `fire()` doesn't spawn anything — it does an immediate overlap scan:
- Builds an AABB in front of the player based on `facingRight`, `range`, `vRange`
- Iterates `em` entities, calls `target->receiveMeleeHit(damage, true)` on each one inside the box
- Stops at the first accepted hit (or continues if `piercing == true` for Marco)

### `FlameWeapon.h` / `.cpp`

Fast cooldown (designed for held trigger). `fire()` spawns one `FlameProjectile` per call with a short max range.

### `LaserWeapon.h` / `.cpp`

`fire()` is **instant raycast**:
1. Step in the firing direction one cell at a time, asking `Level::isSolidAtPixel` — find where the beam terminates
2. Sweep along that line: any entity that overlaps takes damage immediately
3. Spawn a `LaserBeam` (visual only — fades over ~0.15 s)

---

## 6. Projectile System

### `Projectile.h` / `.cpp` — abstract base

Inherits `Entity` directly (projectiles can't be damaged, only deal damage).
- `int damage`, `bool fromPlayer`
- `const Level* level`, `EntityManager* entities` — non-owning context pointers

The critical method:
```cpp
void Projectile::checkEntityCollisions() {
    for each entity in manager:
        if isOverlapping(target):
            if dispatchHit(target):     // virtual!
                deactivateEntity();
                return;
}
```

`dispatchHit()` is virtual — base implementation calls `target->receiveProjectileHit(damage, fromPlayer)`. `FlameProjectile` overrides it to call `receiveFireHit` instead, so the mummy's fire-only-kill rule comes out for free via virtual dispatch on **both** sides (projectile and target).

**This is the P1 penalty fix.** No `dynamic_cast`, no type-checking — every projectile is dispatched via the entity's own `receiveXxxHit` method.

`applyScreenClamp(cameraX)` deactivates the projectile if it flies off the visible screen, preventing endless flight across the world.

### `StraightProjectile`

Constant velocity. Used by pistol, HMG, enemy rifles. Each frame:
1. Move by velocity × dt
2. World-bounds cull (off-map → deactivate)
3. Tile collision at the leading edge (`level->isSolidAtPixel`) → deactivate
4. `checkEntityCollisions()` → deactivate on any accepted hit

### `BallisticProjectile`

Arcing trajectory. Used by rockets, hand grenades, bazooka shots.
- Own gravity + max fall speed (lighter than player gravity)
- On tile hit: spawn an `Explosion` of size `explosionRadius` (passed through from the weapon), then deactivate
- On entity hit (via `checkEntityCollisions`): still spawn the explosion if `explosionRadius > 0`

### `FlameProjectile`

Tracks distance travelled; deactivates once it exceeds `maxRange`. Overrides `dispatchHit` to route damage through `receiveFireHit` so mummies die permanently to fire.

### `Explosion.h` / `.cpp`

Not a projectile — inherits `Entity` directly. Spawned by ballistic impacts.
- On the first update frame: iterates every entity, calls `receiveExplosionHit` on any within `radius`
- Lives for `lifetime` seconds for the visual blast sprite
- Deactivates afterwards
- `receiveProjectileHit` returns `false` so bullets fly through explosions harmlessly

### `LaserBeam.h` / `.cpp`

Purely visual. Inherits `Entity`. Renders a glowing rectangle for ~0.15 s then deactivates. Damage was already applied by `LaserWeapon::fire`.

---

## 7. Enemy System

### `Enemy.h` / `Enemy.cpp` — abstract base

Inherits `DamageableEntity`. Has its own physics implementation (mirror of `Soldier`, kept separate per UML so neither tree depends on the other).

State:
- `const Level* level` (read-only, not owned)
- `Player* player` (AI target — non-owned, updated by `onPlayerRespawn`)
- `EntityManager* entities` (for spawning projectiles from the weapon — non-owned)
- `EnemyAIState* currentState` (OWNED — composition)
- `Weapon* weapon` (OWNED — composition)
- `float detectionRange, attackRange` — AI tuning, set by each subclass

`Enemy::update(float dt)`:
1. `updateDamageTimers(dt)` — invincibility + flash
2. `weapon->update(dt)` — cooldown tick
3. `currentState->update(*this, dt)` — AI tick; may return a new state pointer
4. If new state returned, `transitionTo(newState)` — calls old state's `exit`, deletes it, swaps, calls new state's `enter`
5. Apply gravity, horizontal move, `resolveHorizontal`, vertical move, `resolveVertical`

`Enemy::receiveProjectileHit`:
```cpp
if (!fromPlayer) return false;    // enemies ignore other enemies' fire
takeDamage(damage);
return true;
```

Helper methods the AI states call: `moveLeft()`, `moveRight()`, `moveTowardPlayer()`, `stopMoving()`, `fireWeapon()`, `distanceToPlayer()`.

### `EnemyAIState.h` / `.cpp` — the State Pattern

Pure virtual base:
```cpp
class EnemyAIState {
    virtual void enter(Enemy&) = 0;
    virtual EnemyAIState* update(Enemy&, float dt) = 0; // returns next state or nullptr
    virtual void exit(Enemy&) = 0;
};
```

**`PatrolState`** — walks back and forth on a 4-second timer. If `distanceToPlayer < detectionRange`, returns `new ChaseState`.

**`ChaseState`** — calls `enemy.moveTowardPlayer()` every frame. If player gets within `attackRange`, returns `new AttackState`. If player leaves `detectionRange × 1.3` (hysteresis to prevent ping-ponging), returns `new PatrolState`.

**`AttackState`** — stops moving, fires on cooldown indefinitely. Returns `new ChaseState` only if player escapes beyond `attackRange × 1.5`.

**`ParachuteState`** — used by `Paratrooper` while descending. Idle until `onGround == true`, then returns `new PatrolState`.

This is the pattern that earns the marks listed in the rubric under "OOP concepts" — each transition is a single line returning `new SomeState()`, completely encapsulating behaviour per state.

### The 8 concrete enemies

| Class | HP | Speed | Detect | Attack | Score | Special |
|---|---|---|---|---|---|---|
| **RebelSoldier** | 2 | 120 | 450 | 250 | 100 | Pistol, standard AI |
| **ShieldedSoldier** | 5 | 90 | 400 | 220 | 200 | Frontal shield blocks player bullets; `receiveMeleeHit` lets Marco's piercing knife instakill |
| **BazookaSoldier** | 2 | 70 | 500 | 350 | 300 | Slow ballistic rockets with area explosion |
| **GrenadeSoldier** | 2 | 100 | 420 | 300 | 250 | Ballistic grenade lob (area explosion) |
| **Paratrooper** | 2 | 110 | 450 | 250 | 150 | Drops from top of screen at reduced gravity; pistol once landed |
| **Zombie** | 5 | 80 | 500 | 240 | 200 | Contact damage every 0.8s when overlapping player |
| **MummyWarrior** | 5 | 70 | 480 | 65 | 400 | **Resurrects twice** at full HP; melee only; fire/explosion = instant permanent kill |
| **Martian** | 3 | 90 | — | — | 500 | Phase 1: hovers (near-zero gravity, drifts toward player). Phase 2: walks/fires energy beam once landed |

#### ShieldedSoldier — the directional shield
```cpp
bool ShieldedSoldier::playerIsInFront() const {
    // compare midpoints + facingRight → returns true when the player is on
    // the same side as the shield
}

void ShieldedSoldier::takeDamage(int amount) {
    if (playerIsInFront()) {
        DamageableEntity::takeDamage(0);  // visual flash, zero HP loss
        return;
    }
    DamageableEntity::takeDamage(amount); // hit from behind or above
}

bool ShieldedSoldier::receiveMeleeHit(int damage, bool fromPlayer) {
    if (!fromPlayer) return false;
    DamageableEntity::takeDamage(damage); // melee bypasses the shield entirely
    return true;
}
```

#### MummyWarrior — the resurrection
On reaching 0 HP, the mummy intercepts deactivation:
```cpp
if (!isActive && resurrectionCount < MAX_RESURRECTIONS) {
    isActive = true;       // cancel the death
    currentHP = maxHP;     // restore HP
    resurrectionTimer = 3.0f;   // "crumbling" pause
    invincibilityTimer = 3.0f;  // can't be hit during the pause
    resurrectionCount++;
}
```
After two resurrections, the third death is permanent. `receiveFireHit` and `receiveExplosionHit` override the resurrection to enable permanent kill on the first hit from a flame weapon or explosion.

### Score scan timing
When an enemy reaches 0 HP, `deactivateEntity()` marks it inactive but does **not** delete it. The mummy's resurrection logic exploits this to "cancel" deactivation. `PlayState::update` does the score scan **after** `updateAll()` returns and **before** `removeDead()` runs — by that point, mummies that resurrected are back to `isActive == true` and don't get scored.

---

## 8. Level System

### `Level.h` / `Level.cpp`

A tilemap with **5 tile types** stored as a 2D char array:
| Char | Tile |
|---|---|
| `\0` | air (passable) |
| `'g'` | grass (solid, destructible) |
| `'s'` | stone (solid, destructible) |
| `'d'` | dirt (solid, destructible) |
| `'b'` | bedrock (solid, indestructible) |

State:
- `char** lvl` — owned 2D grid
- `int gridWidth = 200, gridHeight = 22, cell_size = 64` → world is 12800 × 1408 px
- `sf::Texture grassTex, stoneTex, dirtTex, bedrockTex` — four block textures
- `sf::Sprite blockSprite` — reused for every draw call

**Rule of Three:** copy ctor and assignment are `= delete` (would double-free `lvl`).

Collision query methods:
- `bool checkLeftWall(leftX, topY, bottomY)` — anything solid touching the entity's left edge?
- `bool checkRightWall(rightX, topY, bottomY)` — same for the right edge
- `bool checkGroundBelow(feetY, leftX, rightX, int& outRow)` — solid tile under feet? Returns row of impact
- `bool checkCeiling(headY, leftX, rightX, int& outRow)` — same for upward motion
- `bool isSolidAtPixel(float x, float y)` — point query (used by projectile tile collision)

Used by `Soldier::resolveHorizontal/Vertical` and the matching methods on `Enemy`.

Rendering:
- `render(window, cameraX, cameraY)` only draws tiles inside the visible 1600×900 viewport. Computes start/end columns from `cameraX / cell_size` and skips everything outside. Critical for performance — a naive loop over 200×22 = 4400 tiles every frame would still be cheap, but only ~25×14 = 350 are visible at once.

Map building:
- `buildTestMap()` lays down the entire 12800-px world with three biome zones (plain / aerial / aquatic), platforms, and gaps. Called once from `PlayState` constructor.
- `fillColumn(int col, int surfaceRow)` — helper that paints grass on the surface, dirt for a few rows below, stone deeper, and bedrock at the very bottom.

---

## 9. PlayState — the conductor

This is where every subsystem meets. ~400 lines in `PlayState.cpp`.

### State variables
```cpp
Level*         level;            // owns the world
EntityManager* entities;         // owns every entity including the player
Player*        player;           // non-owning view of current character (manager owns the object)
Player*        respawnPrototype; // factory-only mirror used to recreate the player on death

float cameraX, cameraY;          // top-left of viewport in world coords
int   lives;                     // starts at 3
float respawnTimer;              // > 0 → waiting for respawn after death
float spawnX, spawnY;            // screen-space offsets used by respawn

int   score;
int   comboCount;
float comboTimer;                // > 0 → combo window open

int   currentLevel;              // 1 or 2
float levelTransitionTimer;      // > 0 → "LEVEL N COMPLETE!" overlay showing

sf::Font hudFont;
sf::Text scoreText, comboText, grenadeText, levelText, levelCompleteText;
```

### Construction
1. `new Level(200, 22, 64)` then `level->buildTestMap()`
2. `new EntityManager(64)` — initial capacity, grows on demand
3. `new EriPlayer(...)` added to the manager; `player` and `respawnPrototype` initialised
4. Load three biome backgrounds (`Plain_Biome.png`, `Aerial_Biome.png`, `Aquatic_Biome.png`)
5. Load HUD font from `Sprites/hud.ttf` → fallback to `arial.ttf` → fallback to `consola.ttf`
6. Configure all five `sf::Text` HUD elements
7. Call `startLevel(1)` to spawn enemies and update the level HUD label

### `handleInput(event, window)`
- ESC → close window
- Z → character switch via runtime polymorphism (see section 4)

### `update(float dt)` — the order matters

1. **dt clamp** to `1/30 s` — prevents teleporting on frame spikes
2. **Level-complete transition check** — if active, tick the timer and `return` early (world frozen). When it hits zero, call `startLevel(currentLevel + 1)`.
3. **`entities->updateAll(dt)`** — every entity ticks; projectiles internally call `checkEntityCollisions` via virtual dispatch
4. **Screen-clamp pass** — iterate entities, call `applyScreenClamp(cameraX)` on each (enemies stop at right edge; projectiles despawn off-screen)
5. **Score scan** — `for each entity: if !isActive && getScoreValue() > 0 → awardKill(value)`. Runs **before** `removeDead()` so dead enemies still exist
6. **Combo window tick** — counts down, resets `comboCount` and hides combo text when expired
7. **Player death check** — if `!player->getIsActive()`, clone the prototype, null the player pointer, decrement lives, set `respawnTimer`
8. **`entities->removeDead()`** — frees everything marked inactive (player included if they died this frame)
9. **Respawn countdown** — when `respawnTimer ≤ 0`, call `respawnPlayer()`
10. **Camera follow** — horizontal right-deadzone, vertical top/bottom deadzones, clamp to world edges
11. **Level-end trigger** — if `player->getPosX() >= 12500`, set `levelTransitionTimer` and configure the overlay text

### `render(window)`
1. Pick which biome background to draw based on `cameraX` zone (plain / aerial / aquatic)
2. Tile the background horizontally with parallax (scrolls at 0.2× camera speed)
3. `level->render(window, cameraX, cameraY)` — visible tiles
4. `entities->renderAll(...)` — every entity
5. HUD (top-left score + combo, bottom-left grenades, bottom-middle level indicator)
6. If transitioning, draw the centre overlay last

### `startLevel(int n)`
1. Deactivate every entity except the player (sweep clears stale enemies and projectiles from the previous level)
2. Reset player position + velocity, reset camera to (0, 0)
3. Call `spawnLevel1Enemies()` or `spawnLevel2Enemies()`
4. Rebuild the "LEVEL N" HUD string using `intToBuf` and `sf::String`; recompute centred position

### `awardKill(int baseValue)`
- If `comboTimer > 0`, increment `comboCount` (up to `COMBO_CAP = 8`)
- Reset `comboTimer` to `COMBO_WINDOW = 1.5 s`
- `multiplier = 1.0 + 0.25 × comboCount` (range 1.0× to 3.0×)
- Add to `score`
- Rebuild `scoreText` and `comboText` strings via `intToBuf` + `sf::String` concatenation

### `respawnPlayer()`
1. Use `respawnPrototype->createSelf(...)` to make a fresh same-character player at the spawn point
2. Add to entity manager
3. Call `onPlayerRespawn(player)` on every entity so enemy AI retargets

### Spawn helpers (`spawnRebelBatch`, etc.)
Each one places `count` of a specific enemy type at `(x, y)` with a fixed spacing per type. Called from `spawnLevel1Enemies` / `spawnLevel2Enemies`.

### `intToBuf` helper
Local `static` function in `PlayState.cpp` (file-scope). Writes a base-10 integer into a fixed `char` buffer with no allocations. Used everywhere a number needs to appear in the HUD because `<cstdio>` and `<string>` are both banned.

---

## 10. Patterns Cheat Sheet (for viva)

| Concept | Where in the code |
|---|---|
| **Abstract class** | `Entity` (`update`/`render` pure virtual), `DamageableEntity`, `Soldier`, `Player`, `Enemy`, `Weapon`, `Projectile`, `EnemyAIState` |
| **Virtual destructor** | Every abstract base — `virtual ~Entity() {}`, `virtual ~Weapon() {}`, etc. Without these, deleting via a base pointer would leak the subclass-owned resources |
| **Pure virtual factory** | `Player::createNext`, `Player::createSelf` — character switching uses these to avoid `if/else` on type |
| **Runtime polymorphism** | `Entity::update`, `Entity::receiveProjectileHit`, `Weapon::fire`, `EnemyAIState::update`. Every call site dispatches to the right subclass without knowing the concrete type |
| **State Pattern** | `EnemyAIState` and its four subclasses — each state encapsulates one mode of behaviour, transitions are values, not flags |
| **Composition** | `Enemy` owns `EnemyAIState*` + `Weapon*` (both `delete`d in destructor); `Player` owns its 7 weapon slots; `Level` owns its tile array |
| **Aggregation** | `Soldier::level`, `Enemy::level/player/entities` — pointers stored but never deleted; lifetime managed elsewhere |
| **Encapsulation** | All Entity state is `protected` or `private`. No public data members anywhere |
| **Rule of Three / Five** | `Level`, `EntityManager`, `Player`, `Enemy` all delete copy ctor and assignment — own raw memory, can't be deep-copied safely |
| **No `dynamic_cast`** | Collision dispatch (`receiveProjectileHit`/`receiveMeleeHit`/`receiveFireHit`/`receiveExplosionHit`) uses virtual dispatch instead of type-checking |

---

## 11. Data Flow Walkthroughs

### A. Player fires the pistol
1. User presses LMB → SFML event polled in `Game::run` → routed to `PlayState::handleInput`
2. `PlayState::update` calls `entities->updateAll(dt)`
3. EntityManager calls `player->update(dt)` → calls `player->handleInput()`
4. `Player::handleInput` detects `Mouse::isButtonPressed(Mouse::Left)`, edge-detects against `fireHeldLastFrame`
5. Computes `spawnX, spawnY` at the player's front edge
6. Calls `weaponSlots[activeSlot]->fire(spawnX, spawnY, facingRight, true, *entities)`
7. `ProjectileWeapon::fire` constructs a new `StraightProjectile(...)` and calls `em.add(...)`
8. Player's `onShotFired(...)` virtual hook fires — Marco may dual-fire backwards, Fiolina may halve the cooldown
9. EntityManager grows if at capacity, stores the projectile

**Next frame:**

10. `entities->updateAll(dt)` calls `StraightProjectile::update`
11. Projectile moves; cull check; `checkEntityCollisions` iterates the manager
12. It finds a `RebelSoldier` whose AABB overlaps; calls `dispatchHit(target)`
13. `dispatchHit` calls `target->receiveProjectileHit(damage, true)`
14. `Enemy::receiveProjectileHit` accepts (because `fromPlayer == true`) and calls `takeDamage`
15. `DamageableEntity::takeDamage` subtracts HP; at 0 calls `deactivateEntity()` → `isActive = false`
16. Back in `Projectile::checkEntityCollisions`, the hit was accepted → projectile deactivates too
17. `PlayState::update` continues past `updateAll`. Screen-clamp pass runs (no-op for dead entities)
18. **Score scan**: finds the dead rebel (`!isActive && getScoreValue() == 100`) → calls `awardKill(100)`
19. `awardKill` updates `score`, sets combo state, rebuilds the score string with `intToBuf` + `sf::String`
20. `removeDead()` frees both the dead enemy and the spent projectile

### B. Player switches character (Z key)
1. `PlayState::handleInput` sees `event.key.scancode == Z`
2. `Player* next = player->createNext(currentX, currentY, level, entities)` — virtual dispatch picks the right subclass (Marco → Tarma, etc.)
3. `delete respawnPrototype; respawnPrototype = next->createSelf(0, 0, level, entities)` — keep the prototype in sync
4. Capture `wasDevMode` flag from old player
5. `player->deactivateEntity()` — old player will be freed next frame by `removeDead()`
6. `entities->add(next)` — manager takes ownership
7. `player = next`
8. If `wasDevMode`, call `player->toggleDevMode()` to carry the cheat state across
9. Loop every active entity and call `onPlayerRespawn(player)` — `Enemy` overrides update their `player` pointer; everything else is a no-op

### C. Player dies → respawns
1. An enemy projectile lands → `target->receiveProjectileHit(damage, false)` returns `true` on Soldier (because `fromPlayer == false`)
2. `DamageableEntity::takeDamage` drains HP, eventually `deactivateEntity()`
3. Next `PlayState::update` runs `updateAll()`, then the **player-death check** runs **before** `removeDead()`:
   ```cpp
   if (player && !player->getIsActive()) {
       delete respawnPrototype;
       respawnPrototype = player->createSelf(0, 0, level, entities);  // capture type
       player = nullptr;
       lives--;
       if (lives > 0) respawnTimer = RESPAWN_DELAY;  // 2 seconds
   }
   ```
4. `removeDead()` frees the dead player
5. Two seconds later, `respawnTimer ≤ 0` → `respawnPlayer()` runs
6. `respawnPlayer` builds a new player from the prototype, adds to manager, blasts `onPlayerRespawn` to retarget enemies

### D. Player crosses `LEVEL_END_X` (level 1 → level 2)
1. After camera update in `PlayState::update`, the level-end check fires:
   ```cpp
   if (player->getPosX() >= 12500) {
       levelTransitionTimer = 2.0f;
       levelCompleteText.setString("LEVEL 1 COMPLETE!");
       levelCompleteText.setPosition(centred);
   }
   ```
2. Next frame, the level-complete branch at the top of `update()` returns early — world is frozen, only the overlay text renders
3. After 2 s, `levelTransitionTimer ≤ 0`. Since `currentLevel < MAX_LEVEL`, call `startLevel(currentLevel + 1)`
4. `startLevel(2)`:
   - Deactivate every entity except the player (kills leftover enemies and projectiles)
   - Reset player to (spawnX, spawnY) with zero velocity
   - Reset camera to (0, 0)
   - Call `spawnLevel2Enemies()` — new enemy layout
   - Rebuild "LEVEL 2" HUD text
5. Game resumes normally

---

## 12. File Index (alphabetical)

| File | One-line summary |
|---|---|
| `ARCHITECTURE.md` | This document |
| `BallisticProjectile.h/.cpp` | Gravity-affected arc projectile; spawns `Explosion` on impact |
| `BazookaSoldier.h/.cpp` | 2 HP enemy that launches slow ballistic rockets |
| `DamageableEntity.h/.cpp` | Adds HP, invincibility, and red-flash to `Entity` |
| `Enemy.h/.cpp` | Abstract base for all enemies — owns AI state + weapon |
| `EnemyAIState.h/.cpp` | State Pattern interface + Patrol/Chase/Attack/Parachute |
| `Entity.h/.cpp` | Root of the entity hierarchy — position, velocity, virtual update/render/hit-dispatch |
| `Entitymanager.h/.cpp` | Owns every Entity\*; raw growable array; update/render/remove sweep |
| `EriPlayer.h/.cpp` | Grenade specialist (no melee, doubled grenades, +50 % blast) |
| `Explosion.h/.cpp` | Short-lived area-damage entity spawned by ballistic impact |
| `FiolinaPlayer.h/.cpp` | Weapon master (auto-fire power-up, +50 % ammo on pickup) |
| `FlameProjectile.h/.cpp` | Range-limited fire pellet; routes damage through `receiveFireHit` |
| `FlameWeapon.h/.cpp` | Held-trigger flamethrower; rapid `FlameProjectile` spawns |
| `Game.h/.cpp` | Window + game loop + initial state push |
| `GameState.h` | Interface every screen implements |
| `GameStateManager.h/.cpp` | Raw-array stack of `GameState*` |
| `GrenadeSoldier.h/.cpp` | 2 HP enemy that lobs ballistic grenades |
| `LaserBeam.h/.cpp` | Visual-only entity for the laser beam |
| `LaserWeapon.h/.cpp` | Instant raycast weapon; damages everything along the line |
| `Level.h/.cpp` | 2D tilemap, 5 tile types, collision queries, render |
| `main.cpp` | 7-line entry point — instantiates `Game` |
| `MarcoPlayer.h/.cpp` | Fast-fire balanced character (piercing knife, dual-fire power-up) |
| `Martian.h/.cpp` | 3 HP alien — two-phase (hover then ground combat) |
| `MeleeWeapon.h/.cpp` | Knife — overlap scan, optional piercing |
| `MenuState.h/.cpp` | Main menu — pick survival or campaign |
| `MummyWarrior.h/.cpp` | 5 HP undead that resurrects twice; permanent kill only via fire/explosion |
| `Paratrooper.h/.cpp` | Drops with low gravity, transitions to standard infantry on landing |
| `Player.h/.cpp` | Abstract player base; weapon slots, grenades, power-up, dev mode |
| `PlayState.h/.cpp` | The conductor — owns world, manages levels/score/respawn/HUD |
| `Projectile.h/.cpp` | Abstract base for shot entities; centralised collision dispatch |
| `ProjectileWeapon.h/.cpp` | Configurable weapon → spawns Straight or Ballistic projectiles |
| `RebelSoldier.h/.cpp` | 2 HP grunt with pistol — baseline enemy |
| `ShieldedSoldier.h/.cpp` | 5 HP enemy with directional frontal shield |
| `Soldier.h/.cpp` | Walking, jumping, axis-separated tile collision |
| `StraightProjectile.h/.cpp` | Constant-velocity bullet — pistol/HMG ammo |
| `TarmaPlayer.h/.cpp` | Slower/frail-on-foot character with 20-second damage immunity power-up |
| `Weapon.h/.cpp` | Abstract base — cooldown bookkeeping, virtual `fire` |
| `Zombie.h/.cpp` | 5 HP shambler with melee contact damage |

---
