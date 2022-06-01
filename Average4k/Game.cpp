#include "includes.h"
#include <algorithm>
#include "Game.h"
#include "StartMenu.h"
#include "Text.h"
#include "CPacketHostUpdateLobby.h"
#include "MultiplayerLobby.h"
#include "AvgRect.h"
#include "msgpack.hpp"
#include "FuckinEditor.h"
#include "TweenManager.h"
#include "imgui_impl_sdl.h"
#include "Helpers.h"
using namespace std;

mutex pog;

float Game::multiplierx = 1;
float Game::multipliery = 1;

bool printPackets_DB = false;
bool Game::gameplayEvents_DB = false;

AvgRect* __transRect;

vector<Object*>* objects;

bool Game::errorWindowOpen = false;

std::vector<Events::packetEvent> packetsToBeHandeld;
std::map<std::string,Texture*> texturesToCreate;

AvgCamera* Game::mainCamera = NULL;
Viewpoint Game::mainView;
Menu* Game::currentMenu = NULL;
noteskin_asset* Game::noteskin = NULL;
Menu* Game::toGoTo = NULL;
SaveFile* Game::save = NULL;
Steam* Game::steam = NULL;
Multiplayer* Game::multi = NULL;
Game* Game::instance = NULL;
chartMeta Game::loadedChart;
bool Game::patched = false;

Texture* error_button;
Texture* error_major;
Texture* error_minor;

Rect errorButtonRect;
bool errorType;

Text* errorTitleText;
Text* errorDescriptionText;
Text* errorButtonText;

std::string Game::version;

bool debug_takingInput;

float Game::wW = 1280;
float Game::wH = 720;

std::string debug_string;

AvgRect* consoleBG;
AvgRect* consoleCMDBar;

struct needWindowPop {
	std::string title;
	std::string desc;
	bool minor;
};

bool debugConsole;
Text* debugText;
Text* consoleLog;
Text* cmdPrompt;
Text* fpsText;
Text* alphaWatermark;

map<int, bool> Game::controls = {
	{SDLK_d, false},
	{SDLK_f, false},
	{SDLK_j, false},
	{SDLK_k, false},
	{SDLK_UP, false},
	{SDLK_DOWN, false},
	{SDLK_RETURN, false}
};

SDL_Renderer* Game::renderer;
SDL_Window* Game::window;

double Game::gameFPS = 0.0;

float Game::deltaTime = 0;

float Game::startTick = 0;

int Game::gameWidth = 0;
int Game::gameHeight = 0;

bool Game::startConnect = false;

bool transCompleted = false;

HANDLE multiThreadHandle;

needWindowPop asyncPop;

void Game::asyncShowErrorWindow(std::string title, std::string description, bool major)
{
	std::lock_guard cock(pog);
	asyncPop.title = title;
	asyncPop.desc = description;
	asyncPop.minor = major;
}

void Game::showErrorWindow(std::string title, std::string description, bool major)
{
	std::string titlt = title;

	if (titlt.size() > 24)
		titlt = titlt.substr(0, 24);

	if (!errorTitleText)
		errorTitleText = new Text(0, 0, title, 18, "arial");
	else
		errorTitleText->setText(title);
	errorTitleText->setCharacterSpacing(3);

	errorType = major;

	if (!errorDescriptionText)
		errorDescriptionText = new Text(0, 0, description, 14, "arial");
	else
		errorDescriptionText->setText(description);
	errorDescriptionText->setCharacterSpacing(2.33);
	errorDescriptionText->color = { 0,0,0 };

	if (!errorButtonText)
		errorButtonText = new Text(0, 0, "ok", 14, "arialbd");
	errorButtonText->setCharacterSpacing(2.33);
	errorButtonText->color = { 0,0,0 };

	if (!error_button)
	{
		error_button = Noteskin::getMenuElement(noteskin, "roundedbutton_ok.png");
		error_major = Noteskin::getMenuElement(noteskin, "majorerroricon.png");
		error_minor = Noteskin::getMenuElement(noteskin, "minorerroricon.png");
	}

	int startingX = (1280 / 2) - (433 / 2);
	int startingY = (720 / 2) - (185 / 2);

	errorButtonRect.x = startingX + (433 - 92);
	errorButtonRect.y = startingY + (185 - 46);
	errorButtonRect.w = 80;
	errorButtonRect.h = 39;
	errorWindowOpen = true;
}

