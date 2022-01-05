#pragma once
#include "includes.h"
#include "Game.h"
class ReceptorObject : public Object {
public:
	int x;
	int y;

	int type;
	
	float lightUpTimer;

	ReceptorObject(int _x, int _y, int _type) {
		x = _x;
		y = _y;
		// this does the same thing but im lazy and too lazy to check
		setX(_x);
		setY(_y);
		type = _type;
	}

	void light()
	{
		lightUpTimer = 1;
	}

	void draw() override;
};