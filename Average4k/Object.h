#pragma once
#include "includes.h"

class Object
{
public:
	Object(float x, float y);
	Object() = default;
	~Object() {
		for (Object* obj : children)
		{
			obj->beforeDeath();
			obj->die();
		}
		beforeDeath();
		die();
	};

	std::vector<Object*> children;

	bool isCreated;
	void create();
	virtual void update(Events::updateEvent ev) {};
	virtual void draw() {};
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
		obj->beforeDeath();
		obj->die();
	}

	float alpha = 255;
	float x = 0;
	float y = 0;
	virtual int* getW(int* defaultW);
	virtual int* getH(int* defaultH);

	bool isLocal = false;

	bool isDead = false;


	virtual void die();


	void drawChildren()
	{
		for (Object* obj : children)
			obj->draw();
	}


	virtual void setX(float x);
	virtual void setY(float y);
};