void Game::GetMousePos(int* mx, int* my)
{
	SDL_GetMouseState(mx, my);
	*mx *= multiplierx;
	*my *= multipliery;
}

std::vector<Text*> lines;

void Game::db_addLine(std::string s) {
	for (Text* t : lines)
		t->y -= 16;
	Text* newLine = new Text(0, 204, s, 16, "NotoSans-Regular");
	lines.push_back(newLine);
}

void transCall() {
	MUTATE_START
	Tweening::TweenManager::activeTweens.clear();
	Game::currentMenu->removeAll();
	Game::mainCamera->children.clear();
	delete Game::currentMenu;
	Game::currentMenu = Game::toGoTo;
	Game::currentMenu->addCamera(Game::mainCamera);
	Game::currentMenu->create();
	Game::currentMenu->created = true;
	__transRect->alpha = 0;
	transCompleted = true;
	MUTATE_END
}

void Game::transitionToMenu(Menu* m)
{
	MUTATE_START
	transCompleted = false;
	toGoTo = m;
	Tweening::tweenCallback callback = (Tweening::tweenCallback)transCall;
	transitioning = true;
	Tweening::TweenManager::activeTweens.clear();
	Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 0, 1, callback, Easing::EaseInSine);
	MUTATE_END
}

void Game::switchMenu(Menu* m)
{
	for (Object* obj : Game::mainCamera->children)
	{
		delete obj;
	}
	Tweening::TweenManager::activeTweens.clear();
	currentMenu->removeAll();
	delete currentMenu;
	currentMenu = m;
	currentMenu->addCamera(mainCamera);
	currentMenu->create();
	currentMenu->created = true;
}

void Game::createGame()
{
	VM_START
	objects = new std::vector<Object*>();
	steam = new Steam();
	steam->InitSteam();
	


	save = new SaveFile();

	std::vector<int> res = save->ObtainResolution();

	int fs = 0;

	std::string fsType = save->GetString("Fullscreen");
	if (fsType == "Borderless")
		fs = 2;
	else if (fsType == "Fullscreen")
		fs = 1;


	noteskin = Noteskin::getNoteskin();

	__transRect = new AvgRect(0,0,Game::gameWidth, Game::gameHeight);
	__transRect->create();
	__transRect->alpha = 0;

	mainCamera = new AvgCamera(0, 0, 1280, 720);

	// to start
	currentMenu = new StartMenu();
	currentMenu->addCamera(mainCamera);
	currentMenu->create();

	resizeGame(res[0], res[1], fs);

	currentMenu->created = true;
	mainView.x = 0;
	mainView.y = 0;

	consoleBG = new AvgRect(0, 0, Game::gameWidth, 220);
	consoleBG->alpha = 0.4;

	consoleCMDBar = new AvgRect(0, 220, Game::gameWidth, 25);
	consoleCMDBar->alpha = 0.4;


	fpsText = new Text(4, 4, "FPS: 240", 12, "Futura Bold");
	alphaWatermark = new Text(0, 0, "- AVERAGE4K ALPHA " + Game::version + " - EVERYTHING IS SUBJECT TO CHANGE - ", 14, "Futura Bold");
	alphaWatermark->border = true;
	fpsText->borderSize = 2;
	fpsText->border = true;
	fpsText->borderColor = { 0,0,0 };
	fpsText->borderAlpha = 0.4;
	fpsText->alpha = 0.8;
	alphaWatermark->borderSize = 2;
	alphaWatermark->alpha = 0.6;
	alphaWatermark->borderAlpha = 0.8;
	alphaWatermark->borderColor = { 255,255,255 };
	multi = new Multiplayer();
	multiThreadHandle = CreateThread(NULL, NULL, Multiplayer::connect, NULL, NULL, NULL);
	VM_END
}


