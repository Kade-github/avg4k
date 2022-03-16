#include "MainerMenu.h"
#include "SoundManager.h"

#include "AvgContainer.h"
#include "Pack.h"

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

	icon = new AvgSprite(32, 32, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	Rect clip;
	clip.x = 34;
	clip.y = 34;
	clip.w = 43;
	clip.h = 43;
	icon->clipRect = clip;

	hello = new Text(32 + ((icon->w / 2) * 2.6), 36, "Refreshing avatar data...", 16, "arialbd");
	hello->border = false;
	hello->create();
	add(hello);

	bottom = new Text(32 + ((icon->w / 2) * 2.6), 54, "", 16, "arialbd");
	bottom->border = false;
	bottom->create();
	add(bottom);

	border = new AvgSprite(32, 32, Noteskin::getMenuElement(Game::noteskin, "border.png"));
	border->create();
	add(border);

	soloContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/maincontainer.png"));
	soloContainer->alpha = 1;
	soloContainer->x = (Game::gameWidth / 2) - (soloContainer->w / 2);
	add(soloContainer);

	// solo creation

	soloContainer->addObject(new Text(22, 18, "Packs", 18, "arialbd"), "packsText");
	((Text*)soloContainer->findItemByName("packsText"))->setCharacterSpacing(3);
	
	soloContainer->addObject(new Text(22, 36, "scroll down to see more", 16, "ariali"), "packsBottom");

	AvgContainer* packContainer = new AvgContainer(0, 88, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/leftcontainer.png"));

	soloContainer->addObject(packContainer, "packContainer");

	// create packs

	packs = SongGather::gatherPacks();

	for (Pack p : packs)
	{
		addPack(p.packName, p.background, p.showName);
	}

	Tweening::TweenManager::createNewTween("movingContainer", soloContainer, Tweening::tt_Y, 1000, Game::gameHeight, 120, NULL, Easing::EaseOutCubic);
}

void MainerMenu::update(Events::updateEvent ev)
{
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

void MainerMenu::leftMouseDown()
{

}
