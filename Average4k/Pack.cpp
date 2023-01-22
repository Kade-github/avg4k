#include "Pack.h"
#include "Chart.h"
#include "SMFile.h"
#include "Texture.h"
#include "QuaverFile.h"
#include "Steam.h"
#include "Game.h"
#include "OsuFile.h"
#include "Average4k.h"
#include "MainerMenu.h"
#include <algorithm>
#include <execution>
bool SongGather::packAsyncAlready;
bool SongGather::steamRegAsyncAlready;

int SongGather::loaded = 0;

int SongGather::localId = 0;

int SongGather::max = 0;

std::list<int> smTimes = {};
std::list<int> quaTimes = {};
std::list<int> osuTimes = {};

std::string SongGather::currentPack;

std::mutex lock;

void SongGather::gatherPacksAsync()
{
	smTimes = {};
	quaTimes = {};
	osuTimes = {};
	if (!steamRegAsyncAlready)
	{
		localId = 0;
		loaded = 0;
		steamRegAsyncAlready = true;

		MainerMenu::asyncPacks.clear();

		currentPack = "";
		
		std::jthread t([]() {

			std::vector<std::filesystem::directory_entry> dirs = std::vector<std::filesystem::directory_entry>();

			for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
			{
				dirs.push_back(entry);
			}

			currentPack = "Local Packs";

			std::for_each(std::execution::par, dirs.begin(), dirs.end(), [](auto&& entry)
			{
				if (SongUtils::IsDirectory(entry.path()))
				{
					Pack p;

					std::ifstream fs;
					// meta
					fs.open(entry.path().string() + "/pack.meta");

					if (!fs.good())
						return;

					p.steamId = localId;
					p.isSteam = false;

					p.folder = entry.path().string();
					p.metaPath = entry.path().string() + "/pack.meta";

					std::string str;


					while (std::getline(fs, str))
					{
						if (!str.starts_with("#"))
						{
							std::vector<std::string> split = Chart::split(str, ':');
							std::string end = split[1].erase(0, 1);
							if (split[0] == "banner")
							{
								p.background = entry.path().string() + "/" + end;
								p.banner = Texture::getTextureData(p.background);
								p.hasBanner = true;
							}
							if (split[0] == "packName")
							{
								p.packName = end;

							}
							if (split[0] == "showName")
								p.showName = (end == "false" ? false : true);
						}
					}
					fs.close();


					std::vector<Song> songs = gatherSongsInFolder(entry.path().string() + "/");

					if (songs.size() == 0)
						return;

					loaded = 0;


					for (Song s : songs)
					{
						p.songs.push_back(s);
					}
					MainerMenu::packMutex.lock();
					MainerMenu::asyncPacks.push_back(p);
					MainerMenu::packMutex.unlock();
					loaded++;
					localId++;
				}
			});


			std::vector<steamItem> copy;

			std::copy(Game::steam->subscribedList.begin(), Game::steam->subscribedList.end(), std::back_inserter(copy));
			

			currentPack = "Workshop Packs";

			std::for_each(std::execution::par, copy.begin(), copy.end(), [](auto&& entry) {
				steamItem st = entry;

				if (!st.isPackFolder)
					return;

				Pack p;
				p.steamId = (uint64_t)st.details.m_nPublishedFileId;
				p.isSteam = true;

				std::string folder = std::string(st.folder);

				std::ifstream fs;
				// meta
				fs.open(folder + "/pack.meta");

				if (!fs.good())
					return;

				p.metaPath = folder + "/pack.meta";

				std::string str;

				while (std::getline(fs, str))
				{
					if (!str.starts_with("#"))
					{
						std::vector<std::string> split = Chart::split(str, ':');
						std::string end = split[1].erase(0, 1);
						if (split[0] == "banner")
						{
							p.background = folder + "/" + end;
							p.banner = Texture::getTextureData(p.background);
							p.hasBanner = true;
						}
						if (split[0] == "packName")
						{
							p.packName = end;
						}
						if (split[0] == "showName")
							p.showName = (end == "false" ? false : true);
					}
				}
				fs.close();
				std::vector<Song> songs = gatherSongsInFolder(folder + "/");

				if (songs.size() == 0)
					return;


				loaded = 0;

				for (Song s : songs)
				{
					s.steamIdPack = p.steamId;
					s.isSteam = true;
					p.songs.push_back(s);
				}
				
				MainerMenu::packMutex.lock();
				MainerMenu::asyncPacks.push_back(p);
				MainerMenu::packMutex.unlock();
				loaded++;
			});

			loaded = 0;

			currentPack = "Workshop Songs";

			std::for_each(std::execution::par, copy.begin(), copy.end(), [](auto&& entry) {

				steamItem st = entry;

				if (st.isPackFolder)
					return;

				Song s;

				s.isSteam = true;

				s.steamId = st.details.m_nPublishedFileId;

				std::string path(st.folder);

				if (path == "")
					return;

				std::string fullPath = path + "\\" + st.chartFile;

				std::string replaced = Steam::ReplaceString(fullPath, "\\", "/");
				std::string replaced2 = Steam::ReplaceString(path, "\\", "/");
				s.path = replaced;
				Chart c;
				if (st.chartType == "qv" || st.chartType == "qp")
				{
					QuaverFile f = QuaverFile();
					c = f.returnChart(replaced2);
				}
				if (st.chartType == "sm")
				{
					SMFile f = SMFile(s.path, replaced2, false);
					c = Chart();
					c.meta = f.meta;
				}

				if (s.c.meta.banner.size() > 0)
				{
					s.banner = Texture::getTextureData(s.c.meta.folder + "/" + s.c.meta.banner);
					s.hasBanner = true;
				}
				else
				{
					s.hasBanner = false;
				}

				s.c = c;
				MainerMenu::packMutex.lock();
				MainerMenu::asyncSongs.push_back(s);
				MainerMenu::packMutex.unlock();

				loaded++;
			});
			steamRegAsyncAlready = false;
			});
		t.detach();
	}
}

