#pragma once
#include "MainMenu.h"
#include "Game.h"
#include "Text.h"
#include "Gameplay.h"
#include <string>
#include <iostream>
#include <filesystem>

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

void MainMenu::updateList() {
	listOfCharts.clear();
	for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
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

void MainMenu::switchChart(song s)
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

MainMenu::MainMenu()
{
	instance = this;
	selectedDiffIndex = 0;
	mainMenuText = new Text(0, 0, "Average 4k (Hi " + std::string(SteamFriends()->GetPersonaName()) + ")", 100, 50);
	mainMenuText->setX((Game::gameWidth / 2) - (mainMenuText->surfaceMessage->w / 2));
	mainMenuText->setY((Game::gameHeight / 2) - 250);
	mainMenuText->create();

	selectedIndex = 0;

	updateList();

	switchChart(listOfCharts[selectedIndex]);

	difficulty diff = (*currentChart->meta.difficulties)[selectedDiffIndex];

	songSelect = new Text(0, 0, "> " + currentChart->meta.songName, 100, 500);
	songSelect->setX((Game::gameWidth / 2) - (songSelect->surfaceMessage->w / 2));
	songSelect->setY((Game::gameHeight / 2) - songSelect->surfaceMessage->h);
	songSelect->create();

	diffSelected = new Text(0, 0, diff.name + " (" + std::to_string(diff.notes->size()) + " NOTES) (" + std::to_string((*currentChart->meta.bpms)[0].bpm) + " BPM)", 100, 500);
	diffSelected->setX((Game::gameWidth / 2) - (diffSelected->surfaceMessage->w / 2));
	diffSelected->setY(songSelect->y + songSelect->surfaceMessage->h + 30);
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
		mainMenuText->die();
		songSelect->die();
		diffSelected->die();
	}

	// downscroll toggle

	if (event.keysym.sym == SDLK_1)
	{
		Game::save->SetBool("downscroll", !Game::save->GetBool("downscroll"));
		std::cout << "downscroll is now " << Game::save->GetBool("downscroll") << std::endl;
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

	songSelect->setX((Game::gameWidth / 2) - (songSelect->surfaceMessage->w / 2));
	songSelect->setY((Game::gameHeight / 2) - songSelect->surfaceMessage->h);

	diffSelected->setX((Game::gameWidth / 2) - (diffSelected->surfaceMessage->w / 2));
	diffSelected->setY(songSelect->y + songSelect->surfaceMessage->h + 30);
}
