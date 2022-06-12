#include "Pack.h"
#include "Chart.h"
#include "SMFile.h"
#include "Texture.h"
#include "QuaverFile.h"
#include "Steam.h"
#include "Game.h"
#include "OsuFile.h"
#include "Average4k.h"

bool SongGather::packAsyncAlready;
bool SongGather::steamRegAsyncAlready;

std::mutex lock;

void SongGather::gatherPacksAsync(std::vector<Pack>* packs)
{
	if (!steamRegAsyncAlready)
	{
		steamRegAsyncAlready = true;
		std::thread t([packs]() {
			for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
			{
				if (SongUtils::IsDirectory(entry.path()))
				{
					Pack p;

					std::ifstream fs;
					// meta
					fs.open(entry.path().string() + "/pack.meta");

					if (!fs.good())
						continue;

					p.folder = entry.path().string();
					p.metaPath = entry.path().string() + "/pack.meta";

					std::string str;

					while (std::getline(fs, str))
					{
						if (str.starts_with("#"))
							continue;
						std::vector<std::string> split = Chart::split(str, ':');
						std::string end = split[1].erase(0, 1);
						if (split[0] == "banner")
							p.background = entry.path().string() + "/" + end;
						if (split[0] == "packName")
							p.packName = end;
						if (split[0] == "showName")
							p.showName = (end == "false" ? false : true);
					}


					std::vector<Song> songs = gatherSongsInFolder(entry.path().string() + "/");

					if (songs.size() == 0)
						continue;

					for (Song s : songs)
						p.songs.push_back(s);
					{
						std::lock_guard cock(lock);
						packs->push_back(p);
					}
				}
				steamRegAsyncAlready = false;
			}
			});
		t.detach();
	}
}

Pack SongGather::gatherPack(std::string filePath, bool checkForMod)
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
		if (str.starts_with("#"))
			continue;
		std::vector<std::string> split = Chart::split(str, ':');
		std::string end = split[1].erase(0, 1);
		if (split[0] == "banner")
			p.background = filePath + "/" + end;
		if (split[0] == "packName")
			p.packName = end;
		if (split[0] == "showName")
			p.showName = (end == "false" ? false : true);
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
	}

	return p;
}

std::vector<Pack> SongGather::gatherPacks()
{
	std::vector<Pack> packs;
	for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
	{
		if (SongUtils::IsDirectory(entry.path()))
		{
			Pack p;

			std::ifstream fs;
			// meta
			fs.open(entry.path().string() + "/pack.meta");

			if (!fs.good())
				continue;

			p.metaPath = entry.path().string() + "/pack.meta";

			std::string str;

			while (std::getline(fs, str))
			{
				if (str.starts_with("#"))
					continue;
				std::vector<std::string> split = Chart::split(str, ':');
				std::string end = split[1].erase(0, 1);
				if (split[0] == "banner")
					p.background = entry.path().string() + "/" + end;
				if (split[0] == "packName")
					p.packName = end;
				if (split[0] == "showName")
					p.showName = (end == "false" ? false : true);
			}


			std::vector<Song> songs = gatherSongsInFolder(entry.path().string() + "/");

			if (songs.size() == 0)
				continue;

			for (Song s : songs)
				p.songs.push_back(s);

			packs.push_back(p);
		}
	}
	return packs;
}

std::vector<Song> SongGather::gatherNoPackSongs()
{
	return gatherSongsInFolder();
}

