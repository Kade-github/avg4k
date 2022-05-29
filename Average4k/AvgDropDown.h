#pragma once
#include "includes.h"
#include "Object.h"
#include "Texture.h"
#include "SaveFile.h"
#include "Text.h"
#include "AvgContainer.h"


typedef void(__cdecl* dropDownCallback)(std::string, std::string);

class AvgDropDown : public Object {
public:

	std::vector<std::string> def;

	std::vector<Text*> textObjects;

	Text* textPart;

	int addHeight = 0;

	bool isActive = false;

	std::vector<Text*> otherTexts;

	setting toModify;

	Texture* box;
	Texture* dropTexture;
	Texture* dropEnd;

	Text* selectedText;

	dropDownCallback call;

	AvgDropDown(int _x, int _y, std::vector<std::string> _def)
	{
		x = _x;
		y = _y;
		def = _def;

		toModify.name = "none";

		box = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/dropdown.png");
		dropTexture = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/dropdownItem.png");
		dropEnd = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/endOfDropdown.png");

		w = box->width;
		h = box->height;

		Text* def = new Text(0, 0, "N/A", 14, "arial");
		def->color = { 13, 28, 64 };
		def->setCharacterSpacing(2.33);
		textObjects.push_back(def);

		for (std::string s : _def)
		{
			Text* t = new Text(0,0,s,14, "arial");
			t->color = { 13, 28, 64 };
			t->setCharacterSpacing(2.33);
			textObjects.push_back(t);
		}
	}


	void mouseDown()
	{
		if (((AvgContainer*)parent)->dropDownActive && !isActive)
			return;
		int _x, _y;
		Game::GetMousePos(&_x, &_y);

		if (parent == NULL)
			return;

		float scrll = ((AvgContainer*)parent)->scrollAddition;

		int relX = _x - parent->x;
		int relY = _y - parent->y + scrll;

		if (selectedText != NULL && isActive)
		{
			isActive = false;
			Game::save->SetString(toModify.name, selectedText->text);
			call(toModify.name, selectedText->text);
			selectedText = nullptr;
		}

		if ((relX > x && relY > y) && (relX < x + (w * 1.4) && relY < y + ((h + addHeight) * 1.4)))
		{
			isActive = true;
		}
		else
		{
			isActive = false;
			selectedText = nullptr;
		}

	}

	void draw()
	{
		Rect dstRect;
		Rect arrowRect;
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

		Text* first = textObjects[0];

		std::string settingText = Game::save->GetString(toModify.name);
		if (first->text != settingText)
			first->setText(settingText);

		first->x = (x + 6);
		first->y = (y + ((first->h / 2) - 3));
		if (isActive)
		{
			first->alpha = 1;
			Rect _g;
			_g.x = x;
			_g.y = y + (dropTexture->height / 2);
			_g.w = dropTexture->width;
			_g.h = dropTexture->height;
			Rendering::PushQuad(&_g, &srcRect, dropTexture, GL::genShader);
		}
		else
			first->alpha = 0.5;

		Rendering::PushQuad(&dstRect, &srcRect, box, GL::genShader);
		first->draw();

		if (isActive)
		{
			int _x, _y;
			Game::GetMousePos(&_x, &_y);


			float scrll = ((AvgContainer*)parent)->scrollAddition;

			if (textObjects.size() == 1)
			{
				Rect g;

				g.x = x;
				g.y = y + dropTexture->height;
				g.w = dropTexture->width;
				g.h = dropTexture->height;
				Rendering::PushQuad(&g, &srcRect, dropEnd, GL::genShader);
			}
			bool selected = false;
			for (int i = 1; i < textObjects.size(); i++)
			{
				Rect g;

				g.x = x;
				g.y = y + (dropTexture->height * i);
				g.w = dropTexture->width;
				g.h = dropTexture->height;


				int extra = 0;

				if (i == textObjects.size() - 1)
				{
					Rendering::PushQuad(&g, &srcRect, dropEnd, GL::genShader);
					extra = 3;
				}
				else
				{
					Rendering::PushQuad(&g, &srcRect, dropTexture, GL::genShader);
				}

				Text* t = textObjects[i];
				t->x = g.x + 6;
				t->y = g.y + ((t->h / 2) - (3 + extra));

				t->draw();

				if ((_x > g.x && _y > g.y) && (_x < g.x + (g.w) && _y < (g.y) + (g.h)))
				{
					selectedText = t;
					selected = true;
					t->alpha = 0.5;
				}
				else
					t->alpha = 1;
			}
			if (!selected)
				selectedText = NULL;
		}
	}
};