#pragma once
#include "includes.h"
#include "Object.h"
#include "Texture.h"
#include "SaveFile.h"
#include "Text.h"

class AvgCheckBox : public Object {
public:
	bool def;

	Text* textPart;

	setting toModify;

	float time = 0;

	Texture* box;
	Texture* toggle;

	AvgCheckBox(int _x, int _y, bool _def)
	{
		x = _x;
		y = _y;
		def = _def;

		toModify.name = "none";

		box = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/toggle_bg.png");
		toggle = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/toggle.png");
		w = box->width;
		h = box->height;
		textPart = new Text(x, y, def ? "on" : "off", 10, "arialbd");
		textPart->setCharacterSpacing(1.67);
		textPart->color = { 13, 28, 64 };
		time = 500;
	}


	void mouseDown()
	{
		int _x, _y;
		Game::GetMousePos(&_x, &_y);

		if (parent == NULL)
			return;

		float scrll = ((AvgContainer*)parent)->scrollAddition;

		int relX = _x - parent->x;
		int relY = _y - parent->y + scrll;

		if ((relX > x && relY > y) && (relX < x + w && relY < y + h))
		{
			def = !def;
			if (def)
				textPart->setText("on");
			else
				textPart->setText("off");
			if (toModify.name != "none")
			{
				Game::save->SetBool(toModify.name, def);
				Game::save->Save();
			}

			time = 0;
		}
	}

	void draw()
	{
		Rect dstRect;
		Rect toggleRect;
		Rect srcRect;


		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;

		dstRect.a = alpha;

		if (time < 500)
			time += Game::deltaTime * 8;

		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = w;
		dstRect.h = h;

		toggleRect.x = std::lerp((x + (w / 2)) - 4, x + 2, time / 500);
		toggleRect.a = alpha;
		toggleRect.y = y + 2;
		toggleRect.w = toggle->width;
		toggleRect.h = toggle->height;

		textPart->y = (toggleRect.y + (textPart->h / 2)) - 2;
		textPart->x = (x + (textPart->w / 2)) + (w / 2);

		if (def)
		{
			toggleRect.x = std::lerp(x + 2, (x + (w / 2)) - 4, time / 500);
			textPart->x = (x + (textPart->w / 2));
		}

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		Rendering::PushQuad(&dstRect, &srcRect, box, GL::genShader);
		Rendering::PushQuad(&toggleRect, &srcRect, toggle, GL::genShader);

		textPart->alpha = alpha;
		textPart->draw();
	}
};