#include "includes.h"
#include <algorithm>

#include "Game.h"
#include "MainMenu.h"
#include "Text.h"

using namespace std;

vector<Object*>* objects;

Menu* Game::currentMenu = NULL;
Camera* Game::mainCamera = NULL;
SaveFile* Game::save = NULL;
Steam* Game::steam = NULL;

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


void Game::createGame()
{
	objects = new std::vector<Object*>();

	// to start
	currentMenu = new MainMenu();

	mainCamera = new Camera(1280, 720);
	mainCamera->cameraTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Game::gameWidth, Game::gameHeight);
	save = new SaveFile();

	steam = new Steam();
	steam->InitSteam();
}


void Game::update(Events::updateEvent update)
{

	SDL_RenderClear(update.renderer);

	SDL_SetRenderTarget(renderer, mainCamera->cameraTexture);

	SDL_RenderClear(update.renderer);

	static Text* fpsText = nullptr;
	if (!fpsText)
	{
		fpsText = new Text(0, 0, "FPS: 0", 60, 40);
		fpsText->create();
	}

	mainCamera->update(update);

	currentMenu->update(update);

	fpsText->setText("FPS: " + std::to_string(gameFPS) + " | Delta Time: " + std::to_string(deltaTime));

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

	SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);

	SDL_SetRenderTarget(renderer, NULL);

	SDL_RenderCopyEx(renderer, mainCamera->cameraTexture, NULL, &DestR, mainCamera->angle, NULL, SDL_FLIP_NONE);

	SDL_RenderPresent(renderer);
}

void Game::keyDown(SDL_KeyboardEvent ev)
{
	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = true;

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

	for (int i = 0; i < objects->size(); i++)
	{
		Object* bruh = (*objects)[i];
		bruh->keyDown(ev);
	}

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
	for (int i = 0; i < objects->size(); i++)
	{
		Object* btuh = (*objects)[i];
		btuh->textInput(event);
	}
}