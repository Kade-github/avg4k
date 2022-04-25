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

class AvgWheel : public Object
{
public:
	std::vector<Song> songs;

	std::map<std::string, shit> wheels;

	std::mutex lock;

	Texture* backgroundImage;

	float actualValue = 0;
	float select = 0;

	void returnMinMax(int* min, int* max)
	{
		*min = actualValue - 3;
		*max = actualValue + 3;

		if (*min < 0)
			*min = 0;

		if (*max > songs.size() - 1)
			*max = songs.size() - 1;
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
			path = s.c.meta.folder + "/" + "bn.png"; // common banner name


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

		std::vector<std::string> toDelete;
		
		for (itr = wheels.begin(); itr != wheels.end(); ++itr) {
			if (itr->second.songIndex < min || itr->second.songIndex > max)
			{
				delete itr->second.spr;
				delete itr->second.topText;
				delete itr->second.bottomText;
				toDelete.push_back(itr->first);
			}
			else
				toSkip.push_back(itr->second.songIndex);
		}
		


		for (std::string s : toDelete)
			wheels.erase(s);
		toDelete.clear();

		for (int i = min; i < max; i++)
		{
			if (std::find(toSkip.begin(), toSkip.end(), i) != toSkip.end()) {
				continue;
			}
			addWheelAssets(i);
		}
		lock.unlock();
	}

	static int loop(void* ptr) {

		AvgWheel* fuckshit = reinterpret_cast<AvgWheel*>(ptr);

		while (!istotalydeadashell) {
			fuckshit->addWheelAssetsForVisible();
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
		}
		return 0;
	}


	songSelectCallback call;

	void setSongs(std::vector<Song>* _songs)
	{
		lock.lock();
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
		lock.unlock();

		addWheelAssetsForVisible();

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

		std::thread t([&]() {
			loop(this);
		});
		t.detach();
	}

	~AvgWheel()
	{
		istotalydeadashell = true;
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


		if (!lock.try_lock())
			return;
		//lock.lock();
		for (int i = 0; i < songs.size(); i++)
		{
			float away = (i + 1) - (select + 1);

			float rank = away / (select + 1);

			int yy = (y + h / 2) + 12 + ((backgroundImage->height + 12) * away);

			int songId = i;

			if (yy + backgroundImage->height < y || yy > y + h || (i < min || i > max))
				continue;


			float lerp = std::lerp(backgroundImage->width / 2, 0.0f, std::abs(rank));

			if (rank < 0.98)
				lerp *= 0.6;

			int xx = x - (backgroundImage->width / 2) + (lerp);
			std::string name = songs[songId].c.meta.folder;

			if (!wheels[name].created)
			{
				createStuff(&wheels[name]);
			}

			Rect boxRect;

			if (xx + 8 < x)
				clip.x = x;
			else
				clip.x = xx + 8;
			if (yy < y)
				clip.y = y;
			else
				clip.y = yy;

			if (backgroundImage->height > h)
				clip.h = h;
			else
				clip.h = backgroundImage->height;

			if (clip.y + clip.h > y + h)
				clip.h -= (clip.y + clip.h) - (y + h);

			boxRect.x = xx + 8;
			boxRect.y = yy + 8;
			boxRect.w = backgroundImage->width - 8;
			boxRect.h = backgroundImage->width - 8;


			wheels[name].spr->x = xx;
			wheels[name].spr->y = yy;

			wheels[name].topText->x = xx + (backgroundImage->width - 15) - wheels[name].topText->w;
			wheels[name].topText->y = yy + 4;
			wheels[name].bottomText->x = xx + (backgroundImage->width - 15) - wheels[name].bottomText->w;
			wheels[name].bottomText->y = yy + 4 + wheels[name].topText->h;

			clip.w = backgroundImage->width - (x - xx);

			Rendering::SetClipRect(&clip);

			wheels[name].spr->draw();
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

			if (backgroundImage->width > w)
				clip.w = w;
			else
				clip.w = backgroundImage->width;
			if (backgroundImage->height > h)
				clip.h = h;
			else
				clip.h = backgroundImage->height;

			if (clip.y + clip.h > y + h)
				clip.h -= (clip.y + clip.h) - (y + h);

			Rendering::SetClipRect(&clip);
			Rendering::PushQuad(&boxRect, &srcRect, backgroundImage, GL::genShader);
			Rendering::SetClipRect(NULL);
		}
		lock.unlock();
	}
};