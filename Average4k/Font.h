#pragma once
#include "includes.h"

class Font {
public:
	static std::map<int, TTF_Font*>* mapOfFonts;

	static TTF_Font* getFont(int size)
	{
		if ((*mapOfFonts).contains(size))
			return (*mapOfFonts)[size];
		(*mapOfFonts)[size] = TTF_OpenFont("assets/graphical/fonts/NotoSans-Regular.ttf",size);
		return (*mapOfFonts)[size];
	}
};