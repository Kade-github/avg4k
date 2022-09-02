#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h"
#include "Text.h"
#include "TweenManager.h"
#include "AvgRect.h"
#include "SaveFile.h"
#include "AvgContainer.h"
#include "CPacketLeaderboardRequest.h"
#include "ChatObject.h"
#include "SPacketLeaderboardResponse.h"
// Objects
#include "AvgWheel.h"
struct LeaderboardResult {
	LeaderboardEntry entry;
	Text* name;
	Text* accuracy;
};

class PackObject : public Object
{
public:

	Text* pack;


	Texture* bg;

	bool selected = false;

	bool showText;

	PackObject(int _x, int _y, std::string packName, Texture* background, bool _showText)
	{
		pack = new Text(0, 0, packName, 18, "ANDALEMO");
		pack->color = { 0,0,0 };
		if (background)
		{
			pack->color = { 255,255,255 };
			pack->border = 1;
		}
		pack->setCharacterSpacing(3);

		if (background == NULL)
		{
			unsigned char c[] = { 0,0,0,255 };
			bg = new Texture(c, 1,1);
		}
		else
			bg = background;

		x = _x;
		y = _y;

		showText = _showText;

	}


	~PackObject()
	{
		delete bg;
		delete pack;
	}

	void draw()
	{
		Rect dstRect;
		Rect srcRect;

		dstRect.r = 0;
		dstRect.g = 0;
		dstRect.b = 0;
		

		dstRect.a = alpha;

		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = w;
		dstRect.h = h;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		Rendering::PushQuad(&dstRect, &srcRect, bg, GL::genShader);

		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;

		dstRect.x += 2;
		dstRect.y += 2;
		dstRect.w -= 2;
		dstRect.h -= 2;

		Rendering::PushQuad(&dstRect, &srcRect, bg, GL::genShader);
		if (selected)
		{
			dstRect.a = 0.1;

			Rendering::PushQuad(&dstRect, &srcRect, NULL, GL::genShader);
		}

		if (showText)
		{
			if (pack->surfW > w)
			{
				float factor = static_cast<float>(w) / static_cast<float>(pack->surfW);

				pack->surfW = pack->surfW * factor;
				pack->surfH = pack->surfH * factor;
				pack->borderW = pack->borderW * factor;
				pack->borderH = pack->borderH * factor;
				pack->w = pack->surfW;
				pack->h = pack->surfH;
			}

			pack->x = x + ((w / 2) - (pack->w / 2));
			pack->y = y + ((h / 2) - (pack->h / 2));


			pack->draw();
		}
	}
};

// class

class MainerMenu : public Menu
{
public:
	static bool isInMainerMenu;
	static Chart currentSelectedSong;
	static Song selectedSong;
	static Pack selected;
	static int packSongIndex;
	static int selectedDiffIndex;
	static bool isInLobby;
	static bool isHost;
	float lastBeat = 0;
	bool started;
	bool justJoined = false;
	static AvgWheel* wheel;

	static std::vector<Pack> packs;

	bool fetchingScores;

	bool uploading;

	ChatObject* chat;

	std::vector<lobby> Lobbies;
	std::vector<AvgContainer*> LobbyContainers;

	std::vector<Object*> multiplayerObjects;

	std::vector<AvgSprite*> icons;

	bool lobbyStuffCreated = false;

	static lobby currentLobby;

	Shader* shad;
	Shader* lobbyShader;

	bool downloading;
	bool downloadingPack;

	static std::vector<Pack> asyncPacks;
	static std::vector<Song> asyncSongs;

	AvgContainer* lobbyContainer;

	static AvgContainer* soloContainer;
	static AvgContainer* multiContainer;
	static AvgContainer* settingsContainer;
	static AvgContainer* testWorkshop;

	AvgSprite* icon;
	AvgSprite* bg;
	Text* hello;
	Text* bottom;
	AvgSprite* border;

	Text* soloText;
	AvgRect* selectSolo;
	Text* multiText;
	AvgRect* selectMulti;
	Text* settingsText;
	static bool lockInput;
	AvgRect* selectSettings;

	int peopleWhoHaveChart = 0;
	int peopleWhoNeedChart = 0;

	std::vector<LeaderboardResult> leaderboardResults;

	AvgContainer* currentContainer;

	void createNewLobbies(std::string searchTerm = "");

	void selectContainer(int container);

	void dropFile(SDL_DropEvent ev);

	void mouseWheel(float wheel) override;

	void createLobby();
	void lobbyUpdatePlayers();
	void cleanLobby();

	static bool actuallyLoad;

	bool stop = false;

	void selectPack(int index);
	void addPack(std::string name, texData bg, bool showText, bool isSteam);
	void clearPacks();

	void onPacket(PacketType pt, char* data, int32_t length);

	void postUpdate(Events::updateEvent ev) override;

	void addSettings(std::string catNam, std::vector<setting> settings);

	void create() override;

	void loadPacks();

	void update(Events::updateEvent ev) override;
	void onSteam(std::string s) override;

	void keyDown(SDL_KeyboardEvent event) override;

	void leftMouseDown() override;
};

