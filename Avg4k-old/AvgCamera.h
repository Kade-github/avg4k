#pragma once
#include "AvgGroup.h"
#include "includes.h"

struct AvgColorTint {
	int r, g, b;
};

class AvgCamera : public Object
{
public:
	AvgColorTint tint;

	Rect clipRect;

	float initalW, initalH;

	AvgCamera(int _x, int _y, int _w, int _h)
	{
		MUTATE_START
		id = 690690690;
		handleDraw = true;
		w = _w;
		h = _h;
		x = _x;
		y = _y;
		tint.r = -1;

		initalW = _w;
		initalH = _h;
		MUTATE_END
	}

	float shakeTime = 0;
	float shakeInten;
	void leDraw(Object* obj);

	void shake(float time, float inten)
	{
		shakeTime = time;
		shakeInten = inten;
	}

	void resize(float _w, float _h)
	{
		w = _w;
		h = _h;

		float aspect = w / h;

	}

	virtual void draw();

};