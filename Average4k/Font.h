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

static std::vector<avgFont>* mapOfFonts;

class Font {
public:
	static void ClearAll()
	{
		if (mapOfFonts == NULL)
			mapOfFonts = new std::vector<avgFont>();
		for (avgFont& font : (*mapOfFonts))
			delete font.font;
		mapOfFonts->clear();
	}

	static void FreeFont(std::string name, int size)
	{
		if (mapOfFonts == NULL)
			mapOfFonts = new std::vector<avgFont>();
		int i = 0;
		for (avgFont& font : (*mapOfFonts))
		{
			if (font.name == name && font.size == size)
			{
				delete font.font;
				mapOfFonts->erase(mapOfFonts->begin() + i);
				break;
			}
			i++;
		}
	}
	static TTF_Font* getFontByName(std::string name, std::string folder, int size)
	{
		if (mapOfFonts == NULL)
			mapOfFonts = new std::vector<avgFont>();
		avgFont* found = NULL;
		for (avgFont& font : (*mapOfFonts))
		{
			if (font.name == name && font.size == size)
			{
				return font.font;
			}
		}
		avgFont f;
		f.font = TTF_OpenFont((folder + name + ".ttf").c_str(), size);
		f.name = name;
		f.size = size;
		mapOfFonts->push_back(f);
		return f.font;
	}
	static TTF_Font* getFontByName(std::string name, int size)
	{
		if (mapOfFonts == NULL)
			mapOfFonts = new std::vector<avgFont>();
		for (avgFont& font : (*mapOfFonts))
		{
			if (font.name == name && font.size == size)
			{
				return font.font;
			}
		}
		avgFont f;
		f.font = TTF_OpenFont(("assets/graphical/fonts/" + name + ".ttf").c_str(), size);
		f.name = name;
		f.size = size;
		mapOfFonts->push_back(f);
		return f.font;
	}
};