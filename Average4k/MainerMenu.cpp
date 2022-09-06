#include "MainerMenu.h"
#include "SoundManager.h"
#include "AvgTextBox.h"
#include "Pack.h"
#include "AvgCheckBox.h"
#include "MultiplayerLobby.h"
#include "MainMenu.h"
#include "Gameplay.h"
#include "AvgDropDown.h"
#include "Average4k.h"
#include "CPacketServerList.h"
#include "SPacketServerListReply.h"
#include "CPacketJoinServer.h"
#include "SPacketFuckYou.h"
AvgContainer* MainerMenu::soloContainer;
AvgContainer* MainerMenu::multiContainer;
AvgContainer* MainerMenu::settingsContainer;
AvgContainer* MainerMenu::testWorkshop;

std::mutex packMutex;

Pack steamWorkshop;

int scrollLeaderboard = 0;


bool MainerMenu::isInMainerMenu = false;

bool MainerMenu::actuallyLoad = true;

int packIndex = 0;
int catIndex = 0;

int lastHeight = 0;

lobby MainerMenu::currentLobby;

Song MainerMenu::selectedSong;

Pack MainerMenu::selected;

bool MainerMenu::isInLobby = false;
bool MainerMenu::isHost = false;

int MainerMenu::packSongIndex = 0;
bool lobbyUp = false;


std::vector<Pack> MainerMenu::packs;

std::vector<Pack> MainerMenu::asyncPacks;
std::vector<Song> MainerMenu::asyncSongs;

AvgWheel* MainerMenu::wheel;

Chart MainerMenu::currentSelectedSong;
int MainerMenu::selectedDiffIndex = 0;

std::vector<steamItem> item;

int selectedContainerIndex = 0;

bool MainerMenu::lockInput = false;

bool chartUploading = false;


int lastTrans = 0;
int transToContainer = 0;
int despawn = 0;

void refreshLobbies() {
	VM_START
	std::cout << "refreshing lobbies" << std::endl;
	CPacketServerList list;
	list.Order = 0;
	list.PacketType = eCPacketServerList;

	Multiplayer::sendMessage<CPacketServerList>(list);
	VM_END
}

void shittyShitShit(std::string s)
{
	MainerMenu* menu = (MainerMenu*)Game::instance->currentMenu;
	menu->createNewLobbies(s);
}

void resetStuff()
{
	transToContainer = 0;
	despawn = 0;
	lobbyUp = false;
	lastTrans = 0;
	selectedContainerIndex = 0;

	MainerMenu* menu = ((MainerMenu*)Game::instance);
	menu->wheel->setSongs({});
	menu->clearPacks();
	lastHeight = 0;
	catIndex = 0;
}

void endTrans()
{
	resetStuff();
	Tweening::TweenManager::activeTweens.clear();
	Game::instance->switchMenu(new MainMenu());
}

void selectedSongCallback(int sId)
{
	MUTATE_START
	MainerMenu::packSongIndex = sId;
	if (MainerMenu::selected.songs.size() == 0)
		return;
	Song s = MainerMenu::selected.songs[sId];

	std::cout << "selected " << s.c.meta.songName << std::endl;

	AvgContainer* cont = (AvgContainer*)MainerMenu::soloContainer->findItemByName("songContainer");
	if (!cont) // lol
		return;



	MainerMenu::selectedDiffIndex = 0;

	MainerMenu::currentSelectedSong.destroy();

	MainerMenu::currentSelectedSong = s.c;
	MainerMenu::selectedSong = s;

	Tweening::TweenManager::removeTween("fuckyoutween");


	for (Object* obj : cont->above)
	{
		cont->removeObject(obj);
	}
	for (Object* obj : cont->below)
	{
		cont->removeObject(obj, true);
		if (obj)
			delete obj;
	}
	cont->items.clear();
	cont->above.clear();
	cont->below.clear();
	
	// bg
	AvgSprite* background = new AvgSprite(0, 0, Texture::createWithImage(s.c.meta.folder + "/" + s.c.meta.background));
	if (background->w < cont->w)
		background->w = cont->w;
	if (background->h < cont->h)
		background->h = cont->h;
	background->x = (cont->w / 2) - background->w / 2;
	background->y = (cont->h / 2) - background->h / 2;
	background->alpha = 0;

	Tweening::TweenManager::createNewTween("fuckyoutween", background, Tweening::tt_Alpha, 500, 0, 1, NULL, Easing::EaseInSine, false);

	cont->addObject(background, "background", true);

	// play song

	Channel* ch = SoundManager::getChannelByName("prevSong");

	if (ch != NULL)
	{
		ch->stop();
		ch->free();
		SoundManager::removeChannel("prevSong");
	}

	SoundManager::createChannelThread(MainerMenu::selectedSong.c.meta.folder + "/" + MainerMenu::selectedSong.c.meta.audio);

	// text stuff

	std::string display = s.c.meta.songName;
	std::string secondLine = "";

	if (display.size() > 25)
	{
		secondLine = display.substr(25, display.size());
		display = display.substr(0, 25);
		if (secondLine.size() > 22)
			secondLine = secondLine.substr(0, 22) + "...";
	}
	Text* title = new Text(12, 24, display, 18, "arialbd");
	title->setCharacterSpacing(3);
	Text* artist = new Text(12, 44, s.c.meta.artist, 14, "arial");
	artist->setCharacterSpacing(2.33);

	cont->addObject(title, "title");
	if (secondLine.size() != 0)
	{
		Text* title2 = new Text(12, 42, secondLine, 18, "arialbd");
		title2->setCharacterSpacing(3);
		cont->addObject(title2, "title2");
		artist->y += 20;
	}
	cont->addObject(artist, "artist");

	if (s.c.meta.difficulties.size() == 0)
	{
		std::string diffDisplay = "No Diffs :(";

		Text* diff = new Text(0, 0, diffDisplay, 20, "arialblk");
		diff->setCharacterSpacing(4.17);

		diff->x = (cont->w / 2) - (diff->w / 2);
		diff->y = 145;
		cont->addObject(diff, "diff");
	}
	else
	{

		std::string diffDisplay = s.c.meta.difficulties[0].name;
		if (diffDisplay.size() > 14)
			diffDisplay = diffDisplay.substr(0, 14) + "...";

		Text* diff = new Text(0, 0, diffDisplay, 20, "arialblk");
		diff->setCharacterSpacing(4.17);

		diff->x = (cont->w / 2) - (diff->w / 2);
		diff->y = 145;

		cont->addObject(diff, "diff");

		if (s.c.meta.difficulties.size() >= 2)
		{

			AvgSprite* diffSelectLeft = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/diffSelectArrow.png"));
			diffSelectLeft->y = 145 + ((diff->h / 2) - (diffSelectLeft->h / 2));
			diffSelectLeft->x = diff->x - 24;

			AvgSprite* diffSelectRight = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/diffSelectArrow.png"));
			diffSelectRight->y = 145 + ((diff->h / 2) - (diffSelectRight->h / 2));
			diffSelectRight->x = (diff->x + diff->w) + 24;
			diffSelectRight->flipX = true;


			cont->addObject(diffSelectLeft, "leftSelect");
			cont->addObject(diffSelectRight, "rightSelect");
		}
	}

	std::string type = "StepMania";

	switch (MainerMenu::selectedSong.c.meta.chartType)
	{
	case 1:
		type = "Quaver";
		break;
	case 2:
		type = "Osu!";
		break;
	}

	Text* chartType = new Text(0, 0, type, 14, "arial");
	chartType->setCharacterSpacing(4.17);

	chartType->x = 12;
	chartType->y = cont->h - 24;
	cont->addObject(chartType, "chartType");

	Text* localWorkshop = new Text(0, 0, MainerMenu::selectedSong.isSteam ? "Steam Workshop" : "Local", 14, "arial");
	localWorkshop->setCharacterSpacing(4.17);

	localWorkshop->x = ((cont->w) - localWorkshop->w) - 12;
	localWorkshop->y = cont->h - 24;
	cont->addObject(localWorkshop, "localWorkshop");

	if (MainerMenu::selectedSong.isSteam)
	{
		Text* leaderboardPt1 = new Text(cont->w / 2, cont->h - 100, "Press tab to", 14, "arial");
		Text* leaderboardPt2 = new Text(cont->w / 2, cont->h - 80, "view scores", 14, "arial");

		leaderboardPt1->x = (cont->w / 2) - (leaderboardPt1->w / 2);
		leaderboardPt2->x = (cont->w / 2) - (leaderboardPt2->w / 2);

		cont->addObject(leaderboardPt1, "leadpt1");
		cont->addObject(leaderboardPt2, "leadpt2");
	}

	MUTATE_END
}

