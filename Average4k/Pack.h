#pragma once
#include "includes.h"
#include "Chart.h"
#include "Texture.h"
#include "QuaverFile.h"
#include "SMFile.h"

struct Song {
	std::string path;
	Chart c;
};

struct Pack {
	std::vector<Song> songs;
	std::string metaPath;
	Texture* background;
	std::string packName;
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
	static std::vector<Pack> gatherPacks()
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
						p.background = Texture::createWithImage(entry.path().string() + "/" + end);
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

	static std::vector<Song> gatherNoPackSongs()
	{
		return gatherSongsInFolder();
	}

	static std::vector<Pack> gatherSteamPacks()
	{

	}
	
	static std::vector<Song> gatherNoPackSteamSongs()
	{

	}

	static std::vector<Song> gatherSongsInFolder(std::string folder = "")
	{
		std::vector<Song> songs;
		if (folder == "")
			folder = "assets/charts/";
		for (const auto& entry : std::filesystem::directory_iterator(folder))
		{
			// qp (and osu zip files)
			if (SongUtils::ends_with(entry.path().string(), ".qp"))
			{
				std::string newDir = Chart::split(entry.path().string(), '.')[0];
				std::cout << "creating " << newDir << std::endl;
				if (CreateDirectory(SongUtils::s2ws(newDir).c_str(), NULL))
				{
					std::string path = entry.path().string();
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
						continue;
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
							fout.open(newDir + "/" + sb.name, std::ios::binary);

							char* contents = new char[sb.size];
							zip_fread(zf, contents, sb.size);
							fout.write(contents, sb.size);
							fout.close();

							zip_fclose(zf);
						}
					}
					zip_close(z);
					remove(path.c_str());
					Song s;
					QuaverFile file = QuaverFile();
					chartMeta m = file.returnChart(newDir);
					s.c = Chart(m);
					s.path = newDir;
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
							chartMeta m = file.returnChart(bruh);
							s.c = Chart(m);
							s.path = bruh;
							songs.push_back(s);
							break;
						}
					}
			}
		}
		return songs;
	}
};