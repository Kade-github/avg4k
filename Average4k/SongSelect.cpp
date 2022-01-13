#include "SongSelect.h"
#include "Gameplay.h"
#include "MultiplayerLobby.h"
#include "AvgSprite.h"
int SongSelect::selectedDiffIndex = 0;
Chart* SongSelect::currentChart = NULL;
song* SongSelect::selectedSong = NULL;
int SongSelect::lastChecked = 0;

void SongSelect::updateList() {
	listOfCharts.clear();
	if (!MultiplayerLobby::inLobby)
	{
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
					s.steam = false;
					strcpy_s(s.path, newDir.c_str());
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
						s.steam = false;
						strcpy_s(s.folder, entry.path().string().c_str());
						strcpy_s(s.path, bruh.c_str());
						listOfCharts.push_back(s);
					}
					if (ends_with(bruh, ".qua"))
					{
						song s;
						s.type = Quaver;
						s.steam = false;
						strcpy_s(s.path, entry.path().string().c_str());
						listOfCharts.push_back(s);
						break;
					}
				}
			}
		}
	}

	if (Multiplayer::loggedIn)
		for (int i = 0; i < Game::steam->subscribedList.size(); i++)
		{
			steamItem st = Game::steam->subscribedList[i];

			song s;
			
			if (st.chartType == "sm")
				s.type = StepMania;
			if (st.chartType == "qv" || st.chartType == "qp")
				s.type = Quaver;

			s.steam = true;
			
			s.steamHandle = st.details.m_nPublishedFileId;

			std::string path(st.folder);

			if (path == "")
				continue;

			if (s.type == StepMania)
			{
				std::string fullPath = path + "\\" + st.chartFile;

				std::string replaced = Steam::ReplaceString(fullPath, "\\", "/");
				std::string replaced2 = Steam::ReplaceString(path, "\\", "/");
				strcpy_s(s.folder, replaced2.c_str());
				strcpy_s(s.path, replaced.c_str());
			}
			else
				strcpy_s(s.path, path.c_str());
			listOfCharts.push_back(s);
		}

	selectedIndex = 0;
	selectedDiffIndex = 0;

	infoText->setText("F5 to refresh - " + std::to_string(listOfCharts.size()) + " Charts loaded");
	infoText->centerX();


	if (listOfCharts.size() != 0)
	{

		switchChart(listOfCharts[0]);

		song& selectedChart = listOfCharts[selectedIndex];
		std::string name = currentChart->meta.songName;
		std::string diff = currentChart->meta.difficulties[selectedDiffIndex].name;

		songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
		songName->centerX();

		Color c;
		if (listOfCharts[selectedIndex].steam)
		{
			c.r = 128;
			c.g = 128;
			c.b = 255;
		}
		else
		{
			c.r = 128;
			c.g = 255;
			c.b = 128;
		}
		steam->color = c;

		steam->setText(listOfCharts[selectedIndex].steam ? "Steam Workshop" : "Local Storage (Press shift to upload)");
		steam->centerX();
	}
}

void SongSelect::update(Events::updateEvent event)
{
	if (!currentChart)
		return;
	
	// auto refresh when steam charts are added (more or less for the start)
	if (Game::steam->subscribedList.size() != lastChecked && Multiplayer::loggedIn)
	{
		lastChecked = Game::steam->subscribedList.size();
		updateList();
	}
}

