#pragma once
#include "includes.h"
#include <map>
#include "Steam.h"
#include "SaveFile.h"
#include "Camera.h"
#include "Menu.h"
#include "Multiplayer.h"
#include "Chart.h"
class Game
{
public:
	void createGame();

	Menu* menuToDelete;

	bool transitioning;

	static bool startConnect;
	static Menu* toGoTo;

	void update(Events::updateEvent updateEvent);
	void keyDown(SDL_KeyboardEvent event);
	void keyUp(SDL_KeyboardEvent event);

	void textInput(SDL_TextInputEvent event);

	static Menu* currentMenu;
	static Camera* mainCamera;
	static SaveFile* save;
	static Steam* steam;
	static Multiplayer* multi;
	static chartMeta loadedChart;
	static std::string version;

	static Game* instance;
	static bool patched;

	bool fullscreen = false;

	static int gameWidth;
	static int gameHeight;
	static bool gameplayEvents_DB;
	static std::map<int, bool> controls;
	void db_addLine(std::string s);
	void weGotPacket(Events::packetEvent p);

	void transitionToMenu(Menu* m);
	void switchMenu(Menu* m);

	static std::vector<Object*>* getGlobalObjects();
	static void addGlobalObject(Object* obj);
	static void removeGlobalObject(Object* obj);

	static bool getKey(int code);

	static SDL_Renderer* renderer;
	static SDL_Window* window;
	static float startTick;
	static double gameFPS;
	static float deltaTime;
};