void Game::mouseButtonDown()
{
	if (errorWindowOpen)
	{
		int mx, my;
		GetMousePos(&mx, &my);
		if (mx >= errorButtonRect.x && my >= errorButtonRect.y && mx < errorButtonRect.x + errorButtonRect.w && my < errorButtonRect.y + errorButtonRect.h)
		{
			errorWindowOpen = false;
		}
		return;
	}
	 
	if (objects != nullptr)
	{
		for (int i = 0; i < objects->size(); i++)
		{
			Object* bruh = objects->at(i);
			bruh->mouseDown();
		}
	}
	if (currentMenu)
	{
		currentMenu->leftMouseDown();
		for (Object* obj : mainCamera->children)
			obj->mouseDown();
	}
}

void Game::update(Events::updateEvent update)
{
	MUTATE_START
		if (Multiplayer::connectedToServer)
			SteamAPI_RunCallbacks();

	if (!Multiplayer::connectedToServer && startConnect)
	{
		startConnect = false;
		CloseHandle(multiThreadHandle);
		multiThreadHandle = CreateThread(NULL, NULL, Multiplayer::connect, NULL, NULL, NULL);
	}


	//SDL_SetRenderTarget(renderer, NULL);

	//SDL_RenderClear(update.renderer);

	//SDL_SetRenderTarget(renderer, mainCamera->cameraTexture);

	//SDL_RenderClear(update.renderer);

	if (transitioning && transCompleted)
	{
		transitioning = false;
		transCompleted = false;
		Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 255, 0, []()->void {
			Game::instance->transitioning = false;
			std::cout << "no more" << std::endl;
			}, Easing::EaseInSine);
	}

	mainCamera->update(update);


	if (currentMenu != nullptr && currentMenu->created)
	{
		currentMenu->update(update);
		currentMenu->imguiUpdate(Game::deltaTime);
	}

	if (!transitioning && SDL_GetTicks() % 250 == 0)
	{
		if (gameFPS > 240)
			gameFPS = 240;
		fpsText->setText("FPS: " + std::to_string((int)gameFPS));
	}

	for (int i = 0; i < objects->size(); i++)
	{
		try
		{
			Object* bruh = (*objects)[i];
			bruh->update(update);
		}
		catch (...)
		{

		}
	}



	SDL_Rect DestR;

	DestR.x = mainCamera->x;
	DestR.y = mainCamera->y;
	DestR.w = mainCamera->w;
	DestR.h = mainCamera->h;
	if (!transitioning)
	{
		std::vector<Events::packetEvent> bruh;
		std::vector<Texture*> bruhTex;
		{
			std::lock_guard cock(pog);
			// copy shit
			bruh = packetsToBeHandeld;
			packetsToBeHandeld.clear();
			if (asyncPop.title.size() != 0)
			{
				showErrorWindow(asyncPop.title, asyncPop.desc, asyncPop.minor);
				asyncPop.title = "";
			}
		}

		for (int i = 0; i < bruh.size(); i++)
		{
			// uh
			Events::packetEvent& p = bruh[i];
			if (printPackets_DB)
			{
				if (p.type != -42) // status packets
					db_addLine("recieved " + std::to_string(p.type));
			}
			if (currentMenu != NULL)
				currentMenu->onPacket(p.type, p.data, p.length);
			for (int i = 0; i < objects->size(); i++)
			{
				Object* btuh = (*objects)[i];
				btuh->onPacket(p.type, p.data, p.length);
			}
			free(p.ogPtr);
		}
	}

	if (currentMenu != NULL)
		if (SDL_GetTicks() > 1000)
		{
			for (Tweening::Tween tw : Tweening::TweenManager::tweenRemove)
			{
				Tweening::TweenManager::activeTweens.erase(std::remove(Tweening::TweenManager::activeTweens.begin(), Tweening::TweenManager::activeTweens.end(), tw), Tweening::TweenManager::activeTweens.end());
			}
			Tweening::TweenManager::tweenRemove.clear();

			for (Tweening::Tween& tw : Tweening::TweenManager::activeTweens)
			{
				Tweening::TweenManager::updateTween(tw, Game::deltaTime);
			}

		}
	if (currentMenu != NULL)
	{
		if (currentMenu->created)
			for (int i = 0; i < currentMenu->children.size(); i++)
			{
				Object* obj = currentMenu->children[i];
				obj->draw();

			}
	}

	if (currentMenu != nullptr && currentMenu->created)
		currentMenu->postUpdate(update);

	if (fpsText && !debugConsole)
	{
		if (alphaWatermark->x < Game::gameWidth + (alphaWatermark->surfW / 2))
		{
			alphaWatermark->x += 0.6;
		}
		else
			alphaWatermark->x = -(alphaWatermark->surfW + 12);
		alphaWatermark->draw();
		fpsText->draw();
	}


	if (debugConsole)
	{
		SDL_FRect topBar;
		topBar.x = 0;
		topBar.y = 0;
		topBar.w = Game::gameWidth;
		topBar.h = 25;
		if (!debugText)
		{
			debugText = new Text(0, 0, "Debug Console", 16, "NotoSans-Regular");
			consoleLog = new Text(0, 195, "", 16, "NotoSans-Regular");
			cmdPrompt = new Text(0, 220, ">", 16, "NotoSans-Regular");
		}

		debugText->setText("Debug Console | FPS: " + std::to_string(gameFPS) + " (CTRL to start typing, F11 again to close it)");

		if (debug_takingInput)
			cmdPrompt->setText(">" + debug_string + "_");
		else
			cmdPrompt->setText(">");



		consoleBG->draw();
		consoleCMDBar->draw();

		Rect clip;

		clip.x = 0;
		clip.y = 25;
		clip.w = Game::gameWidth;
		clip.h = 220;

		Rendering::SetClipRect(&clip);

		for (Text* t : lines)
			t->draw();

		Rendering::SetClipRect(NULL);

		debugText->draw();
		cmdPrompt->draw();
	}

	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//SDL_SetRenderTarget(renderer, NULL);

	//SDL_RenderCopyEx(renderer, mainCamera->cameraTexture, NULL, &DestR, mainCamera->angle, NULL, SDL_FLIP_NONE);


	if (transitioning)
		__transRect->draw();

	

	if (errorWindowOpen)
	{
		Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		int startingX = (1280 / 2) - (433 / 2);
		int startingY = (720 / 2) - (185 / 2);

		Rect mainBackground;
		mainBackground.x = startingX;
		mainBackground.y = startingY;
		mainBackground.w = 433;
		mainBackground.h = 185;

		Rect overlay;
		overlay.x = 0;
		overlay.y = 0;
		overlay.w = 1280;
		overlay.h = 1280;

		overlay.r = 0;
		overlay.g = 0;
		overlay.b = 0;
		overlay.a = 0.5;

		Rect topHeader;
		topHeader.x = startingX;
		topHeader.y = startingY;
		topHeader.w = 433;
		topHeader.h = 33;

		topHeader.r = 255;
		topHeader.g = 55;
		topHeader.b = 79;

		Rendering::PushQuad(&overlay, &srcRect, NULL, GL::genShader);
		Rendering::PushQuad(&mainBackground, &srcRect, NULL, GL::genShader);
		Rendering::PushQuad(&topHeader, &srcRect, NULL, GL::genShader);

		int iconW = error_major->width;


		Rect icon;
		icon.x = startingX + 6;
		icon.y = startingY + 6;
		if (errorType)
		{
			icon.w = error_major->width;
			icon.h = error_major->height;
			errorTitleText->x = (icon.x + icon.w) + 6;
			errorTitleText->y = icon.y + 2;
		}
		else
		{
			icon.x = startingX + 2;
			icon.y = startingY + 2;
			icon.w = error_minor->width;
			icon.h = error_minor->height;
			errorTitleText->x = (icon.x + icon.w) + 6;
			errorTitleText->y = icon.y + 4;
		}

		if (errorType)
			Rendering::PushQuad(&icon, &srcRect, error_major, GL::genShader);
		else
			Rendering::PushQuad(&icon, &srcRect, error_minor, GL::genShader);

		errorTitleText->draw();

		errorDescriptionText->x = (startingX + (433 / 2)) - (errorDescriptionText->w / 2);
		errorDescriptionText->y = (startingY + (185 / 2)) - (errorDescriptionText->h / 2);

		errorDescriptionText->draw();

		Rendering::PushQuad(&errorButtonRect, &srcRect, error_button, GL::genShader);
		errorButtonText->x = (errorButtonRect.x + errorButtonRect.w / 2) - (errorButtonText->w / 2);
		errorButtonText->y = (errorButtonRect.y + errorButtonRect.h / 2) - (errorButtonText->h / 2);
		errorButtonText->draw();
	}

	//SDL_RenderPresent(renderer);
	MUTATE_END
}

