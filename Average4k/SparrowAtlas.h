#pragma once
#include "includes.h"
#include "pugixml.hpp"
#include <SDL_image.h>
#include "Game.h"
struct sparrowFrame {
	std::map<std::string, SDL_FRect> frameVector;
	
};

class SparrowAtlas
{
public:
	SDL_Texture* t;

	std::map<std::string,sparrowFrame> frames;

	std::string currentAnim;

	float x;
	float y;
	int w;
	int h;

	int startTime;

	bool loop;

	bool finished;

	int frame;
	int frameCount;

	int fps;


	SparrowAtlas(std::string path, std::string xmlPath, int _fps, std::string startAnim) {
		t = IMG_LoadTexture(Game::renderer, path.c_str());
		pugi::xml_document doc;
		doc.load_file(xmlPath.c_str());

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
			rect.w = node.attribute("width").as_int();
			rect.h = node.attribute("height").as_int();


			sf.frameVector[name.substr(name.size() - 4, 4)] = rect;


			frames[firstPart] = sf;
		}

		currentAnim = startAnim;
		w = frames[currentAnim].frameVector["0000"].w;
		h = frames[currentAnim].frameVector["0000"].h;

		play(currentAnim, _fps);
	}

	void play(std::string name, int newFPS)
	{
		startTime = SDL_GetTicks();
		currentAnim = name;
		fps = newFPS;
		finished = false;
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


		SDL_Rect source;
		source.x = frames[currentAnim].frameVector[buffer].x;
		source.y = frames[currentAnim].frameVector[buffer].y;
		source.w = frames[currentAnim].frameVector[buffer].w;
		source.h = frames[currentAnim].frameVector[buffer].h;
		SDL_FRect dest;
		dest.w = source.w != w ? w : source.w;
		dest.h = source.h != h ? h : source.h;
		dest.x = x;
		dest.y = y;

		SDL_RenderCopyF(Game::renderer, t, &source, &dest);
	}


};