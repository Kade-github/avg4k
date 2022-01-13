#pragma once
#include "includes.h"
#include <SDL_image.h>
#include <SDL2_gfxPrimitives.h>
class AvgSprite : public Object
{
public:
	SDL_Texture* tex;

	int w = 0;
	int h = 0;
	int borderSize = 4;
	Color borderColor;
	bool border = false;
	bool round = false;

	SDL_Rect clipRect;

	AvgSprite(int _x, int _y, std::string path) : Object(x, y) {
		x = _x;
		y = _y;
		tex = IMG_LoadTexture(Game::renderer, path.c_str());
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	};

	virtual ~AvgSprite()
	{
		for (Object* obj : children)
		{
			delete obj;
		}
		beforeDeath();
		die();
	}

	void changeOutTexture(SDL_Texture* n)
	{
		SDL_DestroyTexture(tex);
		tex = n;
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	}

	virtual void draw() {
		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		if (clipRect.x != 0 || clipRect.y != 0)
			SDL_RenderSetClipRect(Game::renderer, &clipRect);

		SDL_SetTextureAlphaMod(tex, alpha);

		SDL_RenderCopyF(Game::renderer, tex, NULL, &rect);

		if (clipRect.x != 0 || clipRect.y != 0)
			SDL_RenderSetClipRect(Game::renderer, NULL);

		if (border)
		{
			SDL_SetRenderDrawColor(Game::renderer, borderColor.r, borderColor.g, borderColor.b, 255);
			if (!round)
				SDL_RenderDrawRectF(Game::renderer, &rect);
			else
				roundedRectangleRGBA(Game::renderer, (x + w), y, x , (y + h), 4, borderColor.r, borderColor.g, borderColor.b, 255, 1);
		}

	}

	virtual void beforeDeath() {
		SDL_DestroyTexture(tex);
	}
};