void Game::keyDown(SDL_KeyboardEvent ev)
{
	if (transitioning || !currentMenu || errorWindowOpen)
		return;
	if (!currentMenu->created)
		return;
	MUTATE_START
	if (ev.keysym.sym == SDLK_ESCAPE && debug_takingInput)
	{
		debug_takingInput = false;
		return;
	}
	if (ev.keysym.sym == SDLK_RETURN && debug_takingInput)
	{
		debug_takingInput = false;
		// cmds

		std::transform(debug_string.begin(), debug_string.end(), debug_string.begin(), Helpers::asciitolower);
		db_addLine(">" + debug_string);

		if (debug_string == "help")
		{
			db_addLine("checkConnection - Check's your connection and shows some other details");
			db_addLine("dumpVar - dumps a lot of variables");
			db_addLine("packets - Prints incoming packets");
			db_addLine("gameplayEvents - prints out gameplay events when they happen");
			db_addLine("xg - hacks");
			db_addLine("changeName - change the lobby name (you must be the host, and also in a lobby lol)");
			db_addLine("debug - toggle a debug mode, allows you to control the flow of fucking time. (its a lil broken, space to pause, up and down to navigate time. +/- to rate change)");
		}
		else if (debug_string == "checkconnection")
		{
			std::string res = (Multiplayer::loggedIn ? "fuck you" : "bitch ass (not logged in)");
			db_addLine(res + " - also cock: " + std::to_string(Multiplayer::connectedToServer));
		}
		else if (debug_string == "xg")
		{
			db_addLine("xg is a dumbass lol! (jkjkjkjk 187384089228214273)");
		}
		else if (debug_string == "dumpvar")
		{
			db_addLine("Menu Variables:");
			db_addLine("Transitioning: " + std::to_string(transitioning));
			db_addLine("Children: " + std::to_string(currentMenu->cam->children.size()));
		}
		else if (debug_string == "gameplayevents")
		{
			gameplayEvents_DB = !gameplayEvents_DB;
			db_addLine("set to " + std::to_string(gameplayEvents_DB));
		}
		else if (debug_string == "packets")
		{
			printPackets_DB = !printPackets_DB;
		}
		else if (debug_string.starts_with("changename"))
		{
			if (!MultiplayerLobby::inLobby)
			{
				db_addLine("u aint in a lobby dumbfuck");
				debug_string = "";
				return;
			}
			if (!MultiplayerLobby::isHost)
			{
				db_addLine("u aint a host dumbfuck");
				debug_string = "";
				return;
			}

			std::string titleName = "";

			int index = 0;
			for (std::string s : Chart::split(debug_string, ' '))
			{
				if (index != 0)
					titleName += s + " ";
				index++;
			}

			db_addLine("changing it to " + titleName);

			lobby l = MultiplayerLobby::CurrentLobby;
			l.LobbyName = titleName;

			CPacketHostUpdateLobby change;
			change.Order = 0;
			change.PacketType = eCPacketHostUpdateLobby;
			change.Lobby = l;
			Multiplayer::sendMessage<CPacketHostUpdateLobby>(change);

		}
		else if (debug_string == "debug")
		{
			flowtime = !flowtime;
			if (flowtime)
				db_addLine("FLOW TIME!!");
			else
				db_addLine("no more flow time >:((((");
		}
		else if (debug_string == "editor")
		{
			transitionToMenu(new FuckinEditor());
		}
		else
		{
			db_addLine("No command found");
		}

		debug_string = "";

		return;
	}

	if (ev.keysym.sym == SDLK_BACKSPACE && debug_takingInput)
	{
		if (debug_string.size() > 0)
			debug_string.pop_back();
	}
	if (debug_takingInput)
		return;

	if (ev.keysym.sym == SDLK_LCTRL && debugConsole)
	{
		debug_string = "";
		debug_takingInput = true;
		return;
	}

	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = true;

	if (currentMenu != NULL)
		currentMenu->keyDown(ev);

	if (ev.keysym.sym == SDLK_F11)
	{
		debugConsole = !debugConsole;
	}
	if (currentMenu)
		for (Object* obj : mainCamera->children)
			obj->keyDown(ev);

	MUTATE_END
}

