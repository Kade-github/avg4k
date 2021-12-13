#pragma once
#include "includes.h"
#include "pugixml.hpp"
#include <SDL_image.h>
#include "Game.h"

struct sframe {
	SDL_FRect rect;
	int offsetX;
	int offsetY;
};

struct sparrowFrame {
	std::map<std::string, sframe> frameVector;
	
};

class SparrowAtlas
{
public:
	SDL_Texture* t;

	std::map<std::string,sparrowFrame> frames;

	std::string currentAnim;
	std::string startingAnim;

	float x;
	float y;
	int w;
	int h;


	bool visible;

	bool showOnion;

	float scale;

	int startTime;

	int _offsetx;
	int _offsety;

	bool loop;

	bool finished;

	int frame;
	int frameCount;

	int fps;


	SparrowAtlas(std::string path, std::string xmlPath, int _fps, std::string startAnim) {
		t = IMG_LoadTexture(Game::renderer, path.c_str());
		pugi::xml_document doc;
		doc.load_file(xmlPath.c_str());
		startingAnim = startAnim;
		showOnion = false;
		visible = true;

		for (pugi::xml_node node : doc.child("TextureAtlas").children())
		{
			sparrowFrame sf;

			std::string name = node.attribute("name").as_string();

			std::string firstPart = name.substr(0, name.size() - 4);

			if (frames[firstPart].frameVector.size() != 0)
				sf = frames[firstPart];

			SDL_FRect rect;
			rect.x = node.attribute("x").as_int();
			rect.y = node.attribute("y").as_int();

			if (node.attribute("frameWidth").as_int() != 0)
				rect.w = node.attribute("frameWidth").as_int();
			if (node.attribute("frameHeight").as_int() != 0)
				rect.h = node.attribute("frameHeight").as_int();


			sframe f;

			f.rect = rect;
			f.offsetX = node.attribute("frameX").as_int();
			f.offsetY = node.attribute("frameY").as_int();


			sf.frameVector[name.substr(name.size() - 4, 4)] = f;


			frames[firstPart] = sf;
		}

		currentAnim = startAnim;
		w = frames[currentAnim].frameVector["0000"].rect.w;
		h = frames[currentAnim].frameVector["0000"].rect.h;

		scale = 1;

		play(currentAnim, _fps,0,0);
	}

	void play(std::string name, int newFPS, int offsetX, int offsetY)
	{
		startTime = SDL_GetTicks();
		currentAnim = name;
		fps = newFPS;
		finished = false;
		_offsetx = offsetX;
		_offsety = offsetY;
		frame = 0;
	}

	void left_fill_zeros(char* dest, const char* str, int length)
	{
		sprintf(dest, "%.*d%s", (int)(length - strlen(str)), 0, str);
	}

	void destroy()
	{
		SDL_DestroyTexture(t);
	}

	void draw()
	{
		char buffer[4];
		left_fill_zeros(buffer, std::to_string(frame).c_str(), 4);

		if (!finished || loop)
		{
			frame = ((SDL_GetTicks() - startTime) * fps / 1000);

			if (frame == (frames[currentAnim].frameVector.size() - 1) && !loop)
			{
				finished = true;
				std::cout << "finished!" << std::endl;
			}
			else
				frame = frame % (frames[currentAnim].frameVector.size() - 1);
		}

		int onionFrame = ((SDL_GetTicks() - startTime) * fps / 1000);
		onionFrame = onionFrame % (frames[startingAnim].frameVector.size() - 1);

		char obuffer[4];
		left_fill_zeros(obuffer, std::to_string(onionFrame).c_str(), 4);

		if (showOnion && visible)
		{
			sframe f = frames[startingAnim].frameVector[obuffer];

			SDL_Rect source;
			source.x = f.rect.x;
			source.y = f.rect.y;
			source.w = f.rect.w;
			source.h = f.rect.h;
			SDL_FRect dest;
			dest.w = source.w * scale;
			dest.h = source.h * scale;
			dest.x = x - f.offsetX;
			dest.y = y - f.offsetY;

			SDL_SetTextureAlphaMod(t, 128);
			SDL_RenderCopyF(Game::renderer, t, &source, &dest);
			SDL_SetTextureAlphaMod(t, 255);
		}

		{
			SDL_Rect source;
			source.x = frames[currentAnim].frameVector[buffer].rect.x;
			source.y = frames[currentAnim].frameVector[buffer].rect.y;
			source.w = frames[currentAnim].frameVector[buffer].rect.w;
			source.h = frames[currentAnim].frameVector[buffer].rect.h;
			SDL_FRect dest;
			dest.w = source.w * scale;
			dest.h = source.h * scale;
			dest.x = (x - _offsetx) - frames[currentAnim].frameVector[buffer].offsetX;
			dest.y = (y - _offsety) - frames[currentAnim].frameVector[buffer].offsetY;

			if (visible)
				SDL_RenderCopyF(Game::renderer, t, &source, &dest);
		}
	}


};