void MainerMenu::create()
{
	isTransDone = true;
	fetchingScores = false;
	VM_START
	isInMainerMenu = true;
	shad = new Shader();

	currentLobby.LobbyID = 0;

	const char* vert = R"(
	in vec2 v_position;
	in vec2 v_uv;
	in vec4 v_colour;
	out vec2 f_uv;
	out vec4 f_colour;
	uniform mat4 u_projection;

	void main()
	{
		f_uv = v_uv;
		f_colour = v_colour;
		gl_Position = u_projection * vec4(v_position.xy, 0.0, 1.0);
	})";

	const char* frag = R"(

	precision mediump float;

	uniform sampler2D u_texture;
	uniform float radius;
	uniform float centerX;
	uniform float centerY;
	in vec2 f_uv;
	in vec4 f_colour;

	out vec4 o_colour;
	void main()
	{
		o_colour = texture(u_texture, f_uv) * f_colour;
		if (o_colour.a == 0.0)
			discard;

		vec2 center = vec2(centerX, centerY);
		vec2 dist = center - f_uv;
		if (length(dist) > radius)
			discard;
	})";

	shad->GL_CompileShader(vert, frag);
	shad->setProject(GL::projection);

	shad->SetUniform("centerX", 0.5f);
	shad->SetUniform("centerY", 0.5f);
	shad->SetUniform("radius", 0.8f);

	lobbyShader = new Shader();

	lobbyShader->GL_CompileShader(vert, frag);
	lobbyShader->setProject(GL::projection);

	lobbyShader->SetUniform("centerX", 0.5f);
	lobbyShader->SetUniform("centerY", 0.5f);
	lobbyShader->SetUniform("radius", 0.5f);

	Game::DiscordUpdatePresence("In the Main Menu", "Browsing Charts", "Average4K", -1, -1, "");
	SteamFriends()->SetRichPresence("gamestatus", "Browsing Charts");

	bg = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	bg->create();
	bg->w = 1286;
	bg->h = 726;
	add(bg);

	bool whiteScreen = false;

	if (stbi_h::get_error())
		whiteScreen = true;

	icon = new AvgSprite(16, 12, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	Rect clip;
	clip.x = 18;
	clip.y = 14;
	clip.w = 43;
	clip.h = 43;
	icon->clipRect = clip;
	icon->customShader = shad;
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

	testWorkshop = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/maincontainer_solo.png"));
	testWorkshop->active = false;
	testWorkshop->x = (Game::gameWidth / 2) - (testWorkshop->w / 2);
	testWorkshop->create();
	add(testWorkshop);

	testWorkshop->addObject(new Text(12, 12, "Average4K Workshop", 34, "arialbd"), "title");

	// output the first page of stuff


	soloContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/maincontainer_solo.png"));
	soloContainer->alpha = 1;
	soloContainer->x = (Game::gameWidth / 2) - (soloContainer->w / 2);
	soloContainer->create();
	add(soloContainer);

	// solo creation

	soloContainer->active = true;

	Texture* packSprite = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/packscontainer.png");

	wheel = (AvgWheel*)soloContainer->addObject(new AvgWheel(packSprite->width, 88, 600, packSprite->height), "wheelObject");

	wheel->callSelect = selectedSongCallback;

	soloContainer->addObject(new Text(22, 18, "Packs", 18, "arialbd"), "packsText");
	((Text*)soloContainer->findItemByName("packsText"))->setCharacterSpacing(3);

	soloContainer->addObject(new Text(22, 36, std::to_string(packs.size()) + " loaded", 16, "ariali"), "packsBottom");
	soloContainer->addObject(new Text(22, 54, "", 16, "ariali"), "uploadingProgress");
	((Text*)soloContainer->findItemByName("uploadingProgress"))->color = { 128,128,255 };

	AvgContainer* packContainer = new AvgContainer(0, 88, packSprite);

	soloContainer->addObject(packContainer, "packContainer");




	soloContainer->addObject(new AvgContainer((soloContainer->x + soloContainer->w), 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/songcontainer.png")), "songContainer");
	soloContainer->findItemByName("songContainer")->x -= soloContainer->findItemByName("songContainer")->w + 40;

	multiContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Multi/maincontainer.png"));
	multiContainer->x = (Game::gameWidth / 2) - (multiContainer->w / 2);
	multiContainer->active = false;
	multiContainer->create();
	add(multiContainer);

	// multi creation

	AvgContainer* filters = (AvgContainer*)multiContainer->addObject(new AvgContainer(0, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Multi/filtercontainer.png")), "filterContainer");
	lobbyContainer = (AvgContainer*)multiContainer->addObject(new AvgContainer(275, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Multi/lobbycontainer.png")), "lobbyContainer");

	Text* lb = (Text*)filters->addObject(new Text(12, 12, "Lobby", 18, "arialbd"), "lobbyText");
	lb->setCharacterSpacing(3);

	Text* searchText = (Text*)filters->addObject(new Text(12, 42, "search", 14, "arial"), "searchText");
	searchText->setCharacterSpacing(2.33);

	AvgTextBar* searchBox = (AvgTextBar*)filters->addObject(new AvgTextBar(12, 42 + searchText->h + 12, "", Noteskin::getMenuElement(Game::noteskin, "MainMenu/Multi/lobbysearch.png")), "lobbySearch");
	searchBox->callback = shittyShitShit;

	Text* sortText = (Text*)filters->addObject(new Text(12, searchBox->y + searchBox->h + 12, "sort by", 14, "arial"), "sortText");
	sortText->setCharacterSpacing(2.33);

	Text* filtersText = (Text*)filters->addObject(new Text(12 + sortText->w + 84, searchBox->y + searchBox->h + 12, "filters", 14, "arial"), "filtersText");
	filtersText->setCharacterSpacing(2.33);

	chat = new ChatObject(0, 0);
	chat->create();
	chat->showNotifs = false;


	settingsContainer = new AvgContainer(0, Game::gameHeight, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/maincontainer.png"));
	settingsContainer->x = (Game::gameWidth / 2) - (settingsContainer->w / 2);
	settingsContainer->active = false;
	settingsContainer->create();
	add(settingsContainer);

	// settings creation

	settingsContainer->addObject(new AvgTextBar(32, 48, "Search", Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/settingssearch.png")), "searchBox");

	// create settings

	std::vector<setting> gameplaySettings;
	gameplaySettings.push_back(Game::save->getSetting("FPS Limit"));
	gameplaySettings.push_back(Game::save->getSetting("scrollspeed"));
	gameplaySettings.push_back(Game::save->getSetting("downscroll"));
	gameplaySettings.push_back(Game::save->getSetting("offset"));
	gameplaySettings.push_back(Game::save->getSetting("keybinds "));
	gameplaySettings.push_back(Game::save->getSetting("Start Delay"));

	addSettings("Gameplay", gameplaySettings);


	std::vector<setting> appearnSettings;
	appearnSettings.push_back(Game::save->getSetting("Show Judgement Count"));
	appearnSettings.push_back(Game::save->getSetting("Note Size"));
	appearnSettings.push_back(Game::save->getSetting("Noteskin"));
	//appearnSettings.push_back(Game::save->getSetting("Resolution"));
	appearnSettings.push_back(Game::save->getSetting("Fullscreen"));
	appearnSettings.push_back(Game::save->getSetting("Auto Accent Colors"));

	appearnSettings.push_back(Game::save->getSetting("Accent Color R"));
	appearnSettings.push_back(Game::save->getSetting("Accent Color G"));
	appearnSettings.push_back(Game::save->getSetting("Accent Color B"));

	appearnSettings.push_back(Game::save->getSetting("Background Transparency"));
	appearnSettings.push_back(Game::save->getSetting("Lane Underway Transparency"));

	addSettings("Appearance", appearnSettings);


	std::vector<setting> audioSettings;
	audioSettings.push_back(Game::save->getSetting("Hitsounds"));
	audioSettings.push_back(Game::save->getSetting("Music Volume"));
	audioSettings.push_back(Game::save->getSetting("Hitsounds Volume"));

	addSettings("Audio", audioSettings);

	std::vector<setting> miscSettings;
	miscSettings.push_back(Game::save->getSetting("Submit Scores"));

	addSettings("Misc", miscSettings);


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

	resetStuff();


	if (!actuallyLoad)
	{
		for (Pack p : packs)
		{
			addPack(p.packName, p.banner, p.showName, p.isSteam);
		}
	}
	else
		loadPacks();

	Tweening::TweenManager::createNewTween("movingContainer3", testWorkshop, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingContainer2", settingsContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingContainer1", multiContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingContainer", soloContainer, Tweening::tt_Y, 1000, Game::gameHeight, 160, NULL, Easing::EaseOutCubic);
	
	refreshLobbies();

	if (isInLobby)
	{
		justJoined = true;


		Channel* ch = SoundManager::getChannelByName("prevSong");

		if (ch != NULL)
		{
			ch->stop();
			ch->free();
			SoundManager::removeChannel("prevSong");
		}
		ch = SoundManager::createChannel(Noteskin::getMusicElement(Game::noteskin, "MenuTheme.wav"), "prevSong");
		ch->play();

		ch->bpm = 155;
	}

	add(chat);

	for (Object* obj : settingsContainer->above)
	{
		obj->isActive = false;
	}

	VM_END
}

void MainerMenu::update(Events::updateEvent ev)
{

	MUTATE_START

	Channel* ch = SoundManager::getChannelByName("prevSong");

	if (ch != NULL)
	{
		if (ch->volume != Game::save->GetDouble("Music Volume"))
		{
			ch->setVolume(Game::save->GetDouble("Music Volume"));
		}
	}


	std::vector<Pack> gatheredPacks;
	std::vector<Song> gatheredSongs;
	if (!SongGather::steamRegAsyncAlready && !stop)
	{
		if (selectedContainerIndex == 0)
		{
			std::lock_guard cock(packMutex);
			{
				if (asyncPacks.size() != 0)
				{
					{
						for (Pack p : asyncPacks)
						{
							gatheredPacks.push_back(p);
						}
						asyncPacks.clear();

					}
				}
				if (asyncSongs.size() != 0)
				{
					{
						for (Song s : asyncSongs)
						{
							gatheredSongs.push_back(s);
						}
						asyncSongs.clear();
					}
				}
			}
			for (Pack p : gatheredPacks)
			{
				bool d = false;
				for (Pack pp : packs)
					if (pp.packName == p.packName)
					{
						if (!p.isSteam || pp.isSteam)
							d = true;
					}
				if (d)
					continue;
				addPack(p.packName, p.banner, p.showName, p.isSteam);
				packs.push_back(p);
			}
			for (Song s : gatheredSongs)
			{
				bool d = false;
				for (Song pp : steamWorkshop.songs)
					if (pp.c.meta.folder == s.c.meta.folder)
						d = true;
				if (d)
					continue;
				steamWorkshop.songs.push_back(s);
				for (Pack& p : packs)
					if (p.packName == "Workshop Songs")
						p.songs = steamWorkshop.songs;
			}
		}
		Text* t = (Text*)soloContainer->findItemByName("packsBottom");
		t->setText("Stuff was loaded!");
	}
	else
	{
		Text* t = (Text*)soloContainer->findItemByName("packsBottom");
		if (t->text != "Loading stuff (" + std::to_string(SongGather::loaded) + ")...")
			t->setText("Loading stuff (" + std::to_string(SongGather::loaded) + ")...");
	}

	if (uploading)
	{
		Text* t = ((Text*)soloContainer->findItemByName("uploadingProgress"));
		t->alpha = 1;
		float prog = Game::steam->CheckWorkshopProgress();
		std::string newTe = "Uploading " + selected.packName + " " + std::to_string(prog * 100).substr(0, 3) + "%";
		if (chartUploading) {
			newTe = "Uploading " + selectedSong.c.meta.songName + " " + std::to_string(prog * 100).substr(0, 3) + " % ";
		}
		if (prog != 0 && t->text != newTe)
		{
			t->setText(newTe);
		}
	}
	for (steamItem it : Game::steam->downloadedList)
	{
		bool dont = false;
		for (steamItem i : item)
			if (i.details.m_nPublishedFileId == it.details.m_nPublishedFileId)
				dont = true;
		if (dont)
			continue;
		item.push_back(it);
		std::string display = std::string(it.details.m_rgchTitle) + " - " + std::to_string(it.details.m_ulSteamIDOwner);

		testWorkshop->addObject(new Text(12, 42 + (20 * item.size()), display, 18, "arial"), "item_" + std::string(it.details.m_rgchTitle));
	}

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


		if (selectedSong.path.size() != 0)
		{
			if (SoundManager::getChannelByName("prevSong") == NULL)
			{
				if (SoundManager::isThreadDone)
				{
					if (SoundManager::threadLoaded != NULL)
					{
						std::string path = MainerMenu::selectedSong.c.meta.folder + "/" + MainerMenu::selectedSong.c.meta.audio;
						if (SoundManager::threadPath == path)
						{
							SoundManager::throwShitOntoVector(SoundManager::threadLoaded, "prevSong");
							Channel* real = SoundManager::getChannelByName("prevSong");
							real->play();
							real->loop = true;
							real->setPos(selectedSong.c.meta.start);
							SoundManager::threadLoaded = NULL;
						}
						else
						{
							SoundManager::threadLoaded = NULL;
						}
					}
				}
			}
		}

		if (justJoined)
		{
			justJoined = false;

			chat->clearMessages();

			CPacketWtfAmIn fuck;
			fuck.Order = 0;
			fuck.PacketType = eCPacketWtfAmIn;

			Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);
			if (selectedContainerIndex != 1)
				selectContainer(1);
		}


	if (isInLobby && downloading && lobbyStuffCreated)
	{
		float prog = Steam::CheckWorkshopDownload();
		if (prog != 0)
		{
			AvgContainer* cont = (AvgContainer*)multiContainer->findItemByName("songContainer");

			if (!downloadingPack)
				((Text*)cont->findItemByName("title"))->setText("Downloading song...");
			else
				((Text*)cont->findItemByName("title"))->setText("Downloading pack...");
			((Text*)cont->findItemByName("diff"))->setText(std::to_string(prog * 100).substr(0,3) + "%");
			((Text*)cont->findItemByName("diff"))->centerX();
		}
	}
	MUTATE_END
}


void updateDiff()
{
	MUTATE_START
	AvgContainer* cont = (AvgContainer*)MainerMenu::soloContainer->findItemByName("songContainer");
	if (!cont) // lol
		return;

	Text* diff = (Text*)cont->findItemByName("diff");

	std::string diffDisplay = MainerMenu::currentSelectedSong.meta.difficulties[MainerMenu::selectedDiffIndex].name;
	if (diffDisplay.size() > 14)
		diffDisplay = diffDisplay.substr(0, 14) + "...";

	diff->setText(diffDisplay);

	diff->x = (cont->w / 2) - (diff->w / 2);
	diff->y = 145;

	AvgSprite* left = (AvgSprite*)cont->findItemByName("leftSelect");
	AvgSprite* right = (AvgSprite*)cont->findItemByName("rightSelect");

	left->x = diff->x - 24;
	right->x = (diff->x + diff->w) + 24;

	MUTATE_END
}


void MainerMenu::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START
	switch (event.keysym.sym)
	{
	case SDLK_TAB:
		if (MainerMenu::selectedSong.isSteam && selectedContainerIndex == 0 && !fetchingScores)
		{
			lockInput = !lockInput;

			scrollLeaderboard = 0;

			for (LeaderboardResult r : leaderboardResults)
			{
				delete r.name;
				delete r.accuracy;
			}

			leaderboardResults.clear();

			if (lockInput)
			{
				CPacketLeaderboardRequest req;

				fetchingScores = true;

				req.chartId = (MainerMenu::selected.isSteam ? MainerMenu::selected.steamId : MainerMenu::selectedSong.steamId);
				req.chartIndex = (MainerMenu::selected.isSteam ? MainerMenu::packSongIndex : -1);
				req.Order = 0;
				req.PacketType = eCPacketLeaderboardRequest;

				Multiplayer::sendMessage<CPacketLeaderboardRequest>(req);
			}
		}
		if (isInLobby && selectedContainerIndex == 1)
		{
			if (chat->opened)
				chat->close();
			else
				chat->open();
		}
		break;
	}
	if (lockInput || lobbyUp)
		return;
	if (selectedContainerIndex == 0)
	{
		switch (event.keysym.sym)
		{
		case SDLK_DOWN:

			break;
		case SDLK_UP:

			break;
		case SDLK_F5:
			if (!SongGather::steamRegAsyncAlready)
			{
				Game::instance->steam->populateSubscribedItems();
				actuallyLoad = true;
				resetStuff();
				for (Pack& p : packs)
				{
					if (p.hasBanner)
						stbi_h::stbi_free(p.banner.data);
					for (Song& s : p.songs)
						if (s.hasBanner)
							stbi_h::stbi_free(s.banner.data);
				}
				packs.clear();
				loadPacks();
			}
			break;
		case SDLK_3:
			Game::instance->steam->populateWorkshopItems(1);
			selectContainer(3);
			break;
		case SDLK_LSHIFT:
			if (selected.metaPath.size() != 0 && selected.metaPath != "unfl")
			{


				if ((!chartUploading && (event.keysym.mod & KMOD_ALT)) || chartUploading)
				{
					uploading = true;
					Game::steam->createWorkshopItem();
				}
			}
			break;
		case SDLK_LEFT:
			if (currentSelectedSong.meta.difficulties.size() > 1)
			{
				MainerMenu::selectedDiffIndex--;
				if (MainerMenu::selectedDiffIndex < 0)
					MainerMenu::selectedDiffIndex = currentSelectedSong.meta.difficulties.size() - 1;
				updateDiff();
			}
			break;
		case SDLK_RIGHT:
			if (currentSelectedSong.meta.difficulties.size() > 1)
			{
				MainerMenu::selectedDiffIndex++;
				if (MainerMenu::selectedDiffIndex > currentSelectedSong.meta.difficulties.size() - 1)
					MainerMenu::selectedDiffIndex = 0;
				updateDiff();
			}
			break;
		case SDLK_RETURN:
				if (currentSelectedSong.meta.difficulties.size() != 0 && !chat->opened && SoundManager::isThreadDone)
				{
					if (!isInLobby)
					{
						resetStuff();
						Game::instance->transitionToMenu(new Gameplay());
						delete lobbyShader;
					}
					else if (selectedSong.isSteam || selected.isSteam)
					{
						selectContainer(1);
						CPacketHostChangeChart chart;
						chart.chartID = selectedSong.steamId;
						chart.packID = selected.steamId;
						chart.isPack = chart.packID != 0;
						chart.chartIndex = packSongIndex;
						chart.diff = selectedDiffIndex;
						chart.Order = 0;
						chart.PacketType = eCPacketHostChangeChart;
						Multiplayer::sendMessage<CPacketHostChangeChart>(chart);

						CPacketClientChartAcquired acquired;
						acquired.PacketType = eCPacketClientChartAcquired;
						acquired.Order = 1;

						Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
					}
				}
			break;
		}
	}
	switch (event.keysym.sym)
	{
	case SDLK_RETURN:
		if (selectedContainerIndex == 1 && !chat->opened)
		{
			if (isHost)
			{
				CPacketHostStartGame start;
				start.Order = 0;
				start.PacketType = eCPacketHostStartGame;

				Multiplayer::sendMessage<CPacketHostStartGame>(start);
			}
		}
		break;
	case SDLK_ESCAPE:
		if (!isInLobby && !lobbyUp && !SongGather::steamRegAsyncAlready)
		{
			lobbyUp = true;
			Tweening::TweenManager::createNewTween("movingContainer2", settingsContainer, Tweening::tt_Y, 1000, 160, Game::gameHeight + 200, (Tweening::tweenCallback)endTrans, Easing::EaseOutCubic);
			Tweening::TweenManager::createNewTween("movingContainer1", multiContainer, Tweening::tt_Y, 900, 160, Game::gameHeight + 200, NULL, Easing::EaseOutCubic);
			Tweening::TweenManager::createNewTween("movingContainer", soloContainer, Tweening::tt_Y, 900, 160, Game::gameHeight + 200, NULL, Easing::EaseOutCubic);
		}
		else if (isInLobby)
		{
			if (selectedContainerIndex != 1)
			{
				selectContainer(1);
				Game::showErrorWindow("Notice", "Press escape again to leave.", false);
			}
			else
			{
				CPacketLeave leave;
				leave.Order = 0;
				leave.PacketType = eCPacketLeave;

				Multiplayer::sendMessage<CPacketLeave>(leave);
				cleanLobby();

			}
		}
		break;
	}

	MUTATE_END
}


void MainerMenu::addPack(std::string name, texData bg, bool showText, bool isSteam)
{
	VM_START
	Texture* background = Texture::loadTextureFromData(bg.data,bg.w,bg.h);
	AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
	PackObject* obj = NULL;
	obj = new PackObject(0, packIndex * 75, name, background, showText);
	obj->w = packContainer->w;
	obj->h = 75;
	packContainer->addObject(obj, "packInd" + packIndex);
	packIndex++;
	VM_END
}

void MainerMenu::dropFile(SDL_DropEvent ev)
{
	if (!SongGather::steamRegAsyncAlready)
	{
		Game::showErrorWindow("Please wait..", "Please wait for all charts to load.", false);
		return;
	}
	if (!SongUtils::IsDirectory(SongUtils::s2ws(std::string(ev.file))))
	{
		Chart c = SongGather::extractAndGetChart(std::string(ev.file));
		if (c.meta.songName.size() != 0)
		{
			actuallyLoad = true;
			resetStuff();
			loadPacks();
			Game::showErrorWindow("Chart imported", "Check in Workshop/Local", false, { 70, 116, 232 });
		}
		else
		{
			std::cout << "Failed to import " << ev.file << std::endl;
			Game::showErrorWindow("Failed to import chart", "Check log.txt", true);
		}
	}
	else
	{
		if (SongGather::steamRegAsyncAlready)
		{
			Game::showErrorWindow("Please wait!", "Please wait until all packs are loaded!", true);
			return;
		}
		bool shouldMove = false;
		for (const auto& entry : std::filesystem::directory_iterator(std::string(ev.file)))
		{
			if (entry.path().string().find("pack.meta") != std::string::npos) {
				shouldMove = true;
				break;
			}
		}

		std::vector<std::string> stuff = Chart::split(ev.file, '\\');
		std::string newDir = stuff[stuff.size() - 1];
		std::string toPath = (Average4k::path + "/assets/charts/" + newDir);
		if (!CreateDirectoryA(toPath.c_str(), NULL))
		{
			std::cout << "Failed to import " << ev.file << ". Couldn't create the directory." << std::endl;
			Game::showErrorWindow("Failed to import pack", "Check log.txt", true);
			return;
		}

		std::string banner = "";

		for (const auto& entry : std::filesystem::directory_iterator(std::string(ev.file)))
		{
			if ((SongUtils::ends_with(entry.path().string(), ".png") || SongUtils::ends_with(entry.path().string(), ".jpg")) && banner.size() == 0)
			{
				std::vector<std::string> stufff = Chart::split(entry.path().string(), '\\');
				banner = stufff[stufff.size() - 1];
			}
			if (std::filesystem::is_directory(entry))
			{
				std::vector<std::string> stufff = Chart::split(entry.path().string(), '\\');
				std::string newDirr = stufff[stufff.size() - 1];
				std::string tooPath = (Average4k::path + "/assets/charts/" + newDir + "/" + newDirr);

				if (!CreateDirectoryA(tooPath.c_str(), NULL))
				{
					std::cout << "Failed to import " << ev.file << ". Couldn't create the directory inside the root directory." << std::endl;
					Game::showErrorWindow("Failed to import pack", "Check log.txt", true);
					return;
				}
				for (const auto& e : std::filesystem::directory_iterator(entry.path()))
				{
					std::vector<std::string> files = Chart::split(e.path().string(), '\\');
					std::string dirFile = files[files.size() - 1];
					if (!std::filesystem::is_directory(e))
					{
						rename(e.path(), std::string(tooPath) + "/" + dirFile);
					}
				}
			}
			else
			{
				std::vector<std::string> files = Chart::split(entry.path().string(), '\\');
				std::string dirFile = files[files.size() - 1];
				if (!std::filesystem::is_directory(entry))
				{
					rename(entry.path(), std::string(toPath) + "/" + dirFile);
				}
			}
		}
		std::filesystem::remove_all(ev.file);

		if (!shouldMove)
		{
			std::ofstream packMeta;
			packMeta.open(toPath + "/pack.meta");

			packMeta << "# Pack imported using the in game importer\nbanner: " + (banner.size() == 0 ? "No-Banner" : banner) + "\npackName: " + newDir + "\nshowName: " + (banner.size() == 0 ? "true" : "false");
		}

		actuallyLoad = true;
		resetStuff();
		loadPacks();
		Game::showErrorWindow("Pack imported", "Check your pack list", false, { 70, 116, 232 });
	}
}

void MainerMenu::onSteam(std::string s)
{
	VM_START
	if (s == "createdItem")
	{
		// ok then lets fuckin do it lol!

		if (!chartUploading)
			Game::steam->uploadPack(&selected, Game::steam->createdId);
		else
		{
			std::string replaced2 = Steam::ReplaceString(selectedSong.path, "\\", "/");

			std::vector<std::string> split = Chart::split(replaced2, '/');

			Game::steam->uploadToItem(&currentSelectedSong, Game::steam->createdId, split[split.size() - 1]);
		}
	}

	if (s == "failedItem")
	{
		uploading = false;
		chartUploading = false;
		((Text*)soloContainer->findItemByName("uploadingProgress"))->text = "Failed to upload!";
	}

	if (s == "uploadItem")
	{
		uploading = false;
		chartUploading = false;
		((Text*)soloContainer->findItemByName("uploadingProgress"))->text = "Uploaded!";
		actuallyLoad = true;
		resetStuff();
		packs.clear();
		loadPacks();
	}

	if (s == "chartAquired" && isInLobby && lobbyStuffCreated)
	{
		if (downloadingPack)
		{
			selected = Game::steam->downloadedPack;
			currentSelectedSong = selected.songs[MainerMenu::packSongIndex].c.meta;
		}
		else
		{
			currentSelectedSong = Game::steam->downloadedChart;
		}

		if (currentSelectedSong.meta.difficulties.size() == 0)
		{
			Game::showErrorWindow("Steam Error!", "Steam failed to download the song.", true);
			currentSelectedSong.destroy();
			return;
		}

		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 0;
		Color c;
		c.r = 128;
		c.g = 128;
		c.b = 255;
		downloading = false;
		downloadingPack = false;
		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
		std::string path = currentSelectedSong.meta.folder + "/" + currentSelectedSong.meta.audio;
		if (SoundManager::getChannelByName("prevSong") != NULL)
		{
			Channel* ch = SoundManager::getChannelByName("prevSong");
			ch->stop();
			ch->free();
			SoundManager::removeChannel("prevSong");
		}
		Channel* ch = SoundManager::createChannel(path.c_str(), "prevSong");
		ch->play();
		ch->loop = true;
		ch->setVolume(Game::save->GetDouble("Music Volume"));

		AvgContainer* cont = (AvgContainer*)multiContainer->findItemByName("songContainer");

		Object* bg = cont->findItemByName("background");

		if (cont->doesObjectExist("background"))
			cont->removeObject(bg, true);

		AvgSprite* background = new AvgSprite(0, 0, Texture::createWithImage(currentSelectedSong.meta.folder + "/" + currentSelectedSong.meta.background));
		if (background->w < cont->w)
			background->w = cont->w;
		if (background->h < cont->h)
			background->h = cont->h;
		background->x = (cont->w / 2) - background->w / 2;
		background->y = (cont->h / 2) - background->h / 2;
		background->alpha = 0;

		Tweening::TweenManager::createNewTween("fuckyoutween", background, Tweening::tt_Alpha, 240, 0, 1, NULL, Easing::EaseInSine, false);

		cont->addObject(background, "background", true);


		std::string display = currentSelectedSong.meta.songName;
		std::string secondLine = "";

		if (display.size() > 25)
		{
			secondLine = display.substr(25, display.size());
			display = display.substr(0, 25);
			if (secondLine.size() > 22)
				secondLine = secondLine.substr(0, 22) + "...";
		}
		Text* title = (Text*)cont->findItemByName("title");
		title->setText(display);
		title->setCharacterSpacing(3);
		Text* artist = (Text*)cont->findItemByName("artist");
		artist->setText(currentSelectedSong.meta.artist);
		artist->setCharacterSpacing(2.33);

		if (secondLine.size() != 0)
		{
			Text* title2 = (Text*)cont->findItemByName("title2");
			title2->setText(secondLine);
			title2->setCharacterSpacing(3);
			artist->y += 20;
		}


		std::string diffDisplay = currentSelectedSong.meta.difficulties[selectedDiffIndex].name;
		if (diffDisplay.size() > 14)
			diffDisplay = diffDisplay.substr(0, 14) + "...";

		Text* diff = (Text*)cont->findItemByName("diff");
		diff->setText(diffDisplay);
		diff->setCharacterSpacing(4.17);

		diff->x = (cont->w / 2) - (diff->w / 2);
		diff->y = 145;

		std::string type = "StepMania";

		switch (selectedSong.c.meta.chartType)
		{
		case 1:
			type = "Quaver";
			break;
		case 2:
			type = "Osu!";
			break;
		}

		Text* chartType = (Text*)cont->findItemByName("chartType");
		chartType->setText(type);
		chartType->setCharacterSpacing(4.17);

		chartType->x = 12;
		chartType->y = cont->h - 24;

		Text* localWorkshop = (Text*)cont->findItemByName("localWorkshop");
		localWorkshop->setText("Steam Workshop");
		localWorkshop->setCharacterSpacing(4.17);

		localWorkshop->x = ((cont->w) - localWorkshop->w) - 12;
		localWorkshop->y = cont->h - 24;
	}

	VM_END
}

void MainerMenu::loadPacks()
{
	if (!actuallyLoad)
		return;
	actuallyLoad = false;
	// create packs

	SongGather::gatherPacksAsync();

	stop = false;

	bool addWorkshop = true;
	for (Pack p : packs)
		if (p.packName == "Workshop Songs")
			addWorkshop = false;

	if (Game::steam->subscribedList.size() > 0 && steamWorkshop.songs.size() == 0)
	{
		steamWorkshop.background = "";
		steamWorkshop.metaPath = "unfl";
		steamWorkshop.packName = "Workshop Songs";
		steamWorkshop.showName = true;
		steamWorkshop.isSteam = false;
		steamWorkshop.songs = {};

		if (addWorkshop)
		{
			packs.push_back(steamWorkshop);

			addPack(steamWorkshop.packName, steamWorkshop.banner, steamWorkshop.showName, true);
		}
	}
	else
	{
		if (steamWorkshop.songs.size() > 0)
		{
			if (addWorkshop)
				packs.push_back(steamWorkshop);

			addPack(steamWorkshop.packName, steamWorkshop.banner, steamWorkshop.showName, true);
		}
	}

	for (Pack p : packs)
	{
		if (p.packName != "Workshop Songs")
			addPack(p.packName, p.banner, p.showName, p.isSteam);
	}
}

void MainerMenu::mouseWheel(float wheel)
{
	if (lockInput)
	{
		if (wheel < 0)
			scrollLeaderboard += 10;
		else if (wheel > 0)
			scrollLeaderboard -= 10;
	}
}

void MainerMenu::onPacket(PacketType pt, char* data, int32_t length)
{
	SPacketLeaderboardResponse res;
	SPacketServerListReply fuck;
	SPacketStatus f;
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketUpdateLobbyChart cc;
	SPacketFuckYou fuckyou;
	msgpack::unpacked result;

	msgpack::object obj;
	switch (pt)
	{
	case eSPacketStatus:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(f);
		switch (f.code)
		{
		case 803:
			// we host pog
			std::cout << "host me" << std::endl;
			isHost = true;
			currentLobby.Host.SteamID64 = std::to_string(SteamUser()->GetSteamID().ConvertToUint64());
			if (lobbyStuffCreated)
				lobbyUpdatePlayers();
			break;
		case 1337:
			Color c;
			c.r = 0;
			c.g = 255;
			c.b = 0;

			if (!isHost)
				return;
			break;
		case 8876:
			Color cc;
			cc.r = 255;
			cc.g = 0;
			cc.b = 0;
			Game::showErrorWindow("Starting... failed!", "Wait for everyone to download the chart!)", false);
			break;
		}

		if (f.code >= 9000)
		{
			std::cout << "got status for " << f.code << std::endl;
			if (isHost && f.code - 9000 < currentLobby.PlayerList.size())
			{
				SPacketOnChat onChat;
				onChat.tagColor = { 0, 228, 255 };
				onChat.color = { 225, 247, 255 };
				onChat.tagText = "[LOBBY]";
				onChat.message = currentLobby.PlayerList[f.code - 9000].Name + " has the chart!";
				if (isHost)
					chat->addMessage(onChat);
			}
		}

		break;
	case eSPacketServerListReply:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(fuck);

		Lobbies = fuck.Lobbies;

		createNewLobbies();
		break;
	case eSPacketStartLobbyGame:
		Game::instance->transitionToMenu(new Gameplay());
	case eSPacketWtfAmInReply:
		if (currentLobby.LobbyID == 0 && isInLobby)
		{
			peopleWhoHaveChart = 0;
			msgpack::unpack(result, data, length);

			obj = msgpack::object(result.get());

			obj.convert(reply);

			isHost = reply.isHost;

			currentLobby = reply.Lobby;
			peopleWhoNeedChart = currentLobby.Players;
			createLobby();
		}
		break;
	case eSPacketFuckYou:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(fuckyou);

		if (fuckyou.demotion)
		{
			Game::asyncShowErrorWindow("Host switch", "You are no longer the host.", false);
			isHost = false;
			currentLobby = fuckyou.lobby;
			lobbyUpdatePlayers();
		}
		else if (fuckyou.lobbyKick)
		{
			Game::asyncShowErrorWindow("Kicked!", "You have been kicked from the lobby!", true);
			isInLobby = false;
			cleanLobby();
		}
		break;
	case eSPacketUpdateLobbyData:
		if (isInLobby)
		{
			peopleWhoHaveChart = 0;
			msgpack::unpack(result, data, length);

			obj = msgpack::object(result.get());

			obj.convert(update);

			currentLobby = update.Lobby;

			peopleWhoNeedChart = currentLobby.Players;

			lobbyUpdatePlayers();
		}
		break;
	case eSPacketUpdateLobbyChart:
		if (isInLobby)
		{
			peopleWhoHaveChart = 0;
			msgpack::unpack(result, data, length);

			obj = msgpack::object(result.get());

			obj.convert(cc);
			packSongIndex = cc.chartIndex;
			selectedDiffIndex = cc.diff;
			Color c;
			c.r = 128;
			c.g = 128;
			c.b = 255;
			if (cc.isPack)
			{
				Game::steam->LoadWorkshopChart((uint64_t)cc.packID);
				downloadingPack = true;
			}
			else
				Game::steam->LoadWorkshopChart((uint64_t)cc.chartID);
			downloading = true;
		}
		break;
	case eSPacketLeaderboardResponse:
		msgpack::unpacked result;

		msgpack::object obj;
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(res);

		for (LeaderboardEntry entry : res.leaderboard.entries)
		{
			LeaderboardResult resu;
			resu.entry = entry;
			resu.accuracy = new Text(0, 0, std::to_string(entry.accuracy * 100) + "%", 16, "arial");
			resu.name = new Text(0, 0, entry.username, 16, "arialbd");

			leaderboardResults.push_back(resu);
		}

		if (res.leaderboard.entries.size() == 0)
		{
			LeaderboardResult resu;
			resu.entry = LeaderboardEntry();
			resu.accuracy = new Text(0, 0, "", 16, "arial");
			resu.name = new Text(0, 0, "No scores have been submited on this chart.", 16, "arialbd");
			leaderboardResults.push_back(resu);
		}
		fetchingScores = false;
		break;
	}
}

void MainerMenu::postUpdate(Events::updateEvent ev)
{

	if (lockInput)
	{
		if (scrollLeaderboard < 0)
			scrollLeaderboard = 0;

		if (scrollLeaderboard > leaderboardResults.size() * 80)
			scrollLeaderboard = leaderboardResults.size() * 80;
		Rect big;
		big.x = 0;
		big.y = 0;
		big.w = 1280;
		big.h = 720;
		big.r = 0;
		big.g = 0;
		big.b = 0;
		big.a = 0.5;


		Rect src;
		src.x = 0;
		src.y = 0;
		src.w = 1;
		src.h = 1;

		Rendering::PushQuad(&big, &src, NULL, GL::genShader);

		Rendering::drawBatch();

		int ind = 0;
		for (LeaderboardResult res : leaderboardResults)
		{
			Rect bg = { 0,(80 * ind) - scrollLeaderboard,1280,80,0,0,0,0.5 };

			Rendering::PushQuad(&bg, &src, NULL, GL::genShader);
			Rendering::drawBatch();
			res.name->x = 14;
			res.accuracy->x = 28 + res.name->w;

			res.name->y = (80 * ind) + 36 - scrollLeaderboard;
			res.accuracy->y = res.name->y;

			res.name->draw();
			res.accuracy->draw();
			Rendering::drawBatch();

			ind++;
		}
	}
}

void MainerMenu::clearPacks()
{
	MUTATE_START
	stop = true;
	packIndex = 0;
	AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
	for (Object* obj : packContainer->above)
	{
		packContainer->removeObject(obj);
		delete obj;
	}
	packContainer->items.clear();
	MUTATE_END
}

void MainerMenu::selectPack(int index)
{
	MUTATE_START
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
	MUTATE_END	
}

void transContainerThing()
{
	selectedContainerIndex = transToContainer;
	MainerMenu* instance = (MainerMenu*)Game::currentMenu;
	instance->isTransDone = true;
	switch (despawn)
	{
	case 0:
		MainerMenu::soloContainer->active = false;
		break;
	case 1:
		MainerMenu::multiContainer->active = false;
		break;
	case 2:
		MainerMenu::settingsContainer->active = false;
		break;
	case 3:
		MainerMenu::testWorkshop->active = false;
		break;
	}
}

void lobbySelectedCallback(int mx, int my)
{
	MainerMenu* menu = (MainerMenu*)Game::instance->currentMenu;
	if (menu->isInLobby)
		return;
	int selectedLobby = -1;
	for (int i = 0; i < menu->LobbyContainers.size(); i++)
	{
		AvgContainer* cont = menu->LobbyContainers[i];
		float scrll = ((AvgContainer*)cont->parent)->scrollAddition;

		int relX = mx - cont->parent->x;
		int relY = my - cont->parent->y + scrll;

		if (((AvgContainer*)cont->parent)->parent != NULL)
		{
			AvgContainer* parentParent = (AvgContainer*)((AvgContainer*)cont->parent)->parent;
			relX -= parentParent->x;
			relY -= parentParent->y;
		}

		if ((relX > cont->x && relY > cont->y) && (relX < cont->x + cont->w && relY < cont->y + cont->h))
		{
			selectedLobby = i;
			break;
		}
	}

	if (selectedLobby == -1)
		return;

	CPacketJoinServer list;
	list.Order = 0;
	list.PacketType = eCPacketJoinServer;
	list.LobbyID = menu->Lobbies[selectedLobby].LobbyID;

	std::cout << "trying to join " << list.LobbyID << std::endl;
	menu->isInLobby = true;
	menu->chat->showNotifs = true;
	Multiplayer::sendMessage<CPacketJoinServer>(list);
}


void MainerMenu::createNewLobbies(std::string searchTerm)
{
	int ind = 0;

	for (Object* obj : lobbyContainer->above)
	{
		delete obj;
	}
	lobbyContainer->above.clear();
	lobbyContainer->items.clear();
	LobbyContainers.clear();
	for (lobby& l : Lobbies)
	{
		if (searchTerm.size() != 0)
			if (l.LobbyName.find(searchTerm) == std::string::npos)
				continue;
		AvgContainer* cont = new AvgContainer(0, 92 * ind, NULL);
		cont->callback = lobbySelectedCallback;
		cont->w = lobbyContainer->w;
		cont->h = 92;
		lobbyContainer->addObject(cont, "lobby" + std::to_string(ind));
		Text* ln = (Text*)cont->addObject(new Text(107, 20, l.LobbyName, 16, "ANDALEMO"), "lobbyName" + std::to_string(l.LobbyID));
		ln->setCharacterSpacing(2.67);
		int y = l.Players == 0 ? ln->y + ln->h + 2 : ln->y + ln->h + 34;
		Text* lm = (Text*)cont->addObject(new Text(107, y, std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers), 14, "arial"), "lobbyPlayers" + std::to_string(l.LobbyID));
		lm->setCharacterSpacing(2);

		AvgSprite* spr = (AvgSprite*)cont->addObject(new AvgSprite(38, 20, Steam::getAvatar(l.Host.Avatar.c_str())), "hostIcon" + std::to_string(l.LobbyID));
		spr->w = 47;
		spr->h = 47;
		spr->customShader = lobbyShader;
		spr->deleteShader = false;

		AvgSprite* divider = (AvgSprite*)cont->addObject(new AvgSprite(0, 90, Noteskin::getMenuElement(Game::noteskin, "TheWhitePixel.png")), "divider" + std::to_string(l.LobbyID));
		divider->w = lobbyContainer->w;
		divider->h = 2;

		int indd = 0;
		for (player p : l.PlayerList)
		{
			if (p.SteamID64 == l.Host.SteamID64)
				continue;
			if (indd > 6)
				break;
			Texture* avatar = Steam::getAvatar(p.Avatar.c_str());

			AvgSprite* av = new AvgSprite(ln->x + (20 * indd), ln->y + ln->h, avatar);
			av->w = 32;
			av->h = 32;
			av->customShader = lobbyShader;
			av->deleteShader = false;
			cont->addObject(av, "playerIcon_" + p.SteamID64);
			indd++;
		}
		ind++;

		LobbyContainers.push_back(cont);
	}

	if (selectedContainerIndex == 1)
	{
		for (AvgContainer* con : LobbyContainers)
		{
			con->shouldUseCallback = true;
		}
	}
	
}

void MainerMenu::createLobby()
{
	multiplayerObjects.push_back(multiContainer->addObject(new Text(28,28, currentLobby.LobbyName, 18, "arial"), "lobbyName"));
	multiplayerObjects.push_back(multiContainer->addObject(new Text(28,50, std::to_string(currentLobby.Players) + "/" + std::to_string(currentLobby.MaxPlayers), 14, "ariali"), "lobbyPlayers"));

	AvgContainer* filters = (AvgContainer*)multiContainer->findItemByName("filterContainer");
	Tweening::TweenManager::createNewTween("filtersMoving", filters, Tweening::tt_Alpha, 250, 1, 0, NULL, Easing::EaseInSine);
	Tweening::TweenManager::createNewTween("lobbiesMoving", lobbyContainer, Tweening::tt_Alpha, 250, 1, 0, NULL, Easing::EaseInSine);

	for (AvgContainer* cont : LobbyContainers)
		cont->shouldUseCallback = false;

	soloText->setText("songs");
	soloText->x = selectSolo->x + (selectSolo->w / 2) - (soloText->w / 2);

	AvgContainer* iconContainers = (AvgContainer*)multiContainer->addObject(new AvgContainer(28, 128, NULL), "lobbyIcons");

	multiplayerObjects.push_back(iconContainers);

	iconContainers->w = multiContainer->w - 250;
	iconContainers->h = multiContainer->h - 142;

	lobbyUpdatePlayers();

	AvgContainer* cont = (AvgContainer*)multiContainer->addObject(new AvgContainer(multiContainer->w, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Solo/songcontainer.png")), "songContainer");
	cont->x -= cont->w;
	multiplayerObjects.push_back(cont);

	// text stuff

	Text* title = new Text(12, 24, "No Song Selected", 18, "arialbd");
	title->setCharacterSpacing(3);
	Text* artist = new Text(12, 44, "", 14, "arial");
	artist->setCharacterSpacing(2.33);

	cont->addObject(title, "title");
	Text* title2 = new Text(12, 42, "", 18, "arialbd");
	title2->setCharacterSpacing(3);
	cont->addObject(title2, "title2");
	artist->y += 20;
	cont->addObject(artist, "artist");


	Text* diff = new Text(0, 0, "", 20, "arialblk");
	diff->setCharacterSpacing(4.17);

	diff->x = (cont->w / 2) - (diff->w / 2);
	diff->y = 145;

	cont->addObject(diff, "diff");

	Text* chartType = new Text(0, 0, "", 14, "arial");
	chartType->setCharacterSpacing(4.17);

	chartType->x = 12;
	chartType->y = cont->h - 24;
	cont->addObject(chartType, "chartType");

	Text* localWorkshop = new Text(0, 0, "", 14, "arial");
	localWorkshop->setCharacterSpacing(4.17);

	localWorkshop->x = ((cont->w) - localWorkshop->w) - 12;
	localWorkshop->y = cont->h - 24;
	cont->addObject(localWorkshop, "localWorkshop");
	cont->alpha = 1;
	lobbyStuffCreated = true;
}

void MainerMenu::lobbyUpdatePlayers()
{
	Text* ln = (Text*)multiContainer->findItemByName("lobbyName");
	Text* lp = (Text*)multiContainer->findItemByName("lobbyPlayers");

	ln->setText(currentLobby.LobbyName);
	lp->setText(std::to_string(currentLobby.Players) + "/" + std::to_string(currentLobby.MaxPlayers));

	AvgContainer* iconContainers = (AvgContainer*)multiContainer->findItemByName("lobbyIcons");

	for (Object* obj : iconContainers->above)
		delete obj;

	iconContainers->items.clear();
	iconContainers->above.clear();

	AvgSprite* host = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "crown.png"));
	host->w = 18;
	host->h = 18;
	host->colorR = 255;
	host->colorG = 209;
	host->colorB = 83;

	int ind = 0;
	for (player p : currentLobby.PlayerList)
	{
		Texture* avatar = Steam::getAvatar(p.Avatar.c_str());
		AvgSprite* spr = new AvgSprite(16, 16 + (49 * ind), avatar);
		spr->w = 47;
		spr->h = 47;
		spr->customShader = lobbyShader;
		spr->deleteShader = false;
		iconContainers->addObject(spr,"player_" + p.SteamID64);

		Text* name = new Text(16 + spr->w + 18, (16 + (49 * ind)) + 10, p.Name, 16, "arial");
		iconContainers->addObject(name, "playerName_" + p.SteamID64);

		if (p.SteamID64 == currentLobby.Host.SteamID64)
		{
			host->x = spr->x - 9;
			host->y = spr->y - 9;
		}
		ind++;
	}
	iconContainers->addObject(host, "hostCrown");
}

void MainerMenu::cleanLobby()
{
	int ind = 0;

	std::vector<itemId> stuff = multiContainer->items;

	for (itemId id : stuff)
	{
		if (id.name != "filterContainer" && id.name != "lobbyContainer")
		{
			multiContainer->removeObject(id.obj);
		}
		ind++;
	}

	stuff.clear();

	for (Object* ob : multiplayerObjects)
		delete ob;
	multiplayerObjects.clear();

	AvgContainer* filters = (AvgContainer*)multiContainer->findItemByName("filterContainer");
	Tweening::TweenManager::createNewTween("filtersMoving", filters, Tweening::tt_Alpha, 250, 0, 1, NULL, Easing::EaseInSine);
	Tweening::TweenManager::createNewTween("lobbiesMoving", lobbyContainer, Tweening::tt_Alpha, 250, 0, 1, NULL, Easing::EaseInSine);
	refreshLobbies();
	isInLobby = false;
	chat->showNotifs = false;
	justJoined = false;
	lobbyStuffCreated = false;
	soloText->setText("solo");
	currentLobby = {};
	if (chat->opened)
		chat->close();
}

void MainerMenu::selectContainer(int container)
{
	MUTATE_START

	if (!isHost && isInLobby && container == 0 && isTransDone)
		return;

	isTransDone = false;

	if (container == 1 && !isInLobby)
	{
		for (AvgContainer* con : LobbyContainers)
		{
			con->shouldUseCallback = true;
		}
	}
	else
	{
		for (AvgContainer* con : LobbyContainers)
		{
			con->shouldUseCallback = false;
		}
	}

	if (container != 0)
	{
		lockInput = false;

		scrollLeaderboard = 0;

		for (LeaderboardResult r : leaderboardResults)
		{
			delete r.name;
			delete r.accuracy;
		}

		leaderboardResults.clear();
	}

	if (container == 2)
	{
		for (Object* obj : settingsContainer->above)
		{
			obj->isActive = true;
		}
	}
	else
	{
		for (Object* obj : settingsContainer->above)
		{
			obj->isActive = false;
		}
	}

	transToContainer = container;
	despawn = lastTrans;
	if (transToContainer <= lastTrans)
		Tweening::TweenManager::createNewTween("movingContainer", currentContainer, Tweening::tt_X, 750, currentContainer->x, Game::gameWidth, (Tweening::tweenCallback)transContainerThing, Easing::EaseOutCubic);
	else
		Tweening::TweenManager::createNewTween("movingContainer", currentContainer, Tweening::tt_X, 750, currentContainer->x, -(currentContainer->w + 100), (Tweening::tweenCallback)transContainerThing, Easing::EaseOutCubic);
	switch (container)
	{
	case 0:
		if (chat->opened)
			chat->close();
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
	case 3:
		currentContainer = testWorkshop;
		testWorkshop->active = true;
		selectSolo->alpha = 0;
		selectMulti->alpha = 0;
		selectSettings->alpha = 0;
		break;
	}
	if (transToContainer <= lastTrans)
		Tweening::TweenManager::createNewTween("movingContainer2", currentContainer, Tweening::tt_X, 750, -(currentContainer->w + 100), (Game::gameWidth / 2) - (currentContainer->w / 2), NULL, Easing::EaseOutCubic);
	else
		Tweening::TweenManager::createNewTween("movingContainer2", currentContainer, Tweening::tt_X, 750, Game::gameWidth, (Game::gameWidth / 2) - (currentContainer->w / 2), NULL, Easing::EaseOutCubic);
	lastTrans = container;
	MUTATE_END
}

void MainerMenu::leftMouseDown()
{
	MUTATE_START
	if (lockInput || lobbyUp)
		return;
	int x, y;
	Game::GetMousePos(&x, &y);

	if (selectedContainerIndex == transToContainer)
	{
		if (selectedContainerIndex != 0)
			if ((x > selectSolo->x && y > soloText->y) && (x < selectSolo->x + selectSolo->w && y < selectSolo->y))
				selectContainer(0);
		if (selectedContainerIndex != 1)
			if ((x > selectMulti->x && y > multiText->y) && (x < selectMulti->x + selectMulti->w && y < selectMulti->y))
			{
				selectContainer(1);
			}
		if (selectedContainerIndex != 2)
			if ((x > selectSettings->x && y > settingsText->y) && (x < selectSettings->x + selectSettings->w && y < selectSettings->y))
				selectContainer(2);
	}

	if (selectedContainerIndex == 0 && !uploading)
	{
		AvgContainer* packContainer = (AvgContainer*)soloContainer->findItemByName("packContainer");
		int relX = (x - soloContainer->x) - packContainer->x;
		int relY = (y - soloContainer->y) - packContainer->y;
		for (int i = 0; i < packContainer->above.size(); i++)
		{
			Object* obj = packContainer->above[i];
			int yy = obj->y - packContainer->scrollAddition;
			if ((relX > obj->x && relY > yy) && (relX < obj->x + obj->w && relY < yy + obj->h))
			{
				wheel->setSongs(packs[i].songs);
				selectPack(i);
				wheel->callSelect(0);
				return;
			}
		}
	}
	MUTATE_END
}

void dropdown_callback(std::string set, std::string value)
{
	MUTATE_START
	if (set == "Fullscreen" || set == "Resolution")
	{
		std::vector<int> res = Game::save->ObtainResolution();

		int fs = 0;

		std::string fsType = Game::save->GetString("Fullscreen");
		if (fsType == "Borderless")
			fs = 2;
		else if (fsType == "Fullscreen")
			fs = 1;

		Game::instance->resizeGame(res[0], res[1], fs);
	}

	if (set == "Noteskin")
	{
		Game::noteskin = Noteskin::getNoteskin();
		resetStuff();
		Game::instance->transitionToMenu(new MainMenu());
		delete ((MainerMenu*)Game::instance->currentMenu)->lobbyShader;
	}

	Game::save->Save();
	MUTATE_END
}


void MainerMenu::addSettings(std::string catNam, std::vector<setting> settings)
{
	VM_START
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
		((Text*)settingsContainer->findItemByName(itemName))->setCharacterSpacing(2.33);

		if (set.isDropdown)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_dropDown";

			settingsContainer->addObject(new AvgDropDown(startX + ww + 26, hey, SaveFile::ObtainDropDownSettingList(set.name)), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgDropDown*)settingsContainer->findItemByName(boxName))->toModify = set;
			((AvgDropDown*)settingsContainer->findItemByName(boxName))->call = (dropDownCallback)dropdown_callback;
		}
		else if (set.takesString)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_textBox";

			settingsContainer->addObject(new AvgTextBar(startX + ww + 26, hey, set.defaultString, Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/typeinputcontainer.png")), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->toModify = set;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->spacedOut = 4;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->unique = set.unique;
		}

		if (set.takesActive)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_checkBox";

			settingsContainer->addObject(new AvgCheckBox(startX + ww + 26, hey, set.defaultActive), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgCheckBox*)settingsContainer->findItemByName(boxName))->toModify = set;
		}

		if (set.takesDouble)
		{
			int hey = startY + lastHeight + 52 + (52 * setInd);

			std::string boxName = "_cat_" + catNam + "_item_" + set.name + "_textBox";

			settingsContainer->addObject(new AvgTextBar(startX + ww + 26, hey, std::to_string(set.defaultDouble), Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/typeinputcontainer.png")), boxName);
			settingsContainer->findItemByName(boxName)->y -= settingsContainer->findItemByName(itemName)->h / 2;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->toModify = set;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->spacedOut = 4;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->suffix = set.settingSuffix;
			((AvgTextBar*)settingsContainer->findItemByName(boxName))->resyncText();
		}

		setInd++;
	}

	lastHeight += (52 * settings.size()) + 52;
	VM_END
}