#pragma once
#include "Object.h"
#include "includes.h"
#include "Game.h"

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class Text : public Object
{
public:

	TTF_Font* GetFont() {
		static TTF_Font* Sans = nullptr;

		if (!Sans)
			Sans = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 24); // we support linux :)
		return Sans;
	}

	Text(int x, int y, std::string temp, int w, int h) : Object(x, y) {

		this->text = temp;
		this->w = w;
		this->h = h;
		this->rW = w;
		this->rH = h;

		SDL_Surface* screen = SDL_GetWindowSurface(Game::window);

		color = { 255,255,255 };
		
		setText(temp);
	};


	~Text() = default;
	void update(Events::updateEvent event) override;

	Color color;

	SDL_Texture* message;
	SDL_FRect message_Rect;

	std::string text;
	int w;
	int h;

	int surfW;
	int surfH;

	int rW;
	int rH;

	void setText(std::string _text);
};

