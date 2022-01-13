#include "includes.h"
#include "Object.h"

#include "Game.h"


Object::Object(float x, float y)
{
	setX(x);
	setY(y);
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


int* Object::getW(int* defaultW)
{
	static int* w;
	if (!w)
		w = defaultW;
	return w;
}

int* Object::getH(int* defaultH)
{
	static int* h;
	if (!h)
		h = defaultH;
	return h;
}