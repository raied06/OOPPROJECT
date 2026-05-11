#include "Entity.h"

Entity::Entity(float x, float y, float w, float h)
{
	this->positionX = x;
	this->positionY = y;
	this->entityWidth = w;
	this->entityHeight = h;
	this->velocityX = 0;
	this->velocityY = 0;
	isActive = true;
}

float Entity::getPosX() const
{
	return positionX;
}

float Entity::getPosY() const
{
	return positionY;
}

float Entity::getEntityWidth() const
{
	return entityWidth;
}

float Entity::getEntityHeight() const
{
	return entityHeight;
}

float Entity::getVelocityX() const
{
	return velocityX;
}

float Entity::getVelocityY() const
{
	return velocityY;
}

bool Entity::getIsActive() const
{
	return isActive;
}

void Entity::setPosition(float x_coordinate, float y_coordinate)
{
	this->positionX = x_coordinate;
	this->positionY = y_coordinate;
}

void Entity::setVelocity(float Vx, float Vy)
{
	this->velocityX = Vx;
	this->velocityY = Vy;
}

void Entity::deactivateEntity()
{
	this->isActive = false;
}

bool Entity::isOverlapping(const Entity* other) const
{
	// The overlap is checked by doing the opposite, rather then checking that do the x and y coordinates of both the
	// entities overlap, we check that if one entity is entirely to the right,left,top or bottom of the other and vice versa. 
	// If any of this is true, we simply return false which means that they are not overlapping, and if none of this check 
	// chains is true, this means that entities are overlapping so we return true.
	if (other == nullptr)
		return false;
	if (this->isActive && other->isActive) {
		if (this->positionX + this->entityWidth <= other->positionX)
			return false; // If this entity is totally left to the other
		if (other->positionX + other->entityWidth <= this->positionX)
			return false; // If this entity is totally right to the other

		if (this->positionY + this->entityHeight <= other->positionY)
			return false; // If this entity is up from the other
		if (other->positionY + other->entityHeight <= this->positionY)
			return false; // If this entity is to the bottom of the other
	}
	else {
		return false;
	}
	return true;
}

bool Entity::receiveProjectileHit(int damage, bool fromPlayer)
{
	return false;
}
