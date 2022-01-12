#pragma once
#include "includes.h"

class Font {
public:
	static std::map<std::string, TTF_Font*>* mapOfFonts;

	static TTF_Font* getFontByName(std::string name, int size)
	{
		std::cout << "loading " << ("assets/graphical/fonts/" + name + ".ttf") << std::endl;
		if ((*mapOfFonts).contains(name))
		{
			TTF_CloseFont((*mapOfFonts)[name]);
		}
		(*mapOfFonts)[name] = TTF_OpenFont(("assets/graphical/fonts/" + name + ".ttf").c_str(), size);
		return (*mapOfFonts)[name];
	}
};