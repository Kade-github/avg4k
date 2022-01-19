#include "includes.h"
#include <algorithm>
#include "Game.h"
#include "MainMenu.h"
#include "Text.h"
#include "CPacketHostUpdateLobby.h"
#include "MultiplayerLobby.h"
#include "AvgRect.h"
#include "msgpack.hpp"
#include "TweenManager.h"
using namespace std;

mutex pog;

float Game::multiplierx = 1;
float Game::multipliery = 1;

bool printPackets_DB = false;
bool Game::gameplayEvents_DB = false;

AvgRect* __transRect;

vector<Object*>* objects;

std::vector<Events::packetEvent> packetsToBeHandeld;

AvgCamera* Game::mainCamera = NULL;
Menu* Game::currentMenu = NULL;
Menu* Game::toGoTo = NULL;
SaveFile* Game::save = NULL;
Steam* Game::steam = NULL;
Multiplayer* Game::multi = NULL;
Game* Game::instance = NULL;
chartMeta Game::loadedChart;
bool Game::patched = false;

std::string Game::version;

bool debug_takingInput;

std::string debug_string;

bool debugConsole;
Text* debugText;
Text* consoleLog;
Text* cmdPrompt;
Text* fpsText;

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

void Game::GetMousePos(int* mx, int* my)
{
	SDL_GetMouseState(mx, my);
	(*mx) *= multiplierx;
	(*my) *= multipliery;
}

void Game::db_addLine(std::string s) {
	consoleLog->setText(consoleLog->text + s + "\n");
	consoleLog->setY(220 - consoleLog->surfH);
}

void transCall() {
	for (Object* obj : Game::mainCamera->children)
	{
		if (obj != NULL)
			if (obj->w >= 0 && obj->h >= 0)
				delete obj;
	}
	Game::mainCamera->children.clear();
	Game::currentMenu->removeAll();
	delete Game::currentMenu;
	Game::currentMenu = Game::toGoTo;
	Game::currentMenu->create();
	__transRect->alpha = 0;
	transCompleted = true;
}

void Game::transitionToMenu(Menu* m)
{
	transCompleted = false;
	toGoTo = m;
	Tweening::tweenCallback callback = (Tweening::tweenCallback)transCall;
	transitioning = true;
	Tweening::TweenManager::activeTweens.clear();
	Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 0, 1, callback, Easing::EaseInSine);
}

void Game::switchMenu(Menu* m)
{
	for (Object* obj : Game::mainCamera->children)
	{
		delete obj;
	}
	Game::mainCamera->children.clear();
	currentMenu->removeAll();
	delete currentMenu;
	currentMenu = m;
}

void Game::createGame()
{
	objects = new std::vector<Object*>();
	steam = new Steam();
	steam->InitSteam();

	__transRect = new AvgRect(0,0,Game::gameWidth, Game::gameHeight);
	__transRect->create();
	__transRect->alpha = 0;

	mainCamera = new AvgCamera(0, 0, 1280, 720);

	// to start
	currentMenu = new MainMenu();
	currentMenu->create();

	save = new SaveFile();

	fpsText = new Text(0, 0, "FPS: 0", 16, "NotoSans-Regular");

	multi = new Multiplayer();
	multiThreadHandle = CreateThread(NULL, NULL, Multiplayer::connect, NULL, NULL, NULL);
}


