#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h"
#include "Text.h"
#include "TweenManager.h"

// Objects

class PackObject : public Object
{
public:

	Text* pack;

	Texture* bg;

	PackObject(int _x, int _y, std::string packName, Texture* background)
	{
		if (background == NULL)
			bg = Rendering::white;
		else
			bg = background;

		x = _x;
		y = _y;

		pack = new Text(0, 0, packName, 18, "ANDALEMO");
		pack->color = { 0,0,0 };
		pack->setCharacterSpacing(3);

	}


	void draw()
	{
		Rect dstRect;
		Rect srcRect;

		dstRect.r = 0;
		dstRect.g = 0;
		dstRect.b = 0;

		dstRect.a = 1;

		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = w;
		dstRect.h = h;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		Rendering::PushQuad(&dstRect, &srcRect, bg, GL::genShader);

		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;

		dstRect.x += 2;
		dstRect.y += 2;
		dstRect.w -= 2;
		dstRect.h -= 2;

		Rendering::PushQuad(&dstRect, &srcRect, bg, GL::genShader);

		pack->x = x + ((w / 2) - (pack->surfW / 2));
		pack->y = y + ((h / 2) - (pack->surfH / 2));

		pack->draw();
	}
};

// class

class MainerMenu : public Menu
{
public:
	float lastBeat = 0;
	bool started;
	AvgSprite* icon;
	AvgSprite* bg;
	Text* hello;
	Text* bottom;
	AvgSprite* border;

	void addPack(std::string name);
	void clearPacks();

	void create() override;

	void update(Events::updateEvent ev) override;

	void keyDown(SDL_KeyboardEvent event) override;
};

