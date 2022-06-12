#pragma once
#include "includes.h"
#include "Game.h"
class ReceptorObject : public Object {
public:
	int x;
	int y;

	int type;
	float defAlpha = 1;
	float lightUpTimer;

	Shader* customShader;

	ReceptorObject(int _x, int _y, int _type);

	void light()
	{
		lightUpTimer = 195;
	}

	void draw() override;
};