#pragma once
#include "includes.h"
#include "Object.h"
#include "Pack.h"
#include "Text.h"
#include "Rendering.h"
#include "Noteskin.h"
#include "Game.h"
#include "AvgGroup.h"

struct wheelObject {
	Song s;
	bool drawBanner = false;
	Texture* banner;
	Text* title;
	Text* artist;
};

typedef void(__cdecl* wheelSelectCallback)(int);

class AvgWheel : public Object {
public:
	std::vector<wheelObject> wheelObjects;

	Texture* box;
	Texture* topBox;

	wheelSelectCallback callSelect;

	Shader* shad;

	int selectedIndex = 0;
	float fakeIndex = 0;

	AvgWheel(int _x, int _y, int _w, int _h)
	{
		MUTATE_START
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		box = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/wheelContainer.png");
		topBox = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/wheelTop.png");

		MUTATE_END
	}

	~AvgWheel()
	{
		for (wheelObject& obj : wheelObjects)
		{
			delete obj.artist;
			if (obj.drawBanner)
				delete obj.banner;
			delete obj.title;
		}
		wheelObjects.clear();
	}


	void keyDown(SDL_KeyboardEvent ev)
	{
		if (wheelObjects.size() == 0)
			return;

		switch (ev.keysym.sym)
		{
		case SDLK_DOWN:
			selectedIndex++;

			if (selectedIndex > wheelObjects.size() - 1)
				selectedIndex = 0;

			callSelect(selectedIndex);
			break;
		case SDLK_UP:
			selectedIndex--;

			if (selectedIndex < 0)
				selectedIndex = wheelObjects.size() - 1;

			callSelect(selectedIndex);
			break;
		}

	}

	void setSongs(std::vector<Song> songs)
	{
		MUTATE_START
		for (wheelObject& obj : wheelObjects)
		{
			if (obj.drawBanner)
				delete obj.banner;
			delete obj.artist;
			delete obj.title;
		}
		wheelObjects.clear();

		selectedIndex = 0;

		for (Song s : songs)
		{
			wheelObject obj;
			obj.s = s;

			if (s.c.meta.banner.size() != 0)
			{
				obj.drawBanner = true;
				obj.banner = Texture::loadTextureFromData(s.banner.data, s.banner.w, s.banner.h);
			}

			obj.artist = new Text(0, 0, s.c.meta.artist, 14, "arial");
			obj.artist->setCharacterSpacing(1.33);
			obj.title = new Text(0, 0, s.c.meta.songName, 16, "arialbd");
			obj.title->setCharacterSpacing(1.33);

			wheelObjects.push_back(obj);
		}

		MUTATE_END
	}


	void mouseWheel(float amount);

	void draw()
	{
		if (wheelObjects.size() == 0)
			return;

		Rect src;
		src.x = 0;
		src.y = 0;
		src.w = 1;
		src.h = 1;

		if (fakeIndex != selectedIndex)
			fakeIndex = std::lerp(fakeIndex, (float)selectedIndex, 0.1f);

		for (int i = 0; i < wheelObjects.size(); i++)
		{

			float realIndex = fakeIndex + 1;

			float away = (i + 1) - realIndex;

			if (std::abs(away) > 15)
				continue;

			float rank = away / realIndex;

			float xBasedOnRank = std::lerp(box->width / 2, 0.0f, std::abs(rank));

			if (selectedIndex != i)
				xBasedOnRank *= 0.6;

			Rect boxRect;
			boxRect.x = x - (box->width / 2) + xBasedOnRank;
			boxRect.y = (y + ((h / 2) - 80)) + ((box->height + 8) * away);

			boxRect.w = box->width;
			boxRect.h = box->height;

			Rect clipRect;

			clipRect.x = boxRect.x;
			clipRect.y = boxRect.y - 1;
			clipRect.w = boxRect.w + 1;
			clipRect.h = boxRect.h + 1;

			// bottom bounds for clipping, they cant be less than the wheel itself (or it'll clip into the pack stuff)

			if (clipRect.x < x)
			{
				clipRect.w = clipRect.w - (x - clipRect.x);
				clipRect.x = x;
			}
			if (clipRect.y - 1 < y - 1)
			{
				clipRect.h = clipRect.h - ((y - 1) - (clipRect.y - 1));
				clipRect.y = y - 1;
			}

			// basically takes the difference if its above the max and slices it off

			int fullHeight = clipRect.y + clipRect.h;
			int fullMaxHeight = (y - 1) + (h + 1);

			if (fullHeight > fullMaxHeight)
				clipRect.h = clipRect.h - (fullHeight - fullMaxHeight);


			if ((boxRect.y + boxRect.h < y) || (boxRect.y > y + h))
				continue;

			wheelObject& obj = wheelObjects[i];

			Rendering::SetClipRect(&clipRect);

			if (obj.drawBanner)
			{
				Rect background;
				background.x = boxRect.x;
				background.y = boxRect.y;

				background.w = obj.banner->width;
				background.h = obj.banner->height;

				if (background.w < boxRect.w)
					background.w = boxRect.w;

				if (background.h < boxRect.h)
					background.h = boxRect.h;

				Rendering::PushQuad(&background, &src, obj.banner, GL::genShader);

				background.r = 0;
				background.g = 0;
				background.b = 0;
				background.a = 0.5;
				Rendering::PushQuad(&background, &src, obj.banner, GL::genShader);

				Rendering::PushQuad(&boxRect, &src, topBox, GL::genShader);
			}
			else
			{
				Rendering::PushQuad(&boxRect, &src, box, GL::genShader);
				boxRect.r = 0;
				boxRect.g = 0;
				boxRect.b = 0;
				boxRect.a = 0.5;
				Rendering::PushQuad(&boxRect, &src, box, GL::genShader);
			}

			obj.artist->x = ((boxRect.x + boxRect.w) - (obj.artist->w)) - 14;
			obj.title->x = ((boxRect.x + boxRect.w) - (obj.title->w)) - 16;

			obj.title->y = boxRect.y + 2;
			obj.artist->y = obj.title->y + 18;

			obj.title->draw();
			obj.artist->draw();

			Rendering::SetClipRect(NULL);
		}
	}
};