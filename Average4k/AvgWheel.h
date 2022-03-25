#pragma once
#include "includes.h"
#include "Object.h"
#include "Pack.h"
#include "Rendering.h"
#include "Noteskin.h"
#include "Game.h"
#include "AvgGroup.h"

struct shit {
	AvgSprite* spr;
	Text* topText;
	Text* bottomText;
};

typedef void(__cdecl* songSelectCallback)(Song s);

class AvgWheel : public Object
{
public:
	std::vector<Song> songs;

	std::map<std::string, shit> wheels;

	Texture* backgroundImage;

	float actualValue = 0;
	float select = 0;

	songSelectCallback call;

	void setSongs(std::vector<Song>* _songs)
	{
		for (std::map<std::string, shit>::iterator iter = wheels.begin(); iter != wheels.end(); ++iter)
		{
			delete iter->second.spr;
			delete iter->second.topText;
			delete iter->second.bottomText;
		}
		wheels.clear();
		songs.clear();

		actualValue = 0;
		select = 0;

		if (_songs == NULL)
			songs = {};
		else
			songs = *_songs;

		AvgGroup* grap = new AvgGroup(0, 0, 1280, 720);
		grap->renderOnce = true;

		// clip all backgrounds through some fb stuff


		for (Song s : songs)
		{
			Texture* bg = Texture::createWithImage(s.c.meta.folder + "/" + s.c.meta.banner);

			if (bg->width < backgroundImage->width)
				bg->width = backgroundImage->width;
			if (bg->height < backgroundImage->height)
				bg->height = backgroundImage->height;

			AvgSprite* spr = new AvgSprite(0, 0, bg);

			spr->x = (backgroundImage->width / 2) - ((bg->width) / 2);
			spr->y = (backgroundImage->height / 2) - ((bg->height) / 2);


			Rect bgClip;
			bgClip.w = backgroundImage->width;
			bgClip.h = backgroundImage->height;
			AvgRect* gray = new AvgRect(0, 0, 0, 0);
			gray->w = bg->width;
			gray->h = bg->height;
			gray->x = spr->x;
			gray->y = spr->y;
			gray->alpha = 0.5;
			grap->add(spr);
			grap->add(gray);
			Rendering::SetClipRect(&bgClip);
			grap->forceDraw();
			Rendering::SetClipRect(NULL);

			shit ss;
			ss.spr = new AvgSprite(0, 0, grap->ctb);
			ss.topText = new Text(0, 0, s.c.meta.songName, 15, "arialbd");
			ss.topText->setCharacterSpacing(2);
			ss.bottomText = new Text(0, 0, s.c.meta.artist, 13, "arialbd");
			ss.bottomText->setCharacterSpacing(2);
			wheels[s.c.meta.songName + s.c.meta.folder] = ss;
			wheels[s.c.meta.songName + s.c.meta.folder].spr->flip = true;
			grap->removeObj(spr);
			grap->removeObj(gray);
			delete grap;
			grap = new AvgGroup(0, 0, 1280, 720);
			grap->renderOnce = true;
		}
		if (grap)
			delete grap;

		if (songs.size() != 0)
			call(songs[0]);
	}

	AvgWheel(int _x, int _y, int _w, int _h, std::vector<Song>* _songs, songSelectCallback callback)
	{
		call = callback;
		x = _x;
		y = _y;

		backgroundImage = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/wheelContainer.png");

		w = _w;
		h = _h;

		setSongs(_songs);
	}

	~AvgWheel()
	{
		for (std::map<std::string, shit>::iterator iter = wheels.begin(); iter != wheels.end(); ++iter)
		{
			delete iter->second.spr;
			delete iter->second.topText;
			delete iter->second.bottomText;
		}
		wheels.clear();
		songs.clear();
	}

	void selectThis(int toSelect)
	{
		if (songs.size() == 0)
			return;
		actualValue = toSelect;
		if (actualValue > songs.size() - 1)
			actualValue = 0;
		if (actualValue < 0)
			actualValue = songs.size() - 1;
		call(songs[actualValue]);
	}

	void draw()
	{
		Rect srcRect;
		srcRect.w = 1;
		srcRect.h = 1;


		Rect clip;
		clip.x = x;
		clip.y = y;
		clip.w = w;
		clip.h = h;

		if (select != actualValue)
		{
			select = std::lerp(select, actualValue, 0.1f);
			//std::cout << select << " " << actualValue << std::endl;
		}

		//Rendering::PushQuad(&clip, &srcRect, Rendering::white, GL::genShader);

		for (int i = 0; i < songs.size(); i++)
		{
			float away = (i + 1) - (select + 1);

			float rank = away / (select + 1);

			int yy = (y + h / 2) + 12 + ((backgroundImage->height + 12) * away);

			if (yy + backgroundImage->height < y || yy > y + h)
				continue;

			float lerp = std::lerp(backgroundImage->width / 2, 0.0f, std::abs(rank));

			int xx = x - (backgroundImage->width / 2) + (lerp);
			std::string name = songs[i].c.meta.songName + songs[i].c.meta.folder;

			Rect boxRect;
			boxRect.x = xx + 8;
			boxRect.y = yy + 8;
			boxRect.w = backgroundImage->width - 8;
			boxRect.h = backgroundImage->height - 8;

			Rendering::SetClipRect(&clip);

			wheels[name].spr->x = xx;
			wheels[name].spr->y = yy;

			wheels[name].topText->x = xx + (backgroundImage->width - 15) - wheels[name].topText->w;
			wheels[name].topText->y = yy + 4;
			wheels[name].bottomText->x = xx + (backgroundImage->width - 15) - wheels[name].bottomText->w;
			wheels[name].bottomText->y = yy + 4 + wheels[name].topText->h;


			wheels[name].spr->draw();
			wheels[name].topText->draw();
			wheels[name].bottomText->draw();
			boxRect.x = xx;
			boxRect.y = yy;
			boxRect.w = backgroundImage->width;
			boxRect.h = backgroundImage->height;

			Rendering::PushQuad(&boxRect, &srcRect, backgroundImage, GL::genShader);

			Rendering::SetClipRect(NULL);
		}
	}
};