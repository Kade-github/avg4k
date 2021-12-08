#pragma once
#include "includes.h"
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "Chart.h"
#include "Game.h"
#include "Text.h"


enum chartType {
	StepMania = 0,
	Quaver = 1,
	Osu = 2
};

struct song {
	chartType type;
	bool steam;
	PublishedFileId_t steamHandle;
	std::string path;
};

class SongSelect :
    public Menu
{
public:
	SongSelect();

	static song* selectedSong;

    static Chart* currentChart;

	Text* infoText;

	static int lastChecked;

	static int selectedDiffIndex;

	bool ends_with(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	std::wstring s2ws(const std::string& s) // helper for converting std::string's to LPCWSTR
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}


	std::vector<song> listOfCharts;

	Text* songName;

	void switchChart(song s);

	int selectedIndex;

	void updateList();

	void update(Events::updateEvent event) override;
	void keyDown(SDL_KeyboardEvent event) override;
};

