#pragma once
#include "includes.h"
#include "Object.h"
#include "Texture.h"
#include "SaveFile.h"
#include "Text.h"
#include "Helpers.h"

class AvgTextBar : public Object {
public:
	std::string def;

	std::string type;
	std::string suffix = "";

	Text* textPart;

	setting toModify;

	bool unique;

	int spacedOut = 32;

	Texture* searchBar;

	bool typing = false;

	AvgTextBar(int _x, int _y, std::string _def, Texture* searchTex)
	{
		MUTATE_START
		x = _x;
		y = _y;
		def = _def;

		toModify.name = "none";

		searchBar = searchTex;
		w = searchBar->width;
		h = searchBar->height;

		textPart = new Text(_x + 24, _y + searchBar->height / 2, def, 14, "arial");
		setText(true, false, true);
		textPart->setCharacterSpacing(2.33);
		textPart->color = { 13, 28, 64 };
		MUTATE_END
	}
	~AvgTextBar()
	{
		delete textPart;
		delete searchBar;
	}

	void resyncText()
	{
		setText(true, false, true);
	}

	void setText(bool de = false, bool include_ = true, bool checkForLong = false)
	{
		MUTATE_START
		std::string todo = de ? def : type;
		if (todo.size() == 0 && !include_)
			return;

		if (Helpers::is_number(todo.c_str()) && checkForLong)
		{
			size_t n = std::count(todo.begin(), todo.end(), '.');
			if (n == 1)
			{
				Helpers::removeTrailingCharacters(todo, '0');
				if (todo[todo.size() - 1] == '.')
					todo = todo.substr(0, todo.size() - 1);

				type = todo;
				if (toModify.name != "none")
					def = todo;
			}
		}
		textPart->setText(todo + (include_ ? "_" : "") + suffix);
		MUTATE_END
	}

	void keyDown(SDL_KeyboardEvent ev)
	{
		MUTATE_START
		if (typing && ev.keysym.sym == SDLK_BACKSPACE && type.size() > 0)
		{
			type.pop_back();
			if (toModify.name != "none")
			{
				def = type;
				Game::save->SetString(toModify.name, type);
				Game::save->Save();
			}
			setText();
		}
		if (typing && ev.keysym.sym == SDLK_MINUS)
		{
			if (toModify.name != "none" && toModify.takesDouble)
			{
				if (type.length() != 0)
				{
					type = std::to_string(-std::stod(type));
					def = type;
				}
			}
			setText(false,true,true);
		}
		MUTATE_END
	}

	void textInput(SDL_TextInputEvent event)
	{
		MUTATE_START
		if (typing)
		{
			if (unique)
			{
				if (type.find(event.text) != std::string::npos)
					return;
			}


			type += event.text;
			if (toModify.name != "none")
			{
				if (toModify.takesString)
					if (type.size() > toModify.defaultMax && toModify.defaultMax != 0)
					{
						type.pop_back();
						return;
					}

				if (!Helpers::is_number(event.text) && toModify.takesDouble)
				{
					if (type[type.size() - 1] == '.')
					{
						size_t n = std::count(type.begin(), type.end(), '.');
						if (n > 1)
						{
							type.pop_back();
							return;
						}
					}
					else
					{
						type.pop_back();
						return;
					}
				}

				def = type;
				if (toModify.takesDouble && type[type.size() - 1] != '.')
					Game::save->SetDouble(toModify.name, std::stod(std::string(type)));
				else
					Game::save->SetString(toModify.name, type);
				Game::save->Save();
			}
			setText();
		}
		MUTATE_END
	}

	void mouseDown()
	{
		MUTATE_START
		int _x, _y;
		Game::GetMousePos(&_x, &_y);

		if (parent == NULL)
			return;

		float scrll = ((AvgContainer*)parent)->scrollAddition;

		int relX = _x - parent->x;
		int relY = _y - parent->y + scrll;

		if ((relX > x && relY > y) && (relX < x + w && relY < y + h))
		{
			if (toModify.name != "none")
				type = def;
			typing = true;
			setText();
		}
		else
		{
			typing = false;

			if (Helpers::is_number(type.c_str()) && toModify.takesDouble)
			{
				double d = std::stod(std::string(type));
				if (d < toModify.defaultMin)
					type = std::to_string(toModify.defaultMin);
				if (d > toModify.defaultMax)
					type = std::to_string(toModify.defaultMax);

				def = type;

				Game::save->SetDouble(toModify.name, std::stod(std::string(type)));
				Game::save->Save();
			}

			if (type.size() == 0)
				setText(true,false,true);
			else
				setText(false,false,true);
		}
		MUTATE_END
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