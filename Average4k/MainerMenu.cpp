#include "MainerMenu.h"
#include "SoundManager.h"
#include "AvgTextBox.h"
#include "Pack.h"
#include "AvgCheckBox.h"

AvgContainer* soloContainer;
AvgContainer* multiContainer;
AvgContainer* settingsContainer;

Pack selected;
std::vector<Pack> packs;

void MainerMenu::create()
{
	bg = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	bg->create();
	bg->w = 1286;
	bg->h = 726;
	add(bg);

	icon = new AvgSprite(16, 12, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	Rect clip;
	clip.x = 18;
	clip.y = 14;
	clip.w = 43;
	clip.h = 43;
	icon->clipRect = clip;

	hello = new Text(16 + ((icon->w / 2) * 2.6), 16, "Refreshing avatar data...", 16, "arialbd");
	hello->border = false;
	hello->create();
	add(hello);

	bottom = new Text(16 + ((icon->w / 2) * 2.6), 30, "", 16, "arialbd");
	bottom->border = false;
	bottom->create();
	add(bottom);

	border = new AvgSprite(16, 12, Noteskin::getMenuElement(Game::noteskin, "border.png"));
	border->create();
	add(border);

	soloContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/maincontainer_solo.png"));
	soloContainer->alpha = 1;
	soloContainer->x = (Game::gameWidth / 2) - (soloContainer->w / 2);
	add(soloContainer);

	// solo creation

	soloContainer->addObject(new Text(22, 18, "Packs", 18, "arialbd"), "packsText");
	((Text*)soloContainer->findItemByName("packsText"))->setCharacterSpacing(3);

	soloContainer->addObject(new Text(22, 36, "scroll down to see more", 16, "ariali"), "packsBottom");

	AvgContainer* packContainer = new AvgContainer(0, 88, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/packscontainer.png"));

	soloContainer->addObject(packContainer, "packContainer");


	// create packs

	packs = SongGather::gatherPacks();

	for (Pack p : packs)
	{
		addPack(p.packName, p.background, p.showName);
	}

	std::vector<Song> songs = SongGather::gatherNoPackSongs();

	if (songs.size() > 0)
	{
		Pack p;
		p.background = NULL;
		p.metaPath = "unfl";
		p.packName = "Unfiltered Songs";
		p.showName = true;
		p.songs = songs;

		addPack(p.packName, p.background, p.showName);
	}

	multiContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Multi/maincontainer.png"));
	multiContainer->x = (Game::gameWidth / 2) - (multiContainer->w / 2);
	multiContainer->active = false;
	add(multiContainer);

	// multi creation


	settingsContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/maincontainer.png"));
	settingsContainer->x = (Game::gameWidth / 2) - (settingsContainer->w / 2);
	settingsContainer->active = false;
	add(settingsContainer);

	// settings creation

	settingsContainer->addObject(new AvgTextBar(32, 48, "Search", Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/settingssearch.png")), "searchBox");

	// create settings

	std::vector<setting> gameplaySettings;
	gameplaySettings.push_back(Game::save->getSetting("scrollspeed"));
	gameplaySettings.push_back(Game::save->getSetting("downscroll"));
	gameplaySettings.push_back(Game::save->getSetting("offset"));
	gameplaySettings.push_back(Game::save->getSetting("keybinds"));

	addSettings("Gameplay", gameplaySettings);


	std::vector<setting> appearnSettings;
	appearnSettings.push_back(Game::save->getSetting("Note Size"));
	appearnSettings.push_back(Game::save->getSetting("Noteskin"));

	addSettings("Apperance", appearnSettings);


	std::vector<setting> audioSettings;
	audioSettings.push_back(Game::save->getSetting("Hitsounds"));

	addSettings("Audio", audioSettings);


	// create text stuff

	selectSolo = new AvgRect(soloContainer->x, soloContainer->y - 26, 232, 5);
	selectSolo->c = { 83, 234, 209 };

	soloText = new Text(0, selectSolo->y - 20, "solo", 24, "ANDALEMO");
	soloText->color = { 255,255,255 };
	soloText->setCharacterSpacing(3);
	soloText->x = selectSolo->x + (selectSolo->w / 2) - (soloText->w / 2);


	add(selectSolo);
	add(soloText);

	selectMulti = new AvgRect(soloContainer->x + (soloContainer->w / 2), soloContainer->y - 26, 232, 5);
	selectMulti->c = { 83, 234, 209 };
	selectMulti->x -= selectMulti->w / 2;
	selectMulti->alpha = 0;

	multiText = new Text(0, selectMulti->y - 20, "multiplayer", 24, "ANDALEMO");
	multiText->color = { 255,255,255 };
	multiText->setCharacterSpacing(3);
	multiText->x = selectMulti->x + (selectMulti->w / 2) - (multiText->w / 2);


	add(selectMulti);
	add(multiText);

	selectSettings = new AvgRect((soloContainer->x + soloContainer->w) - 232, soloContainer->y - 26, 232, 5);
	selectSettings->c = { 83, 234, 209 };
	selectSettings->alpha = 0;

	settingsText = new Text(0, selectSettings->y - 20, "settings", 24, "ANDALEMO");
	settingsText->color = { 255,255,255 };
	settingsText->setCharacterSpacing(3);
	settingsText->x = selectSettings->x + (selectSettings->w / 2) - (settingsText->w / 2);


	add(selectSettings);
	add(settingsText);

	currentContainer = soloContainer;

	Tweening::TweenManager::createNewTween("movingContainer2", settingsContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingContainer1", multiContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingContainer", soloContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
}

void MainerMenu::update(Events::updateEvent ev)
{
	selectSolo->y = soloContainer->y - 26;
	soloText->y = selectSolo->y - 28;

	selectMulti->y = soloContainer->y - 26;
	multiText->y = selectSolo->y - 28;

	selectSettings->y = soloContainer->y - 26;
	settingsText->y = selectSettings->y - 28;

	if (Multiplayer::loggedIn && !started)
	{
		started = true;
		hello->setText("Hi " + std::string(SteamFriends()->GetPersonaName()));
		bottom->setText("Avg4k indev-" + Game::version);
		Tweening::TweenManager::createNewTween("bottoms", bottom, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
		Texture* tex = Steam::getAvatar(Multiplayer::currentUserAvatar.c_str());
		if (tex)
		{
			delete icon->tex;
			icon->tex = tex;
			icon->h = 47;
			icon->w = 47;
		}
		Tweening::TweenManager::createNewTween("icons", icon, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
	}

	int x, y;
	Game::GetMousePos(&x, &y);

	if ((x > selectSolo->x && y > soloText->y) && (x < selectSolo->x + selectSolo->w && y < selectSolo->y))
	{
		if (selectSolo->alpha != 1)
			selectSolo->alpha = 0.5;
	}
	else
	{
		if (selectSolo->alpha != 1)
			selectSolo->alpha = 0;
	}
	if ((x > selectMulti->x && y > multiText->y) && (x < selectMulti->x + selectMulti->w && y < selectMulti->y))
	{
		if (selectMulti->alpha != 1)
			selectMulti->alpha = 0.5;
	}
	else
	{
		if (selectMulti->alpha != 1)
			selectMulti->alpha = 0;
	}
	if ((x > selectSettings->x && y > settingsText->y) && (x < selectSettings->x + selectSettings->w && y < selectSettings->y))
	{
		if (selectSettings->alpha != 1)
			selectSettings->alpha = 0.5;
	}
	else
	{
		if (selectSettings->alpha != 1)
			selectSettings->alpha = 0;
	}
}

void MainerMenu::keyDown(SDL_KeyboardEvent event)
{
}

int packIndex = 0;


void MainerMenu::addPack(std::string name, Texture* background, bool showText)
{
	AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
	PackObject* obj = new PackObject(0, packIndex * 75, name, background, showText);
	obj->w = packContainer->w;
	obj->h = 75;
	packContainer->addObject(obj, "packInd" + packIndex);
	packIndex++;
}

void MainerMenu::clearPacks()
{
	packIndex = 0;
	AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
	for (Object* obj : packContainer->above)
	{
		packContainer->removeObject(obj);
		delete obj;
	}
}

void MainerMenu::selectPack(int index)
{
	AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
	int ind = 0;

	selected = packs[index];

	for (Object* obj : packContainer->above)
	{
		PackObject* p = (PackObject*)obj;
		if (ind == index)
			p->selected = true;
		else
			p->selected = false;

		ind++;
	}

	// show songs
}

int lastTrans = 0;
int transToContainer = 0;
int despawn = 0;
void transContainerThing()
{
	MainerMenu* instance = (MainerMenu*)Game::currentMenu;
	switch (despawn)
	{
	case 0:
		soloContainer->active = false;
		break;
	case 1:
		multiContainer->active = false;
		break;
	case 2:
		settingsContainer->active = false;
		break;
	}
}

void MainerMenu::selectContainer(int container)
{
	transToContainer = container;
	despawn = lastTrans;
	if (transToContainer <= lastTrans)
		Tweening::TweenManager::createNewTween("movingContainer", currentContainer, Tweening::tt_X, 750, currentContainer->x, Game::gameWidth, (Tweening::tweenCallback)transContainerThing, Easing::EaseOutCubic);
	else
		Tweening::TweenManager::createNewTween("movingContainer", currentContainer, Tweening::tt_X, 750, currentContainer->x, -(currentContainer->w + 100), (Tweening::tweenCallback)transContainerThing, Easing::EaseOutCubic);
	switch (container)
	{
	case 0:
		currentContainer = soloContainer;
		soloContainer->active = true;
		selectSolo->alpha = 1;
		selectMulti->alpha = 0;
		selectSettings->alpha = 0;
		break;
	case 1:
		currentContainer = multiContainer;
		multiContainer->active = true;
		selectSolo->alpha = 0;
		selectMulti->alpha = 1;
		selectSettings->alpha = 0;
		break;
	case 2:
		currentContainer = settingsContainer;
		settingsContainer->active = true;
		selectSolo->alpha = 0;
		selectMulti->alpha = 0;
		selectSettings->alpha = 1;
		break;
	}
	if (transToContainer <= lastTrans)
		Tweening::TweenManager::createNewTween("movingContainer2", currentContainer, Tweening::tt_X, 750, -(currentContainer->w + 100), (Game::gameWidth / 2) - (currentContainer->w / 2), NULL, Easing::EaseOutCubic);
	else
		Tweening::TweenManager::createNewTween("movingContainer2", currentContainer, Tweening::tt_X, 750, Game::gameWidth, (Game::gameWidth / 2) - (currentContainer->w / 2), NULL, Easing::EaseOutCubic);
	lastTrans = container;
}

void MainerMenu::leftMouseDown()
{
	int x, y;
	Game::GetMousePos(&x, &y);

	if ((x > selectSolo->x && y > soloText->y) && (x < selectSolo->x + selectSolo->w && y < selectSolo->y))
		selectContainer(0);
	if ((x > selectMulti->x && y > multiText->y) && (x < selectMulti->x + selectMulti->w && y < selectMulti->y))
		selectContainer(1);
	if ((x > selectSettings->x && y > settingsText->y) && (x < selectSettings->x + selectSettings->w && y < selectSettings->y))
		selectContainer(2);
}

int catIndex = 0;

int lastHeight = 0;

void MainerMenu::addSettings(std::string catNam, std::vector<setting> settings)
{
	int startY = settingsContainer->findItemByName("searchBox")->y + 42 + (52 * catIndex);
	int startX = settingsContainer->findItemByName("searchBox")->x;

	std::string name = "_catName:" + catNam;

	settingsContainer->addObject(new Text(startX, startY + lastHeight, catNam, 18, "arialbd"), name);
	((Text*)settingsContainer->findItemByName(name))->setCharacterSpacing(3);

	int setInd = 0;
	for (setting& set : settings)
	{
		settingsContainer->addObject(new Text(startX,startY + lastHeight + 52 + (52 * setInd), set.name, 16, "arial"), "_cat_" + catNam + "_item_" + set.name);
		int ww, hh;
		std::string itemName = "_cat_" + catNam + "_item_" + set.name;
		ww = settingsContainer->findItemByName(itemName)->w;
		hh = settingsContainer->findItemByName(itemName)->h;

		settingsContainer->findItemByName(itemName)->y -= (hh / 2) - 2;

		if (set.takesString)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_textBox";

			settingsContainer->addObject(new AvgTextBar(startX + ww + 24, hey, set.defaultString, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/typeinputcontainer.png")), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->toModify = set;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->spacedOut = 4;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->unique = set.unique;
		}

		if (set.takesActive)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_checkBox";

			settingsContainer->addObject(new AvgCheckBox(startX + ww + 24, hey, set.defaultActive), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgCheckBox*)settingsContainer->findItemByName(boxName))->toModify = set;
		}

		if (set.takesDouble)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_textBox";

			settingsContainer->addObject(new AvgTextBar(startX + ww + 24, hey, std::to_string(set.defaultDouble), Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/typeinputcontainer.png")), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->toModify = set;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->spacedOut = 4;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->suffix = set.settingSuffix;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->resyncText();
		}
		setInd++;
	}

	lastHeight += (52 * settings.size()) + 52;
}