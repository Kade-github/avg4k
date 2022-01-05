#pragma once
#include "includes.h"
#include <SDL_image.h>
class AvgSprite : public Object
{
public:
	SDL_Texture* tex;

	int w = 0;
	int h = 0;

	AvgSprite(int _x, int _y, std::string path) : Object(x, y) {
		tex = IMG_LoadTexture(Game::renderer, path.c_str());
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	};

	void draw() {
		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		SDL_SetTextureAlphaMod(tex, alpha);

		SDL_RenderCopyF(Game::renderer, tex, NULL, &rect);
	}

	void beforeDeath() {
		SDL_DestroyTexture(tex);
	}
};