Chart SongGather::extractAndGetChart(std::string file)
{
	std::string newFile = file;
	std::vector<std::string> stuff = Chart::split(newFile, '\\');
	std::string newDir = stuff[stuff.size() - 1];

	newDir = newDir.substr(0, newDir.find_last_of("."));

	std::string s = Average4k::path + "/assets/charts/" + newDir;
	std::cout << "creating " << s << std::endl;
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
			std::cout << "error opening zip archive " << path << " | " << buf << std::endl;
			return Chart();
		}

		for (int i = 0; i < zip_get_num_entries(z, 0); i++) {
			if (zip_stat_index(z, i, 0, &sb) == 0) {
				zf = zip_fopen_index(z, i, 0);
				if (!zf)
				{
					std::cout << "failed to open a file in a zip file " << path << std::endl;
					continue;
				}

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

void SongGather::gatherSteamPacksAsync(std::vector<Pack>* packs)
{
	if (!packAsyncAlready)
	{
		packAsyncAlready = true;
		std::thread t([packs]() {
			for (int i = 0; i < Game::steam->subscribedList.size(); i++)
			{
				steamItem st = Game::steam->subscribedList[i];

				if (!st.isPackFolder)
					continue;

				Pack p;
				p.steamId = (uint64_t) st.details.m_nPublishedFileId;
				p.isSteam = true;

				std::string folder = std::string(st.folder);

				std::ifstream fs;
				// meta
				fs.open(folder + "/pack.meta");

				if (!fs.good())
					continue;

				p.metaPath = folder + "/pack.meta";

				std::string str;

				while (std::getline(fs, str))
				{
					if (str.starts_with("#"))
						continue;
					std::vector<std::string> split = Chart::split(str, ':');
					std::string end = split[1].erase(0, 1);
					if (split[0] == "banner")
						p.background = folder + "/" + end;
					if (split[0] == "packName")
						p.packName = end;
					if (split[0] == "showName")
						p.showName = (end == "false" ? false : true);
				}


				std::vector<Song> songs = gatherSongsInFolder(folder + "/");

				if (songs.size() == 0)
					continue;

				for (Song s : songs)
				{
					s.steamIdPack = p.steamId;
					s.isSteam = true;
					p.songs.push_back(s);
				}
				{
					std::lock_guard cock(lock);
					packs->push_back(p);
				}
			}
			packAsyncAlready = false;
		});
		t.detach();
	}
}

void SongGather::gatherNoPackSteamSongsAsync(std::vector<Song>* songs)
{
	std::thread t([songs]() {
		for (int i = 0; i < Game::steam->subscribedList.size(); i++)
		{
			steamItem st = Game::steam->subscribedList[i];

			if (st.isPackFolder)
				continue;

			Song s;

			s.isSteam = true;

			s.steamId = st.details.m_nPublishedFileId;

			std::string path(st.folder);

			if (path == "")
				continue;
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

			s.c = c;
			songs->push_back(s);
		}
		});
	t.detach();
}

std::vector<Song> SongGather::gatherSongsInFolder(std::string folder)
{
	std::vector<Song> songs;
	if (folder == "")
		folder = "assets/charts/";
	for (const auto& entry : std::filesystem::directory_iterator(folder))
	{
		// qp (and osu zip files)
		if (SongUtils::ends_with(entry.path().string(), ".qp") || SongUtils::ends_with(entry.path().string(), ".osz"))
		{
			Chart c = extractAndGetChart(entry.path().string());
			if (c.meta.songName.size() != 0)
			{
				Song s;
				s.c = c;
				s.path = entry.path().string();
				songs.push_back(s);
			}
		}
		else
		{
			if (SongUtils::IsDirectory(entry.path()))
				for (auto& e : std::filesystem::directory_iterator(entry.path()))
				{
					std::string bruh = e.path().string();

					std::transform(bruh.begin(), bruh.end(), bruh.begin(),
						[](unsigned char c) { return std::tolower(c); });
					if (SongUtils::ends_with(bruh, ".sm"))
					{
						Song s;
						SMFile file = SMFile(bruh, entry.path().string(), false);
						s.c = Chart(file.meta);
						s.path = bruh;
						songs.push_back(s);
						break;
					}
					if (SongUtils::ends_with(bruh, ".qua"))
					{
						Song s;
						QuaverFile file = QuaverFile();
						chartMeta m = file.returnChart(entry.path().string());
						s.c = Chart(m);
						s.path = bruh;
						songs.push_back(s);
						break;
					}
					if (SongUtils::ends_with(bruh, ".osu"))
					{
						Song s;
						OsuFile file = OsuFile(entry.path().string());
						s.c = Chart(file.meta);
						s.path = bruh;
						songs.push_back(s);
						break;
					}
				}
		}
	}
	return songs;
}