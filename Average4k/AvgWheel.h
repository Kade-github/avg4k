#pragma once
#include "includes.h"
#include "Object.h"
#include "Pack.h"
#include "Rendering.h"
#include "Noteskin.h"
#include "Game.h"
#include "AvgGroup.h"

struct shit {
	unsigned char* backgroundData;
	int w, h;
	AvgSprite* spr;
	Song meta;
	Text* topText;
	Text* bottomText;
	int songIndex;
	bool created = false;
};


typedef void(__cdecl* songSelectCallback)(Song s);

bool istotalydeadashell = false;
bool pleaseSTOP = false;
bool requireUpdate = false;

class AvgWheel : public Object
{
public:
	std::vector<Song> songs;
	std::vector<Song> bangerSongs;
	std::vector<std::string> pleaseDelete;
	std::map<std::string, shit> wheels;
	std::map<std::string, shit> bangerWheels;

	std::mutex lock;

	Texture* backgroundImage;

	int actualValue = 0;
	float select = 0;

	void returnMinMax(int* min, int* max)
	{
		*min = actualValue - 12;
		*max = actualValue + 12;

		if (*min < 0)
			*min = 0;

		if (*max > songs.size())
			*max = songs.size();
	}

	void createStuff(shit* ss)
	{
		AvgSprite* spr = new AvgSprite(0, 0, new Texture(ss->backgroundData, ss->w, ss->h));

		ss->spr = spr;
		ss->topText = new Text(0, 0, ss->meta.c.meta.songName, 15, "arialbd");
		ss->topText->setCharacterSpacing(2);
		ss->bottomText = new Text(0, 0, ss->meta.c.meta.artist, 13, "arialbd");
		ss->bottomText->setCharacterSpacing(2);
		ss->created = true;
	}

	void addWheelAssets(int _songIndex)
	{
		if (songs.size() == 0) {
			return;
		}

		if (_songIndex >= songs.size()) {
			return;
		}

		Song s = songs[_songIndex];

		std::string path = s.c.meta.folder + "/" + s.c.meta.banner;

		if (s.c.meta.banner.size() == 0)
			path = s.c.meta.folder + "/" + s.c.meta.background;


		shit ss;

		unsigned char* bg = stbi_h::stbi_load_file_data(path, &ss.w, &ss.h);

		ss.backgroundData = bg;
		ss.songIndex = _songIndex;
		ss.meta = s;
		/*ss.spr = spr;
		ss.topText = new Text(0, 0, s.c.meta.songName, 15, "arialbd");
		ss.topText->setCharacterSpacing(2);
		ss.bottomText = new Text(0, 0, s.c.meta.artist, 13, "arialbd");
		ss.bottomText->setCharacterSpacing(2);
		ss.songIndex = _songIndex;*/

		wheels[s.c.meta.folder] = ss;
	}

	void addWheelAssetsForVisible()
	{
		int min, max;

		lock.lock();

		returnMinMax(&min, &max);

		std::map<std::string, shit>::iterator itr;

		std::vector<int> toSkip = {};
		
		for (itr = wheels.begin(); itr != wheels.end(); ++itr) {
			if (itr->second.created)
				if (itr->second.songIndex < min || itr->second.songIndex > max)
				{
					pleaseDelete.push_back(itr->first);
				}
				else
					toSkip.push_back(itr->second.songIndex);
		}
		

		for (int i = min; i < max; i++)
		{
			if (!std::binary_search(toSkip.begin(), toSkip.end(), i))
				addWheelAssets(i);
		}
		lock.unlock();
	}

	static int loop(void* ptr) {

		AvgWheel* fuckshit = reinterpret_cast<AvgWheel*>(ptr);

		while (!istotalydeadashell) {
			if (pleaseSTOP || !requireUpdate)
				continue;
			requireUpdate = false;
			fuckshit->addWheelAssetsForVisible();
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
		}
		return 0;
	}


	songSelectCallback call;

	void setSongs(std::vector<Song>* _songs)
	{
		pleaseSTOP = true;
		// yes freeze I know
		while (!lock.try_lock()) {}
		for (std::map<std::string, shit>::iterator iter = wheels.begin(); iter != wheels.end(); ++iter)
		{
			if (iter->second.created)
			{
				delete iter->second.spr;
				delete iter->second.topText;
				delete iter->second.bottomText;
			}
		}
		wheels.clear();
		songs.clear();

		actualValue = 0;
		select = 0;

		if (_songs == NULL)
			songs = {};
		else
			songs = *_songs;

		lock.unlock();
		pleaseSTOP = false;
		requireUpdate = true;

		if (songs.size() != 0)
			call(songs[0]);

	}

	AvgWheel(int _x, int _y, int _w, int _h, std::vector<Song>* _songs, songSelectCallback callback)
	{
		istotalydeadashell = false;
		call = callback;
		x = _x;
		y = _y;

		backgroundImage = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/wheelContainer.png");

		w = _w;
		h = _h;

		setSongs(_songs);

		std::thread t([&]() {
			loop(this);
		});
		t.detach();
	}

