#pragma once
#include "includes.h"

class Object
{
public:
	Object(float x, float y);
	Object() = default;
	virtual ~Object() {
		for (Object* obj : children)
		{
			if (obj != NULL)
				delete obj;
		}
		beforeDeath();
		die();
	}

	std::vector<Object*> children;

	bool isCreated;
	void create();
	virtual void update(Events::updateEvent ev) {};
	virtual void draw() {};
	virtual void mouseDown() {};
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void textInput(SDL_TextInputEvent ev) {};
	virtual void beforeDeath() {};
	void add(Object* obj)
	{
		children.push_back(obj);
	}

	void removeObj(Object* obj)
	{
		children.erase(std::remove(children.begin(), children.end(), obj), children.end());
		delete obj;
	}

	bool handleDraw = false;

	float alpha = 1;
	float x = 0;
	float scale = 1.0;
	int w = 0;
	float y = 0;
	int h = 0;

	float offsetX = 0;
	float offsetY = 0;

	bool isLocal = false;

	bool isDead = false;


	virtual void die();


	void drawChildren()
	{
		for (Object* obj : children)
			obj->draw();
	}

	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}


	virtual void setX(float x);
	virtual void setY(float y);
};

