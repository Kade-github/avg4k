#pragma once
#include "includes.h"
#include "Game.h"
class ReceptorObject : public Object {
private:
	float animStartTime = 0;
	float animTime = 0;
public:

	int frame;
	int fps = 24;

	bool loop = true;
	bool bot = false;
	bool animationFinished;

	float beat = 0;
	Chart currentChart;

	bool hit = false;

	std::string lastFrame = "";

	float pX;
	float pY;

	int x;
	int y;

	int type;
	float defAlpha = 1;
	float lightUpTimer;

	float modX, modY;

	float endX = 0;
	float endY = 0;

	float positionInSong;

	Shader* customShader;

	ReceptorObject(int _x, int _y, int _type);

	void light()
	{
		lightUpTimer = 195;
	}

	void draw() override;
};