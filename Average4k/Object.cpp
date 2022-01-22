#include "includes.h"
#include "Object.h"

#include "Game.h"

float Object::currentId = 0;


Object::Object(float x, float y)
{
	setX(x);
	setY(y);
	currentId++;
	id = currentId;
	isCreated = false;
}

void Object::create()
{
	Game::addGlobalObject(this);
	isCreated = true;
}

void Object::setX(float x)
{
	if (isDead)
		return;
	this->x = x;
}

void Object::setY(float y)
{
	if (isDead)
		return;
	this->y = y;
}

void Object::die()
{
	isDead = true;
	if (isCreated)
		Game::removeGlobalObject(this);
}
