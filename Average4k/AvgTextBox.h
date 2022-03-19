#pragma once
#include "includes.h"
#include "Object.h"
#include "Texture.h"
#include "SaveFile.h"
#include "Text.h"

class AvgTextBar : public Object {
public:
	std::string def;

	std::string type;

	Text* textPart;

	setting toModify;

	int spacedOut = 32;

	Texture* searchBar;

	bool typing = false;

	AvgTextBar(int _x, int _y, std::string _def, Texture* searchTex)
	{
		x = _x;
		y = _y;
		def = _def;

		toModify.name = "none";

		searchBar = searchTex;
		w = searchBar->width;
		h = searchBar->height;

		textPart = new Text(_x + 24, _y + searchBar->height / 2, def, 14, "arial");
		textPart->setCharacterSpacing(3);
		textPart->color = { 64,64,64 };
	}


	void keyDown(SDL_KeyboardEvent ev)
	{
		if (typing && ev.keysym.sym == SDLK_BACKSPACE && type.size() > 0)
		{
			type.pop_back();
			if (toModify.name != "none")
			{
				Game::save->SetString(toModify.name, type);
				Game::save->Save();
			}
			textPart->setText(type + "_");
		}
	}

	void textInput(SDL_TextInputEvent event)
	{
		if (typing)
		{
			type += event.text;
			if (toModify.name != "none")
			{
				Game::save->SetString(toModify.name, type);
				Game::save->Save();
			}
			textPart->setText(type + "_");
		}
	}

	void mouseDown()
	{
		int _x, _y;
		Game::GetMousePos(&_x, &_y);

		int relX = _x - parent->x;
		int relY = _y - parent->y;

		if ((relX > x && relY > y) && (relY < x + w && relY < y + h))
		{
			typing = true;
			if (type.size() == 0)
				textPart->setText("_");
		}
		else
		{
			typing = false;
			if (type.size() == 0)
				textPart->setText(def);
			else
				textPart->setText(type);
		}
	}

	void draw()
	{
		Rect dstRect;
		Rect srcRect;

		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;




		dstRect.a = alpha;

		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = w;
		dstRect.h = h;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		if (typing)
		{
			textPart->alpha = alpha;
		}
		else
			textPart->alpha = alpha != 0 ? 0.5 : 0;

		Rendering::PushQuad(&dstRect, &srcRect, searchBar, GL::genShader);

		textPart->x = x + spacedOut;
		textPart->y = y + 1 + ((h / 2) - (textPart->h / 2));

		textPart->draw();
	}
};