void Game::keyUp(SDL_KeyboardEvent ev)
{
	if (transitioning || !currentMenu || errorWindowOpen)
		return;
	if (!currentMenu->created)
		return;
	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = false;

	currentMenu->keyUp(ev);

	for (int i = 0; i < objects->size(); i++)
	{
		Object* bruh = (*objects)[i];
		//bruh->keyUp(ev);
	}
}
void Game::mouseWheel(float wheel)
{
	if (!currentMenu || errorWindowOpen)
		return;
	if (currentMenu->created)
	{
		currentMenu->mouseWheel(wheel);
		for (Object* obj : mainCamera->children)
			obj->mouseWheel(wheel);
	}
}
void Game::mouseButtonUp()
{
	if (currentMenu || errorWindowOpen)
		currentMenu->leftMouseUp();
}

void Game::resizeGame(int w, int h, int fullscreen)
{

	wW = w;
	wH = h;
	SDL_SetWindowSize(window, w, h);
	multiplierx = (float)1280 / (float)w;
	multipliery = (float)720 / (float)h;
	switch (fullscreen)
	{
	case 0:
		SDL_SetWindowFullscreen(Game::window, 0);
		break;
	case 1:
		SDL_SetWindowFullscreen(Game::window, SDL_WINDOW_FULLSCREEN);
		break;
	case 2:
		// get resolution
		int www, hhh;
		Helpers::GetDesktopResolution(www, hhh);
		wW = www;
		wH = hhh;
		SDL_SetWindowSize(window, wW, wH);
		multiplierx = (float)1280 / (float)wW;
		multipliery = (float)720 / (float)wH;
		SDL_SetWindowFullscreen(Game::window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	}
}

//asd
void Game::weGotPacket(Events::packetEvent p)
{
	MUTATE_START
	std::lock_guard<std::mutex> s(pog);
	packetsToBeHandeld.push_back(p);
	MUTATE_END
}

void Game::createTexture(std::string filename, Texture* tex)
{
	std::lock_guard<std::mutex> cockion(pog);
	texturesToCreate.insert(std::make_pair(filename, tex));
}

Texture* Game::obtainTextureByFileName(std::string file)
{
	std::lock_guard<std::mutex> cockion(pog);
	Texture* t = texturesToCreate[file];
	texturesToCreate.erase(file);
	return t;
}

std::vector<Object*>* Game::getGlobalObjects()
{
	return objects;
}

void Game::addGlobalObject(Object* obj)
{
	objects->push_back(obj);
}

void Game::removeGlobalObject(Object* obj)
{
	objects->erase(std::remove(objects->begin(), objects->end(), obj), objects->end());
}

bool Game::getKey(int code)
{
	if (controls.count(code) != 1)
		return false;
	return controls[code];
}

void Game::textInput(SDL_TextInputEvent event)
{
	if (debug_takingInput)
	{
		debug_string += event.text;
		return;
	}

	if (currentMenu && !errorWindowOpen)
	{
		for (Object* obj : mainCamera->children)
			obj->textInput(event);
	}
}