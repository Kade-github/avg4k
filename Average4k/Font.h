#pragma once
#include "includes.h"

struct avgFont {
	TTF_Font* font;
	int size;
	std::string name;
	bool friend operator==(const avgFont& lhs, const avgFont& rhs)
	{
		return (lhs.name == rhs.name) && (lhs.size == rhs.size);
	}
};

class Font {
public:
	static TTF_Font* getFontByName(std::string name, int size)
	{
		static std::vector<avgFont>* mapOfFonts;
		if (!mapOfFonts)
			mapOfFonts = new std::vector<avgFont>();
		std::cout << "loading " << ("assets/graphical/fonts/" + name + ".ttf") << std::endl;
		avgFont* found = NULL; 
		for (avgFont& font : (*mapOfFonts))
		{
			if (font.name == name && font.size == size)
			{
				TTF_CloseFont(font.font);
				found = &font;
			}
		}
		if (found)
			mapOfFonts->erase(std::remove(mapOfFonts->begin(), mapOfFonts->end(), (*found)));
		avgFont f;
		f.font = TTF_OpenFont(("assets/graphical/fonts/" + name + ".ttf").c_str(), size);
		f.name = name;
		f.size = size;
		mapOfFonts->push_back(f);
		return TTF_OpenFont(("assets/graphical/fonts/" + name + ".ttf").c_str(), size);
	}
};