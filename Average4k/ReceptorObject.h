#pragma once
#include "includes.h"
#include "Game.h"
class ReceptorObject {
public:
	int x;
	int y;

	int type;
	
	float lightUpTimer;

	ReceptorObject(int _x, int _y, int _type) {
		x = _x;
		y = _y;
		type = _type;
	}

	void light()
	{
		lightUpTimer = 1;
	}

	void draw();
};