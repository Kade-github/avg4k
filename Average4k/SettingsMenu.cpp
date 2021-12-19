#include "SettingsMenu.h"
#include "AvgSprite.h"

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
			value = ": " + std::string((set.active ? " true" : " false"));
		else
			value = ": " + std::to_string(set.value);

		bruhhh h;

		Text* t = new Text(0, 100 + (64 * i), set.name + value, 24);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		t->create();

		h.display = t;
		h.set = &set;

		add(t);

		settings.push_back(h);
	}
}

SettingsMenu::SettingsMenu()
{
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/bg.png");
	sprite->create();
	add(sprite);
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
		removeAll();
		settings.clear();
		Game::currentMenu = new MainMenu();
		delete this;
		break;
	case SDLK_RETURN:
		if (b.set->takesActive)
		{
			std::cout << "toggle" << std::endl;
			Game::save->SetBool(b.set->name, !Game::save->GetBool(b.set->name));
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
			if (b.set->value < b.set->lowestValue)
				b.set->value++;
			Game::save->Save();
		}
		break;
	case SDLK_RIGHT:
		if (!b.set->takesActive)
		{
			b.set->value++;
			if (b.set->value > b.set->highestValue)
				b.set->value--;
			Game::save->Save();
		}
		break;
	}
}
