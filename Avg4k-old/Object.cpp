#include "includes.h"
#include "Object.h"
#include "GL.h"
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

void Object::drawChildren()
{
	for (Object* obj : children)
	{
		if (dynamic_cast<AvgGroup*>(obj) != nullptr)
		{
			obj->draw();
			Rendering::drawBatch();
			Rendering::setBlend();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			AvgGroup* gr = (AvgGroup*)obj;

			Rect gdstRect;
			Rect gsrcRect;

			if (gr->center)
			{
				float mpx = (gr->w * (1 - gr->scale)) / 2;
				float mpy = (gr->h * (1 - gr->scale)) / 2;

				gdstRect.x = gr->x + mpx;
				gdstRect.y = gr->y + mpy;

				gdstRect.w = gr->w * gr->scale;
				gdstRect.h = gr->h * gr->scale;
			}
			else
			{

				gdstRect.x = gr->x;
				gdstRect.y = gr->y;

				gdstRect.w = gr->w;
				gdstRect.h = gr->h;
			}
			gdstRect.r = 255;
			gdstRect.g = 255;
			gdstRect.b = 255;
			gdstRect.a = 1;

			gsrcRect.x = 0;
			gsrcRect.y = 1;
			gsrcRect.w = 1;
			gsrcRect.h = -1;

			if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
				Rendering::SetClipRect(&gr->clipRect);
			else
				Rendering::SetClipRect(NULL);

			Rendering::PushQuad(&gdstRect, &gsrcRect, gr->ctb, GL::genShader);

			if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
				Rendering::SetClipRect(NULL);
		}
		else
			obj->draw();
	}
}
