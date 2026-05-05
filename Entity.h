#pragma once
#include <SFML/Graphics.hpp>
class Entity
{
protected:
    float positionX;
    float positionY;
    float entityWidth;
    float entityHeight;
    float velocityX;
    float velocityY;
    bool isActive;   // false means "delete me on next cleanup pass"
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
};

