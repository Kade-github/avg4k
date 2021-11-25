#include "SettingsMenu.h"

void SettingsMenu::refreshList() {
	selectedIndex = 0;
	for (bruhhh t : settings)
		t.display->destroy();
	settings.clear();
	for (int i = 0; i < Game::save->settings.size(); i++)
	{
		setting& set = Game::save->settings[i];
		std::string value = "";
		if (set.takesActive)
			value = ": " + set.active;
		else
			value = ": " + std::to_string(set.value);

		bruhhh h;

		Text* t = new Text(0, 100 + (64 * i), set.name + value, 10, 10);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		t->create();

		h.display = t;
		h.set = &set;

		settings.push_back(h);
	}
}

SettingsMenu::SettingsMenu()
{
	refreshList();
}

void SettingsMenu::update(Events::updateEvent event)
{
	if (settings.size() == 0)
	{
		return;
	}
	if (selectedIndex > settings.size() - 1)
		selectedIndex = 0;
	if (selectedIndex < 0)
		selectedIndex = settings.size() - 1;

	bruhhh& b = settings[selectedIndex];

	Text* t = b.display;

	setting* set = b.set;

	std::string value = "";

	if (set->takesActive)
		value = ": " + std::string((set->active ? " true" : " false"));
	else
		value = ": " + std::to_string(set->value);

	t->setText("> " + std::string(set->name) + value);
	t->setX((Game::gameWidth / 2) - (t->surfW / 2));
}

void SettingsMenu::keyDown(SDL_KeyboardEvent event)
{
	if (settings.size() == 0)
		return;
	bruhhh& b = settings[selectedIndex];
	Text* t = b.display;
	std::string value = "";
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE:
		for (bruhhh& t : settings)
			t.display->destroy();
		settings.clear();
		Game::currentMenu = new MainMenu();
		delete this;
		break;
	case SDLK_RETURN:
		if (b.set->takesActive)
		{
			std::cout << "toggle" << std::endl;
			Game::save->SetBool("downscroll", !Game::save->GetBool("downscroll"));
			Game::save->Save();
		}
		break;
	case SDLK_UP:
		selectedIndex--;

		if (b.set->takesActive)
			value = ": " + std::string((b.set->active ? " true" : " false"));
		else
			value = ": " + std::to_string(b.set->value);

		t->setText(std::string(b.set->name) + value);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		break;
	case SDLK_DOWN:
		selectedIndex++;

		if (b.set->takesActive)
			value = ": " + std::string((b.set->active ? " true" : " false"));
		else
			value = ": " + std::to_string(b.set->value);

		t->setText(std::string(b.set->name) + value);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		break;
	case SDLK_LEFT:
		if (!b.set->takesActive)
		{
			b.set->value--;
			Game::save->Save();
		}
		break;
	case SDLK_RIGHT:
		if (!b.set->takesActive)
		{
			b.set->value++;
			Game::save->Save();
		}
		break;
	}
}
