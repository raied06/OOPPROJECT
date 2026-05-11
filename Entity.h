#pragma once
#include <SFML/Graphics.hpp>
class Player; // forward declaration for onPlayerRespawn
class Entity
{
protected:
    float positionX;
    float positionY;
    float entityWidth;
    float entityHeight;
    float velocityX;
    float velocityY;
    bool isActive;   // false means we will delete it in next frames cleanup
public:
// CONSTRUCTOR
    Entity(float x, float y, float w, float h);
// VIRTUAL DESTRUCTOR (Bcz deleteing any entity like soldier will delete entity but soldier will not be deleted (memory leak) )
    virtual ~Entity() {}
// PURE VIRTUAL FUNCTIONS (Will be used later on in every concrete subclass)
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float cameraX, float cameraY) = 0;
// GETTERS
    float getPosX() const;
    float getPosY() const;
    float getEntityWidth() const;
    float getEntityHeight() const;
    float getVelocityX() const;
    float getVelocityY() const;
    bool getIsActive() const;
// SETTERS
    void setPosition(float x_coordinate, float y_coordinate);
    void setVelocity(float Vx, float Vy);
    void deactivateEntity();
// OTHER FUNCTIONS
    bool isOverlapping(const Entity* other) const;

    // Called by a Projectile when it overlaps this entity.
    // Returns true if the hit was accepted (projectile should deactivate).
    // Base class ignores all hits — subclasses opt in by overriding.
    virtual bool receiveProjectileHit(int damage, bool fromPlayer);

    // Called by a piercing melee weapon (Marco's knife).
    // Bypasses shield logic — enemies override this separately from receiveProjectileHit.
    virtual bool receiveMeleeHit(int damage, bool fromPlayer) { return receiveProjectileHit(damage, fromPlayer); }

    // Called by a fire projectile (flame weapon). Default routes through the
    // normal projectile hit; MummyWarrior overrides — fire is one of the only
    // things that can kill a mummy permanently.
    virtual bool receiveFireHit(int damage, bool fromPlayer) { return receiveProjectileHit(damage, fromPlayer); }

    // Called by an explosion (grenades, rocket impact, etc.). Default routes
    // through the normal projectile hit; MummyWarrior overrides for permanent kill.
    virtual bool receiveExplosionHit(int damage, bool fromPlayer) { return receiveProjectileHit(damage, fromPlayer); }

    // Called on every entity when the player respawns so AI can retarget.
    // Base class no-op — Enemy overrides to update its player pointer.
    virtual void onPlayerRespawn(Player* newPlayer) {}

    // Called each frame to clamp entity to screen boundaries.
    // Base class no-op — Enemy overrides to prevent walking off right edge.
    virtual void applyScreenClamp(float cameraX) {}

    // Score awarded to the player when this entity dies.
    // Default 0 — non-enemies (projectiles, player) award nothing.
    // Concrete Enemy subclasses override to return their kill value.
    virtual int getScoreValue() const { return 0; }
};