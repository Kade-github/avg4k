#pragma once
#include "Object.h"
#include "includes.h"
#include "Game.h"
#include "Font.h"

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class Text : public Object
{
public:
	TTF_Font* Arial;

	Text(int x, int y, std::string temp, int _size, std::string fontName) : Object(x, y) {
		Arial = Font::getFontByName(fontName,_size);
		this->text = temp;
		this->w = w;
		this->h = h;
		this->rW = w;
		this->rH = h;
		size = _size;
		border = true;

		color = { 255,255,255 };
		
		setText(temp);
	};

	int size;

	~Text() = default;
	void draw() override;
	void die() override;
	void centerX();
	void centerY();
	Color color;

	SDL_Texture* outline;
	SDL_Texture* message;
	SDL_FRect message_Rect;

	std::string text;
	int w;
	int h;

	int addW;
	int addH;

	bool border = true;

	int surfW;
	int surfH;

	int rW;
	int rH;

	double scale;
	void setFont(std::string name);
	void forceDraw();

	void setText(std::string _text);
};

