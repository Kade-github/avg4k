#pragma once
#include "includes.h"
class Object
{
public:

	bool drawLast = false;
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

	Object* parent;
	int realPosX, realPosY;
	bool isCreated;
	void create();
	virtual void update(Events::updateEvent ev) {};
	virtual void draw() {};
	virtual void mouseDown() {};
	virtual void onPacket(PacketType pt, char* data, int32_t length) {};
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void textInput(SDL_TextInputEvent ev) {};
	virtual void beforeDeath() {};
	void add(Object* obj)
	{
		children.push_back(obj);
	}

	void removeObj(Object* obj)
	{
		children.erase(
			std::remove_if(children.begin(), children.end(), [&](Object* const ob) {
				return ob->id == obj->id;
				}),
			children.end());
		if (obj->id >= 0 && !obj->isDead)
 			delete obj;
	}

	bool handleDraw = false;
	bool drawGroups = false;
	bool staticView = false;
	bool customDraw = false;

	float alpha = 1;
	float angle = 0;
	float x = 0;
	float scale = 1.0;
	int w = 0;
	float y = 0;
	int h = 0;


	static float currentId;
	float id = 0;

	float offsetX = 0;
	float offsetY = 0;

	bool isLocal = false;

	bool isDead = false;

	bool isActive = true;

	virtual void die();


	void drawChildren();


	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}

	virtual void mouseWheel(float amount) {};

	virtual void setX(float x);
	virtual void setY(float y);
};

