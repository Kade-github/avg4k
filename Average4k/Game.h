#pragma once
#include "includes.h"
#include <map>
#include "Steam.h"
#include "SaveFile.h"
#include "Camera.h"
#include "Menu.h"

class Game
{
public:
	void createGame();

	void update(Events::updateEvent updateEvent);
	void keyDown(SDL_KeyboardEvent event);
	void keyUp(SDL_KeyboardEvent event);

	void textInput(SDL_TextInputEvent event);

	static Menu* currentMenu;
	static Camera* mainCamera;
	static SaveFile* save;
	static Steam* steam;

	bool fullscreen = false;

	static int gameWidth;
	static int gameHeight;

	static std::map<int, bool> controls;

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