void SongSelect::keyDown(SDL_KeyboardEvent event)
{
	if (listOfCharts.size() == 0)
	{
		currentChart = NULL;

		if (Multiplayer::loggedIn)
			Game::steam->populateSubscribedItems();
		updateList();
	}
	else
	{
		Color c;
		song selectedChart = listOfCharts[selectedIndex];
		std::string name = currentChart->meta.songName;
		std::string diff = currentChart->meta.difficulties[selectedDiffIndex].name;
		switch (event.keysym.sym)
		{
		case SDLK_LSHIFT:
			if (!currentChart || !allowMove)
				return;
			if (selectedSong->steam)
				return;
			allowMove = false;
			Color c;
			c.r = 128;
			c.g = 128;
			c.b = 128;
			steam->color = c;

			steam->setText("Checking, hold on...");
			steam->centerX();
			Game::steam->doesWorkshopItemExist(currentChart->meta.songName);
			break;
		case SDLK_DOWN:
			if (!allowMove)
				return;
			selectedIndex++;
			selectedDiffIndex = 0;

			if (selectedIndex > listOfCharts.size() - 1)
				selectedIndex = 0;

			if (currentChart)
				currentChart->destroy();

			selectedChart = listOfCharts[selectedIndex];

			switchChart(selectedChart);

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();

			if (listOfCharts[selectedIndex].steam)
			{
				c.r = 128;
				c.g = 128;
				c.b = 255;
			}
			else
			{
				c.r = 128;
				c.g = 255;
				c.b = 128;
			}

			steam->color = c;

			steam->setText(listOfCharts[selectedIndex].steam ? "Steam Workshop" : "Local Storage (Press shift to upload)");
			steam->centerX();

			break;
		case SDLK_UP:
			if (!allowMove)
				return;
			selectedIndex--;
			selectedDiffIndex = 0;

			if (selectedIndex < 0)
				selectedIndex = listOfCharts.size() - 1;

			selectedChart = listOfCharts[selectedIndex];


			if (currentChart)
				currentChart->destroy();

			switchChart(selectedChart);

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();

			if (listOfCharts[selectedIndex].steam)
			{
				c.r = 128;
				c.g = 128;
				c.b = 255;
			}
			else
			{
				c.r = 128;
				c.g = 255;
				c.b = 128;
			}

			steam->color = c;

			steam->setText(listOfCharts[selectedIndex].steam ? "Steam Workshop" : "Local Storage (Press shift to upload)");
			steam->centerX();


			break;
		case SDLK_LEFT:
			if (!currentChart || !allowMove)
				return;
			selectedDiffIndex--;
			if (selectedDiffIndex < 0)
				selectedDiffIndex = currentChart->meta.difficulties.size() - 1;

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();


			break;
		case SDLK_RIGHT:
			if (!currentChart || !allowMove)
				return;
			selectedDiffIndex++;
			if (selectedDiffIndex > currentChart->meta.difficulties.size() - 1)
				selectedDiffIndex = 0;

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();

			break;
		case SDLK_F5:
			if (!allowMove)
				return;
			currentChart = NULL;

			if (Multiplayer::loggedIn)
				Game::steam->populateSubscribedItems();
			updateList();
			break;
		case SDLK_RETURN:
			if (!allowMove)
				return;
			if (MultiplayerLobby::inLobby && currentChart)
			{
				Game::instance->transitionToMenu(new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, true));
			}
			else if (currentChart)
			{
				Game::instance->transitionToMenu(new Gameplay());
				free(selectedSong);
			}
			break;
		case SDLK_ESCAPE:
			if (MultiplayerLobby::inLobby)
				Game::instance->transitionToMenu(new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, false));
			else
			{
				Game::instance->transitionToMenu(new MainMenu());
			}
			delete currentChart;
			free(selectedSong);
			break;
		}
	}
}

void SongSelect::onSteam(std::string s)
{
	if (s == "createdItem")
	{
		// ok then lets fuckin do it lol!

		std::string fullPath = selectedSong->path;

		std::string replaced2 = Steam::ReplaceString(selectedSong->path, "\\", "/");

		std::vector<std::string> split = Chart::split(replaced2, '/');

		std::cout << "uploading to " << Game::steam->createdId << std::endl;

		Game::steam->uploadToItem(currentChart, Game::steam->createdId, split[split.size() - 1]);

		Color c;
		c.r = 128;
		c.g = 128;
		c.b = 255;
		steam->color = c;

		steam->setText("Uploading please wait...");
		steam->centerX();

	}

	if (s == "failedItem")
	{
		allowMove = true;
		Color c;
		c.r = 255;
		c.g = 128;
		c.b = 128;
		steam->color = c;

		steam->setText("Failed to upload! (please give ur log.txt to lemon or kade)");
		steam->centerX();
	}

	if (s == "uploadItem")
	{
		allowMove = true;

		Color c;
		c.r = 128;
		c.g = 255;
		c.b = 128;
		steam->color = c;

		steam->setText("Uploaded! (should be at the bottom of the list, press F5 if it isn't)");
		steam->centerX();
	}

	if (s == "resultDoesntExist")
	{
		Game::steam->createWorkshopItem();
	}

	if (s == "resultExists")
	{
		allowMove = true;
		Color c;
		c.r = 255;
		c.g = 128;
		c.b = 128;
		steam->color = c;

		steam->setText("That song has already been uploaded! Change the name of the song.");
		steam->centerX();
	}
}

SongSelect::SongSelect()
{
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/bg.png");
	sprite->create();
	add(sprite);

	songName = new Text(Game::gameWidth / 2,(Game::gameHeight / 2) - 200,"No songs loaded",24, "NotoSans-Regular");
	songName->centerX();
	songName->centerY();

	songName->create();
	add(songName);

	if (!currentChart)
		delete currentChart;

	currentChart = NULL;

	infoText = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) - 275, "", 24, "NotoSans-Regular");
	infoText->create();
	add(infoText);
	steam = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) - 170, "", 24, "NotoSans-Regular");
	steam->create();
	add(steam);

	selectedSong = (song*)malloc(sizeof(song)); // REMEMBER TO FREE THIS KADE PLEASE (- kade from the past)

	if (Multiplayer::loggedIn)
		Game::steam->populateSubscribedItems();

	updateList();
}

void SongSelect::switchChart(song s)
{
	// cannot do switch due to "YOU CANNOT DEFINE VARIABLES IN SWITCHES OR SOMETHING"

	int time = SDL_GetTicks();

	if (s.type == StepMania)
	{
		SMFile* filee = new SMFile(s.path, s.folder, false);
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

	if (selectedSong)
	{
		free(selectedSong);
		selectedSong = (song*)malloc(sizeof(song));
	}

	strcpy_s(selectedSong->path, listOfCharts[selectedIndex].path);
	selectedSong->steam = s.steam;
	if (s.steam)
		selectedSong->steamHandle = listOfCharts[selectedIndex].steamHandle;
	selectedSong->type = listOfCharts[selectedIndex].type;

	std::cout << "took " << (SDL_GetTicks() - time) << "ms for " << s.type << std::endl;
}
