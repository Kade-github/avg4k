#pragma once
#include "MainMenu.h"
#include "Game.h"
#include "Text.h"
#include "Gameplay.h"
#include "MultiplayerLobbies.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <limits.h>
#include <zip.h>
#include <fstream>
#include <windows.h>
#include <stdio.h>

MainMenu* MainMenu::instance;
Chart* MainMenu::currentChart;
float MainMenu::offset = 0;
int MainMenu::selectedDiffIndex = 0;
Text* mainMenuText;

Text* songSelect;
Text* diffSelected;


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


void MainMenu::updateList() {
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

void MainMenu::switchChart(song s)
{
	// cannot do switch due to "YOU CANNOT DEFINE VARIABLES IN SWITCHES OR SOMETHING"
	if (s.type == StepMania)
	{
		SMFile* filee = new SMFile(s.path, s.path, true);
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

MainMenu::MainMenu()
{
	instance = this;
	selectedDiffIndex = 0;
	mainMenuText = new Text(0, 0, "Average 4k (F1 for lobby list, F2 for settings)", 100, 50);
	mainMenuText->setX((Game::gameWidth / 2) - (mainMenuText->surfW / 2));
	mainMenuText->setY((Game::gameHeight / 2) - 250);
	mainMenuText->create();

	selectedIndex = 0;

	updateList();

	switchChart(listOfCharts[selectedIndex]);

	difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];

	songSelect = new Text(0, 0, "> " + currentChart->meta.songName, 100, 500);
	songSelect->setX((Game::gameWidth / 2) - (songSelect->surfW / 2));
	songSelect->setY((Game::gameHeight / 2) - songSelect->surfH);
	songSelect->create();

	diffSelected = new Text(0, 0, diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)", 100, 500);
	diffSelected->setX((Game::gameWidth / 2) - (diffSelected->surfW / 2));
	diffSelected->setY(songSelect->y + songSelect->surfH + 30);
	diffSelected->create();

}


void MainMenu::update(Events::updateEvent event)
{
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_RETURN)
	{
		Game::currentMenu = new Gameplay();
		mainMenuText->destroy();
		songSelect->destroy();
		diffSelected->destroy();
	}

	if (event.keysym.sym == SDLK_F1 && Multiplayer::loggedIn)
	{
		Game::currentMenu = new MultiplayerLobbies();
		mainMenuText->destroy();
		songSelect->destroy();
		diffSelected->destroy();
	}

	if (event.keysym.sym == SDLK_F2)
	{
		Game::currentMenu = new SettingsMenu();
		mainMenuText->destroy();
		songSelect->destroy();
		diffSelected->destroy();
	}

	if (event.keysym.sym == SDLK_RIGHT)
	{
		selectedDiffIndex++;
		if (selectedDiffIndex > currentChart->meta.difficulties->size() - 1)
			selectedDiffIndex = 0;
		difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];
		diffSelected->setText(diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)");
	}
	else if (event.keysym.sym == SDLK_LEFT)
	{
		selectedDiffIndex--;
		if (selectedDiffIndex < 0)
			selectedDiffIndex = currentChart->meta.difficulties->size() - 1;
		difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];
		diffSelected->setText(diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)");
	}

	if (event.keysym.sym == SDLK_UP)
	{
		selectedIndex--;

		selectedDiffIndex = 0;

		if (selectedIndex < 0)
			selectedIndex = listOfCharts.size() - 1;

		currentChart->destroy();

		switchChart(listOfCharts[selectedIndex]);
		try {
			difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];
			songSelect->setText("> " + currentChart->meta.songName);

			diffSelected->setText(diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)");
		}
		catch (...) {
			listOfCharts.erase(listOfCharts.begin() + selectedIndex);
			selectedIndex++;
			songSelect->setText("> Failed to load");
			diffSelected->setText("Select a different chart please");
			MessageBox(NULL, L"Failed to load the chart.", L"Failure!", MB_OK | MB_ICONQUESTION);
		}
	}

	if (event.keysym.sym == SDLK_DOWN)
	{
		selectedIndex++;

		selectedDiffIndex = 0;

		if (selectedIndex > listOfCharts.size() - 1)
			selectedIndex = 0;

		currentChart->destroy();

		switchChart(listOfCharts[selectedIndex]);

		try {
			difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];
			songSelect->setText("> " + currentChart->meta.songName);

			diffSelected->setText(diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)");
		}
		catch (...) {
			listOfCharts.erase(listOfCharts.begin() + selectedIndex);
			selectedIndex--;
			songSelect->setText("> Failed to load");
			diffSelected->setText("Select a different chart please");
			MessageBox(NULL, L"Failed to load the chart.", L"Failure!", MB_OK | MB_ICONQUESTION);
		}
	}

	songSelect->setX((Game::gameWidth / 2) - (songSelect->surfW / 2));
	songSelect->setY((Game::gameHeight / 2) - songSelect->surfH);

	diffSelected->setX((Game::gameWidth / 2) - (diffSelected->surfW / 2));
	diffSelected->setY(songSelect->y + songSelect->surfH + 30);
}