void Game::mouseButtonDown()
{
	if (objects != nullptr)
	{
		for (int i = 0; i < objects->size(); i++)
		{
			Object* bruh = (*objects)[i];
			bruh->mouseDown();
		}
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
		transCompleted = false;
		Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 255, 0, []()->void {
			Game::instance->transitioning = false;
			std::cout << "no more" << std::endl;
		}, Easing::EaseInSine);
	}

	mainCamera->update(update);

	currentMenu->update(update);

	if (!transitioning)
		fpsText->setText("FPS: " + std::to_string(gameFPS) + " - Visuals are subject to change");

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
		{
			std::lock_guard cock(pog);
			// copy shit
			bruh = packetsToBeHandeld;
			packetsToBeHandeld.clear();
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
			free(p.ogPtr);
		}
	}

	if (SDL_GetTicks() > 1000)
	{
		for (Tweening::Tween& tw : Tweening::TweenManager::activeTweens)
		{
			Tweening::TweenManager::updateTween(tw, Game::deltaTime);
		}

		for (Tweening::Tween& tw : Tweening::TweenManager::tweenRemove)
		{
			Tweening::TweenManager::activeTweens.erase(std::remove(Tweening::TweenManager::activeTweens.begin(), Tweening::TweenManager::activeTweens.end(), tw), Tweening::TweenManager::activeTweens.end());
		}
		Tweening::TweenManager::tweenRemove.clear();
	}
	if (currentMenu != NULL)
	{
		for (int i = 0; i < currentMenu->children.size(); i++)
		{
			Object* obj = currentMenu->children[i];
			obj->draw();

		}
	}

	currentMenu->postUpdate(update);

	if (fpsText && !debugConsole)
		fpsText->draw();


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

		SDL_FRect bg;
		bg.x = 0;
		bg.y = 0;
		bg.w = Game::gameWidth;
		bg.h = 220;

		SDL_FRect bottomBar;
		bottomBar.x = 0;
		bottomBar.y = 220;
		bottomBar.w = Game::gameWidth;
		bottomBar.h = 25;
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 64);
		SDL_RenderFillRectF(renderer, &bg);
		SDL_RenderFillRectF(renderer, &topBar);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
		SDL_RenderFillRectF(renderer, &bottomBar);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_Rect clip;

		clip.x = 0;
		clip.y = 25;
		clip.w = Game::gameWidth;
		clip.h = 220;

		SDL_RenderSetClipRect(Game::renderer, &clip);

		consoleLog->draw();

		SDL_RenderSetClipRect(Game::renderer, NULL);

		debugText->draw();
		cmdPrompt->draw();
	}

	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//SDL_SetRenderTarget(renderer, NULL);

	//SDL_RenderCopyEx(renderer, mainCamera->cameraTexture, NULL, &DestR, mainCamera->angle, NULL, SDL_FLIP_NONE);


	if (transitioning)
		__transRect->draw();
	

	//SDL_RenderPresent(renderer);
	MUTATE_END
}


void Game::keyDown(SDL_KeyboardEvent ev)
{
	if (transitioning)
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


		db_addLine(">" + debug_string);

		if (debug_string == "help")
		{
			db_addLine("checkConnection - Check's your connection and shows some other details\ndumpVar - dumps a lot of variables\npackets - Prints incoming packets\ngameplayEvents - prints out gameplay events when they happen\nxg - hacks\nchangeName - change the lobby name");
		}
		else if (debug_string == "checkConnection")
		{
			std::string res = (Multiplayer::loggedIn ? "fuck you" : "bitch ass (not logged in)");
			db_addLine(res + " - also cock: " + std::to_string(Multiplayer::connectedToServer));
		}
		else if (debug_string == "xg")
		{
			db_addLine("xg is a dumbass lol! (jkjkjkjk 187384089228214273)");
		}
		else if (debug_string == "dumpVar")
		{
			db_addLine("Menu Variables:\nTransitioning: " + std::to_string(transitioning) + "\nChildren: " + std::to_string(currentMenu->children.size()));
		}
		else if (debug_string == "gameplayEvents")
		{
			gameplayEvents_DB = !gameplayEvents_DB;
			db_addLine("set to " + std::to_string(gameplayEvents_DB));
		}
		else if (debug_string == "packets")
		{
			printPackets_DB = !printPackets_DB;
		}
		else if (debug_string.starts_with("changeName"))
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

	if (ev.keysym.sym == SDLK_F4)
	{
		fullscreen = !fullscreen;

		auto w = 0;
		auto h = 0;

		if (fullscreen)
		{
			SDL_SetWindowFullscreen(Game::window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			SDL_DisplayMode DM;
			SDL_GetCurrentDisplayMode(0, &DM);
			w = DM.w;
			h = DM.h;
		}
		else
		{
			SDL_SetWindowFullscreen(Game::window, 0);
			w = 1280;
			h = 720;
		}

		std::cout << "bruh " << w << " " << h << std::endl;

		multiplierx = (float)1280 / (float)w;
		multipliery = (float)720 / (float)h;

		glViewport(0, 0, w, h);

		std::cout << "FULLSCREEN MULTIPLIERS: " << multiplierx << " " << multipliery << std::endl;

		//__transRect->w = w;
		//__transRect->h = h;
		//mainCamera->w = w;
		//mainCamera->h = h;
	}

	if (ev.keysym.sym == SDLK_F11)
	{
		debugConsole = !debugConsole;
	}

	if (objects != nullptr)
	{
		for (int i = 0; i < objects->size(); i++)
		{
			Object* bruh = (*objects)[i];
			bruh->keyDown(ev);
		}
	}
	MUTATE_END
}

void Game::keyUp(SDL_KeyboardEvent ev)
{
	if (transitioning)
		return;
	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = false;

	currentMenu->keyUp(ev);

	for (int i = 0; i < objects->size(); i++)
	{
		Object* bruh = (*objects)[i];
		bruh->keyDown(ev);
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
	for (int i = 0; i < objects->size(); i++)
	{
		Object* btuh = (*objects)[i];
		btuh->textInput(event);
	}
}