Pack SongGather::gatherPack(std::string filePath, bool checkForMod, bool isSteam)
{
	Pack p;

	std::ifstream fs;
	fs.open(filePath + "/pack.meta");

	if (!fs.good())
		return Pack();

	p.metaPath = filePath + "/pack.meta";

	std::string str;

	while (std::getline(fs, str))
	{
		if (!str.starts_with("#"))
		{
			std::vector<std::string> split = Chart::split(str, ':');
			std::string end = split[1].erase(0, 1);
			if (split[0] == "banner")
				p.background = filePath + "/" + end;
			if (split[0] == "packName")
				p.packName = end;
			if (split[0] == "showName")
				p.showName = (end == "false" ? false : true);
			p.isSteam = isSteam;
		}
	}


	std::vector<Song> songs = gatherSongsInFolder(filePath + "/");

	if (songs.size() == 0)
		return p;

	for (Song s : songs)
	{
		if (!checkForMod)
		{
			s.c.isModFile = false;
			s.c.pathToLua = "";
		}
		p.songs.push_back(s);
		s.isSteam = isSteam;
	}

	return p;
}


Chart SongGather::extractAndGetChart(std::string file)
{
	std::string newFile = file;
	std::vector<std::string> stuff = Chart::split(newFile, '\\');
	std::string newDir = stuff[stuff.size() - 1];

	newDir = newDir.substr(0, newDir.find_last_of("."));

	std::string s = Average4k::path + "/assets/charts/" + newDir;
	if (CreateDirectoryA(s.c_str(), NULL))
	{
		std::string path = file;
		char buf[100];
		struct zip_stat sb;
		struct zip_file* zf;
		int err = 0;
		long long sum;
		int len;
		int fd;
		zip* z = zip_open(path.c_str(), 0, &err);

		if (z == NULL)
		{
			zip_error_to_str(buf, sizeof(buf), err, errno);
			return Chart();
		}

		for (int i = 0; i < zip_get_num_entries(z, 0); i++) {
			if (zip_stat_index(z, i, 0, &sb) == 0) {
				zf = zip_fopen_index(z, i, 0);
				if (!zf)
					continue;

				std::ofstream fout;
				fout.open(s + "/" + sb.name, std::ios::binary);

				char* contents = new char[sb.size];
				zip_fread(zf, contents, sb.size);
				fout.write(contents, sb.size);
				fout.close();

				zip_fclose(zf);
			}
		}
		zip_close(z);
		remove(path.c_str());
		if (SongUtils::ends_with(file, ".qp"))
		{
			QuaverFile file = QuaverFile();
			chartMeta m = file.returnChart(s);
			return Chart(m);
		}
		else
		{
			OsuFile file = OsuFile(s);
			return Chart(file.meta);
		}
	}
	return Chart();
}

std::vector<Song> SongGather::gatherSongsInFolder(std::string folder)
{
	std::vector<Song> songs;

	std::mutex lock;
	if (folder == "")
		folder = "assets/charts/";


	std::vector<std::filesystem::directory_entry> dirs = std::vector<std::filesystem::directory_entry>();


	for (const auto& entry : std::filesystem::directory_iterator(folder))
	{
		dirs.push_back(entry);
	}
	std::for_each(std::execution::par, dirs.begin(), dirs.end(), [&songs, &lock](auto& entry)
	{
		Logging::writeLog("Loading " + entry.path().string());
		// qp (and osu zip files)
		try
		{
			if (SongUtils::ends_with(entry.path().string(), ".qp") || SongUtils::ends_with(entry.path().string(), ".osz"))
			{
				Chart c = extractAndGetChart(entry.path().string());
				if (c.meta.songName.size() != 0)
				{
					Song s;
					s.c = c;
					s.path = entry.path().string();
					lock.lock();
					songs.push_back(s);
					lock.unlock();
					loaded++;
				}
			}
			else
			{
				if (SongUtils::IsDirectory(entry.path()))
				{
					std::vector<std::string> smFiles;
					std::vector<std::string> quaverFiles;
					std::vector<std::string> osuFiles;

					for (auto& e : std::filesystem::directory_iterator(entry.path()))
					{
						std::string bruh = e.path().string();

						std::transform(bruh.begin(), bruh.end(), bruh.begin(),
							[](unsigned char c) { return std::tolower(c); });
						if (SongUtils::ends_with(bruh, ".sm") || SongUtils::ends_with(bruh, ".ssc"))
						{
							Song s;
							SMFile file = SMFile(bruh, entry.path().string(), false);
							s.c = Chart(file.meta);
							s.path = bruh;
							if (!file.dontUse)
							{
								lock.lock();
								songs.push_back(s);
								lock.unlock();
								loaded++;
							}

							return;
						}
						if (SongUtils::ends_with(bruh, ".qua"))
						{
							Song s;
							QuaverFile file = QuaverFile();
							chartMeta m = file.returnChart(entry.path().string());
							s.c = Chart(m);
							s.path = bruh;
							if (m.difficulties.size() != 0)
							{
								lock.lock();
								songs.push_back(s);
								lock.unlock();
								loaded++;
							}
							return;
						}
						if (SongUtils::ends_with(bruh, ".osu"))
						{
							Song s;
							OsuFile file = OsuFile(entry.path().string());
							s.c = Chart(file.meta);
							s.path = bruh;
							lock.lock();
							songs.push_back(s);
							lock.unlock();
							loaded++;
							return;
						}
					}
				}
			}
		}
		catch (...) {

		}
	});
	return songs;
}