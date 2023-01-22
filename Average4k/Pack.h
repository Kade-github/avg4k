#pragma once
#include "includes.h"
#include "Chart.h"
#include "Texture.h"
struct Song {
	std::string path;
	bool isSteam = false;
	uint64_t steamId = -1;
	uint64_t steamIdPack = -1;
	texData banner;
	texData background;
	bool hasBanner = false;
	std::string folder;
	Chart c;
};

struct Pack {
	std::vector<Song> songs;
	std::string metaPath;
	std::string folder;
	std::string background;
	std::string packName;
	bool hasBanner = false;
	texData banner;
	uint64_t steamId = -1;
	bool isSteam = false;
	bool showName;
};

class SongUtils {
public:
	static bool ends_with(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	static std::wstring s2ws(const std::string& s) // helper for converting std::string's to LPCWSTR
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

	static bool IsDirectory(std::wstring path)
	{
		DWORD attrib = GetFileAttributes(path.c_str());

		if ((attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
			return true;

		return false;
	}
};

class SongGather {
public:
	static int localId;
	static std::string currentPack;
	static int max;
	static int loaded;
	static void gatherPacksAsync();
	static Pack gatherPack(std::string filePath, bool checkForMod = true, bool isSteam = false);


	static bool packAsyncAlready;
	static bool steamRegAsyncAlready;


	static Chart extractAndGetChart(std::string file);

	static std::vector<Song> gatherSongsInFolder(std::string folder = "");
};