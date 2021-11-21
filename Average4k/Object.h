#pragma once
#include "includes.h"

class Object
{
public:
	Object(float x, float y);
	Object() = default;
	~Object() = default;
	bool isCreated;
	void create();
	virtual void update(Events::updateEvent ev) = 0;
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void textInput(SDL_TextInputEvent ev) {};

	float alpha = 255;
	float x = 0;
	float y = 0;
	virtual int* getW(int* defaultW);
	virtual int* getH(int* defaultH);

	bool isLocal = false;

	bool isDead = false;


	void die();



	virtual void setX(float x);
	virtual void setY(float y);
};

