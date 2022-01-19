#include "SettingsMenu.h"
#include "AvgSprite.h"
#include "Noteskin.h"



void SettingsMenu::refreshList() {
	selectedIndex = 0;
	for (bruhhh t : settings)
		removeObj(t.display);
	settings.clear();
	noteskins.clear();
	for (const auto& entry : std::filesystem::directory_iterator("assets/noteskin/"))
	{
		std::vector<std::string> spl = Chart::split(entry.path().string(), '/');
		std::string skinName = spl[spl.size() - 1];
		noteskins.push_back(skinName);
	}

	for (int i = 0; i < Game::save->settings.size(); i++)
	{
		setting& set = Game::save->settings[i];
		std::string value = "";
		if (set.takesActive)
			value = ": " + std::string((set.active ? " true" : " false"));
		else
			value = ": " + std::to_string(set.value);

		if (set.isKeybind)
			value = ": " + std::string(SDL_GetKeyName((SDL_Keycode)set.value));

		bruhhh h;

		Text* t = new Text(0, 20 + (64 * i), set.name + value, 24, "NotoSans-Regular");
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		t->create();

		h.display = t;
		h.set = &set;

		add(t);

		settings.push_back(h);
	}


	for (int i = 0; i < noteskins.size(); i++)
	{
		std::vector<std::string> spl = Chart::split(std::string(noteskins[i]), '/');
		std::string skinName = spl[spl.size() - 1];
		for (int s = 0; s < settings.size(); s++)
		{
			bruhhh& b = settings[s];
			std::cout << b.set->name << " " << b.set->stringValue << " vs " << skinName << std::endl;
			if (std::string(b.set->name) == "Noteskin" && std::string(b.set->stringValue) == skinName)
			{
				selectedNoteskinIndex = i;
				std::cout << "selected noteskin " << skinName << std::endl;
				std::string value = ": " + std::string(b.set->stringValue);
				b.display->setText(std::string(b.set->name) + value);
				b.display->setX((Game::gameWidth / 2) - (b.display->surfW / 2));
			}
		}
	}
}

void SettingsMenu::setSelect(bruhhh& b)
{
	Text* t = b.display;

	setting* set = b.set;

	std::string value = "";

	if (set->takesActive)
		value = ": " + std::string((set->active ? " true" : " false"));
	else
		value = ": " + std::to_string(set->value);

	if (set->isKeybind)
		value = ": " + std::string(SDL_GetKeyName((SDL_Keycode)set->value));

	if (std::string(b.set->name) == "Noteskin")
		value = ": " + std::string(b.set->stringValue);

	std::string newText = "> " + std::string(set->name) + value;
	t->setText(newText);
	t->setX((Game::gameWidth / 2) - (t->surfW / 2));
}

void SettingsMenu::updateText(bruhhh& b)
{
	Text* t = b.display;

	setting* set = b.set;

	std::string value = "";

	if (set->takesActive)
		value = ": " + std::string((set->active ? " true" : " false"));
	else
		value = ": " + std::to_string(set->value);

	if (set->isKeybind)
		value = ": " + std::string(SDL_GetKeyName((SDL_Keycode)set->value));

	if (std::string(b.set->name) == "Noteskin")
		value = ": " + std::string(b.set->stringValue);

	std::string newText = std::string(set->name) + value;
	t->setText(newText);
	t->centerX();
}

SettingsMenu::SettingsMenu()
{

}

void SettingsMenu::create() {
	addCamera(Game::mainCamera);
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
	add(sprite);
	AvgRect* rect = new AvgRect(0, 0, 1280, 720);
	rect->alpha = 0.3;
	add(rect);
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

	setSelect(settings[selectedIndex]);
}

void SettingsMenu::keyDown(SDL_KeyboardEvent event)
{
	if (settings.size() == 0)
		return;
	bruhhh& b = settings[selectedIndex];
	Text* t = b.display;
	std::string value = "";
	if (waitingKey)
	{
		Game::save->SetDouble(b.set->name, event.keysym.sym);
		Game::save->Save();
		updateText(b);
		waitingKey = false;
		return;
	}
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE:
		settings.clear();
		Game::instance->transitionToMenu(new MainMenu());
		break;
	case SDLK_RETURN:
		if (b.set->isKeybind && !waitingKey)
		{
			waitingKey = true;
			return;
		}
		else
		{
			if (b.set->takesActive)
			{
				std::cout << "toggle" << std::endl;
				Game::save->SetBool(b.set->name, !Game::save->GetBool(b.set->name));
				Game::save->Save();
				updateText(b);
			}
		}
		break;
	case SDLK_UP:
		if (waitingKey)
			return;
		selectedIndex--;

		if (b.set->takesActive)
			value = ": " + std::string((b.set->active ? " true" : " false"));
		else
			value = ": " + std::to_string(b.set->value);

		if (b.set->isKeybind)
			value = ": " + std::string(SDL_GetKeyName((SDL_Keycode)b.set->value));


		if (std::string(b.set->name) == "Noteskin")
			value = ": " + std::string(b.set->stringValue);

		t->setText(std::string(b.set->name) + value);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		break;
	case SDLK_DOWN:
		if (waitingKey)
			return;
		selectedIndex++;

		if (b.set->takesActive)
			value = ": " + std::string((b.set->active ? " true" : " false"));
		else
			value = ": " + std::to_string(b.set->value);

		if (b.set->isKeybind)
			value = ": " + std::string(SDL_GetKeyName((SDL_Keycode)b.set->value));


		if (std::string(b.set->name) == "Noteskin")
			value = ": " + std::string(b.set->stringValue);

		t->setText(std::string(b.set->name) + value);
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		break;
	case SDLK_LEFT:
		if (waitingKey || b.set->isKeybind)
			return;
		if (std::string(b.set->name) == "Noteskin")
		{
			selectedNoteskinIndex--;
			if (selectedNoteskinIndex < 0)
				selectedNoteskinIndex = noteskins.size() - 1;

			Game::save->SetString("Noteskin", noteskins[selectedNoteskinIndex]);
			Game::save->Save();
			updateText(b);
			return;
		}
		if (!b.set->takesActive)
		{
			double prev = b.set->value;
			if (b.set->increm == 0)
				b.set->value--;
			else
				b.set->value -= b.set->increm;

			std::cout << b.set->increm << std::endl;

			if (b.set->value < b.set->lowestValue)
				b.set->value = prev;
			Game::save->Save();

			updateText(b);
		}
		break;
	case SDLK_RIGHT:
		if (waitingKey || b.set->isKeybind)
			return;
		if (std::string(b.set->name) == "Noteskin")
		{
			selectedNoteskinIndex++;
			if (selectedNoteskinIndex > noteskins.size() - 1)
				selectedNoteskinIndex = 0;

			Game::save->SetString("Noteskin", noteskins[selectedNoteskinIndex]);
			Game::save->Save();
			updateText(b);
			return;
		}
		if (!b.set->takesActive)
		{
			double prev = b.set->value;
			if (b.set->increm == 0)
				b.set->value++;
			else
				b.set->value += b.set->increm;

			std::cout << b.set->increm << std::endl;

			if (b.set->value > b.set->highestValue)
				b.set->value = prev;
			Game::save->Save();

			updateText(b);
		}
		break;
	}
}