	~AvgWheel()
	{
		istotalydeadashell = true;
		pleaseSTOP = true;
		// yes freeze I know
		while (!lock.try_lock()) {}
		for (std::map<std::string, shit>::iterator iter = wheels.begin(); iter != wheels.end(); ++iter)
		{
			if (iter->second.created)
			{
				delete iter->second.spr;
				delete iter->second.topText;
				delete iter->second.bottomText;
			}
		}

		actualValue = 0;
		select = 0;

		lock.unlock();
		pleaseSTOP = false;
		wheels.clear();
		songs.clear();
		delete backgroundImage;
	}

	void selectThis(int toSelect)
	{
		if (songs.size() == 0)
			return;
		actualValue = toSelect;
		if (toSelect > bangerSongs.size() - 1)
			actualValue = 0;
		if (toSelect < 0)
			actualValue = bangerSongs.size() - 1;
		call(songs[actualValue]);
		requireUpdate = true;
	}

	void draw()
	{
		if (songs.size() == 0)
			return;
		Rect srcRect;
		srcRect.w = 1;
		srcRect.h = 1;


		Rect clip;


		if (select != actualValue)
		{
			select = std::lerp(select, actualValue, 0.1f);
			//std::cout << select << " " << actualValue << std::endl;
		}

		//Rendering::PushQuad(&clip, &srcRect, Rendering::white, GL::genShader);

		//addWheelAssetsForVisible();

		int min, max;

		returnMinMax(&min, &max);

		

		if (lock.try_lock())
		{
			//wheels = bangerWheels;
			bangerSongs = songs;

			for (std::string s : pleaseDelete)
			{
				if (wheels[s].created)
				{
					delete wheels[s].spr;
					delete wheels[s].topText;
					delete wheels[s].bottomText;
				}
				wheels.erase(s);
			}

			pleaseDelete.clear();

			lock.unlock();
		}
		//lock.lock();
		if (wheels.size() > 0)
			for (int i = 0; i < bangerSongs.size(); i++)
			{
				float away = (i + 1) - (select + 1);

				float rank = away / (select + 1);

				int yy = (y + h / 2) + 12 + ((backgroundImage->height + 12) * away);

				int songId = i;

				if (yy + backgroundImage->height < y || yy > y + h || (i < min || i > max))
					continue;


				float lerp = std::lerp(backgroundImage->width / 2, 0.0f, std::abs(rank));

				if (actualValue != i)
					lerp *= 0.6;

				int xx = x - (backgroundImage->width / 2) + (lerp);
				std::string name = bangerSongs[songId].c.meta.folder;

				if (!wheels.count(name))
				{
					continue;
				}


				if (!wheels[name].created)
				{
					createStuff(&wheels[name]);
				}
					


				Rect boxRect;

				if (xx < x)
					clip.x = x;
				else
					clip.x = xx;
				if (yy < y)
					clip.y = y;
				else
					clip.y = yy;



				AvgSprite* spr = wheels[name].spr;

				if (!spr || !wheels[name].topText || !wheels[name].bottomText)
					continue;


				if (spr->h > backgroundImage->height)
					clip.h = backgroundImage->height;
				else
					clip.h = spr->h;

				if (clip.y + clip.h > y + h)
					clip.h -= (clip.y + clip.h) - (y + h);

				boxRect.x = xx - 8;
				boxRect.y = yy - 8;
				boxRect.w = backgroundImage->width - 8;
				boxRect.h = backgroundImage->width - 8;

				spr->x = xx + (backgroundImage->width / 2);

				if (spr->x - spr->w > xx)
					spr->x = xx;

				spr->y = yy + (backgroundImage->height / 2);
				spr->x -= spr->w / 2;
				spr->y -= spr->h / 2;


				wheels[name].topText->x = xx + (backgroundImage->width - 15) - wheels[name].topText->w;
				wheels[name].topText->y = yy + 4;
				wheels[name].bottomText->x = xx + (backgroundImage->width - 15) - wheels[name].bottomText->w;
				wheels[name].bottomText->y = yy + 4 + wheels[name].topText->h;

				clip.w = backgroundImage->width - (x - xx);

				Rendering::SetClipRect(&clip);

				if (spr->w < backgroundImage->width)
					spr->w = backgroundImage->width;

				if (spr->h < backgroundImage->height)
					spr->h = backgroundImage->height;
				if (!wheels.count(name))
				{
					continue;
				}


				spr->draw();
				wheels[name].topText->draw();
				wheels[name].bottomText->draw();

				boxRect.x = xx;
				boxRect.y = yy;
				boxRect.w = backgroundImage->width;
				boxRect.h = backgroundImage->height;

				Rendering::SetClipRect(NULL);


				if (xx < x)
					clip.x = x;
				else
					clip.x = xx;
				if (yy < y)
					clip.y = y;
				else
					clip.y = yy;

				clip.w = backgroundImage->width;

				clip.h = backgroundImage->height;

				if (clip.y + clip.h > y + h)
					clip.h -= (clip.y + clip.h) - (y + h);

				Rendering::SetClipRect(&clip);
				Rendering::PushQuad(&boxRect, &srcRect, backgroundImage, GL::genShader);
				Rendering::SetClipRect(NULL);
			}
	}
};