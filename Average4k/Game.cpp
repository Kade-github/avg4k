#include "includes.h"
#include <algorithm>

#include "Game.h"
#include "MainMenu.h"
#include "Text.h"
#include "CPacketHostUpdateLobby.h"
#include "MultiplayerLobby.h"
using namespace std;

mutex pog;

vector<Object*>* objects;

std::vector<Events::packetEvent> packetsToBeHandeld;

Menu* Game::currentMenu = NULL;
Camera* Game::mainCamera = NULL;
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

HANDLE multiThreadHandle;

void Game::switchMenu(Menu* m)
{
	currentMenu->removeAll();
	currentMenu = m;
}

void Game::createGame()
{
	objects = new std::vector<Object*>();
	steam = new Steam();
	steam->InitSteam();


	// to start
	currentMenu = new MainMenu();

	mainCamera = new Camera(1280, 720);
	mainCamera->cameraTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Game::gameWidth, Game::gameHeight);
	save = new SaveFile();

	multi = new Multiplayer();
	multiThreadHandle = CreateThread(NULL, NULL, Multiplayer::connect, NULL, NULL, NULL);
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

	SDL_SetRenderTarget(renderer, NULL);

	SDL_RenderClear(update.renderer);

	SDL_SetRenderTarget(renderer, mainCamera->cameraTexture);

	SDL_RenderClear(update.renderer);

	static Text* fpsText = nullptr;
	if (!fpsText)
	{
		fpsText = new Text(0, 0, "FPS: 0", 16);
	}

	mainCamera->update(update);

	currentMenu->update(update);

	fpsText->setText("FPS: " + std::to_string(gameFPS) + " - Avg4k 0.1a - Visuals are subject to change - " + (Multiplayer::loggedIn ? "You are logged in" : "You are logged out"));

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

	currentMenu->postUpdate(update);



	SDL_Rect DestR;

	DestR.x = mainCamera->x;
	DestR.y = mainCamera->y;
	DestR.w = mainCamera->w;
	DestR.h = mainCamera->h;

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
		currentMenu->onPacket(p.type, p.data, p.length);
		free(p.ogPtr);
	}

	if (fpsText && !debugConsole)
		fpsText->update(update);

	for (int i = 0; i < currentMenu->children.size(); i++)
	{
		Object* obj = currentMenu->children[i];
		obj->draw();
		// TODO: PUT THIS IN A DIFFERENT THREAD
	}
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
			debugText = new Text(0, 0, "Debug Console", 16);
			consoleLog = new Text(0, 195, "", 16);
			cmdPrompt = new Text(0, 220, ">", 16);
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

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetRenderTarget(renderer, NULL);

	SDL_RenderCopyEx(renderer, mainCamera->cameraTexture, NULL, &DestR, mainCamera->angle, NULL, SDL_FLIP_NONE);

	SDL_RenderPresent(renderer);
	MUTATE_END
}

void db_addLine(std::string s) {
	consoleLog->setText(consoleLog->text + s + "\n");
	consoleLog->setY(220 - consoleLog->surfH);
}

void Game::keyDown(SDL_KeyboardEvent ev)
{
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
			db_addLine("checkConnection - Check's your connection and shows some other details\nxg - hacks\nchangeName - change the lobby name");
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

		if (fullscreen)
		{
			int idx = SDL_GetWindowDisplayIndex(Game::window);
			SDL_Rect bounds;
			SDL_GetDisplayBounds(idx, &bounds);
			SDL_SetWindowBordered(Game::window, SDL_FALSE);
			SDL_SetWindowPosition(Game::window, bounds.x, bounds.y);
			SDL_SetWindowSize(Game::window, bounds.w, bounds.h);

			mainCamera->w = bounds.w;
			mainCamera->h = bounds.h;
		}
		else
		{
			int idx = SDL_GetWindowDisplayIndex(Game::window);
			SDL_Rect bounds;
			SDL_GetDisplayBounds(idx, &bounds);
			SDL_SetWindowBordered(Game::window, SDL_TRUE);
			SDL_SetWindowPosition(Game::window, bounds.w / 2, bounds.h / 2);
			SDL_SetWindowSize(Game::window, 1280, 720);
			mainCamera->w = 1280;
			mainCamera->h = 720;
		}
	}

	if (ev.keysym.sym == SDLK_F11)
	{
		debugConsole = !debugConsole;
	}


	for (int i = 0; i < objects->size(); i++)
	{
		Object* bruh = (*objects)[i];
		bruh->keyDown(ev);
	}
	MUTATE_END
}

void Game::keyUp(SDL_KeyboardEvent ev)
{
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