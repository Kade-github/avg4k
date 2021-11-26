#include "SongSelect.h"
int SongSelect::selectedDiffIndex = 0;
Chart* SongSelect::currentChart = NULL;

// lol im not gonna put this in its own file
// CRY, COPE, SEETH, MALDDDDDDD
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


void SongSelect::updateList() {
	listOfCharts.clear();
	for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
	{
		// qp (and osu zip files)
		if (ends_with(entry.path().string(), ".qp"))
		{
			std::string newDir = Chart::split(entry.path().string(), '.')[0];
			std::cout << "creating " << newDir << std::endl;
			if (CreateDirectory(s2ws(newDir).c_str(), NULL))
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
				song s;
				s.type = Quaver;
				s.path = newDir;
				listOfCharts.push_back(s);
			}

		}
		else
		{
			for (auto& e : std::filesystem::directory_iterator(entry.path()))
			{
				std::string bruh = e.path().string();

				std::transform(bruh.begin(), bruh.end(), bruh.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (ends_with(bruh, ".sm"))
				{
					song s;
					s.type = StepMania;
					s.path = bruh;
					listOfCharts.push_back(s);
				}
				if (ends_with(bruh, ".qua"))
				{
					song s;
					s.type = Quaver;
					s.path = entry.path().string();
					listOfCharts.push_back(s);
					break;
				}
			}
		}
	}
}

void SongSelect::update(Events::updateEvent event)
{
}

void SongSelect::keyDown(SDL_KeyboardEvent event)
{
}

SongSelect::SongSelect()
{
}

void SongSelect::switchChart(song s)
{
	// cannot do switch due to "YOU CANNOT DEFINE VARIABLES IN SWITCHES OR SOMETHING"
	if (s.type == StepMania)
	{
		SMFile* filee = new SMFile(s.path);
		currentChart = new Chart(filee->meta);
		delete filee;
	}

	if (s.type == Quaver)
	{
		QuaverFile* file = new QuaverFile();
		chartMeta meta = file->returnChart(s.path);
		currentChart = new Chart(meta);
		delete file;
	}
}
