#pragma once
#include "includes.h"
#include <map>
#include "Steam.h"
#include "SaveFile.h"
#include "Camera.h"
#include "Menu.h"
#include "Multiplayer.h"
#include "Chart.h"
#include "AvgCamera.h"
#include "Noteskin.h"

struct Viewpoint {
	float x, y;
};

struct createtexStruct {
	Texture* texPtr;
	std::string filename;
};

class Game
{
public:
	void createGame();

	static float multiplierx;
	static float multipliery;

	static float wW, wH;
	static bool errorWindowOpen;

	bool isSteam = true;
	bool flowtime = false;

	static int frameLimit;

	Menu* menuToDelete;

	bool transitioning;

	static std::vector<Object*>* objects;

	static bool startConnect;
	static Menu* toGoTo;

	static void asyncShowErrorWindow(std::string title, std::string description, bool minor);
	static void showErrorWindow(std::string title, std::string description, bool minor);

	static void GetMousePos(int* mx, int* my);

	void mouseButtonDown();
	void update(Events::updateEvent updateEvent);
	void keyDown(SDL_KeyboardEvent event);
	void keyUp(SDL_KeyboardEvent event);
	void mouseWheel(float wheel);
	void mouseButtonUp();

	void resizeGame(int w, int h, int fullscreen);

	void textInput(SDL_TextInputEvent event);
	static AvgCamera* mainCamera;
	static Viewpoint mainView;
	static noteskin_asset* noteskin;
	static Menu* currentMenu;
	static SaveFile* save;
	static Steam* steam;
	static Multiplayer* multi;
	static chartMeta loadedChart;
	static std::string version;

	static Game* instance;
	static bool patched;

	static int gameWidth;
	static int gameHeight;
	static bool gameplayEvents_DB;
	static std::map<int, bool> controls;
	void db_addLine(std::string s);
	void weGotPacket(Events::packetEvent p);

	void createTexture(std::string filename, Texture* tex);
	Texture* obtainTextureByFileName(std::string file);

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

	static bool doesObjectExist(int id)
	{
		for (Object* obj : mainCamera->children)
		{
			if (obj->id == id)
				return true;
		}
		return false;
	}

};

