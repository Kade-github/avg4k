#include "includes.h"
#include "Steam.h"
#include "Game.h"
#include <SDL_sound.h>
#include <bass.h>
using namespace std;

#undef main

#pragma comment(lib,"x64\\SDL2.lib")
#pragma comment(lib,"x64\\bass.lib")
#pragma comment(lib,"x64\\msgpackc.lib")
#pragma comment(lib,"x64\\bass_fx.lib")
#pragma comment(lib,"x64\\SDL2_mixer.lib")
#pragma comment(lib,"x64\\SDL2_ttf.lib")
#pragma comment(lib,"x64\\SDL2_image.lib")
#pragma comment(lib,"x64\\steam_api64.lib")

#define FRAME_VALUES 60

Uint32 frametimes[FRAME_VALUES];

Uint32 frametimelast;

Uint32 framecount;

void fpsinit() {
	memset(frametimes, 0, sizeof(frametimes));
	framecount = 0;
	frametimelast = SDL_GetTicks();
}

void fpsthink() {
	Uint32 frametimesindex;
	Uint32 getticks;
	Uint32 count;
	Uint32 i;

	frametimesindex = framecount % FRAME_VALUES;

	getticks = SDL_GetTicks();

	frametimes[frametimesindex] = getticks - frametimelast;

	frametimelast = getticks;

	framecount++;

	if (framecount < FRAME_VALUES)
		count = framecount;
	else
		count = FRAME_VALUES;

	Game::gameFPS = 0;
	for (i = 0; i < count; i++)
		Game::gameFPS += frametimes[i];

	Game::gameFPS /= count;
	Game::gameFPS = std::floorf(1000.f / Game::gameFPS);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	VM_START
	STR_ENCRYPT_START
	STR_ENCRYPTW_START
	SDL_Init(SDL_INIT_EVERYTHING);
	


	#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
	#else
	freopen("log.txt", "w", stdout);
	#endif

	BASS_Init(-1,44100,0,NULL,NULL);


	int test = 123;

	CHECK_CODE_INTEGRITY(test, 6969);


	if (test != 6969) {


		//FUCK SHIT WE PATCHED

	}

	TTF_Init();

	fpsinit();

	SDL_Window* window = SDL_CreateWindow("Da window", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	Game* game = new Game();

	game->gameWidth = 800;
	game->gameHeight = 600;

	SDL_StartTextInput();

	bool run = true;

	int lastTime = 0;

	float timestamps[31], fps[31], deltaTimes[31];

	for (int i = 0; i < 30; i++)
	{
		timestamps[i] = 0;
		fps[i] = 0;
		deltaTimes[i] = 0;
	}

	int frames = 0;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;

	bool create = false;
	while (run)
	{
		const Uint32 startTime = SDL_GetTicks();
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		Game::startTick = startTime;
		SDL_Event event;
		int wheel = 0;

		while (SDL_PollEvent(&event) > 0)
		{
			switch (event.type) {
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYDOWN:
				game->keyDown(event.key);
				break;
			case SDL_KEYUP:
				game->keyUp(event.key);
				break;
			case SDL_TEXTINPUT:
				game->textInput(event.text);
				break;
			case SDL_MOUSEWHEEL:
				wheel = event.wheel.y;
				break;
			}

		}


		Events::updateEvent updateEvent;

		updateEvent.renderer = renderer;
		updateEvent.window = window;

		if (!create)
		{
			Game::renderer = renderer;
			Game::window = window;
			game->createGame();
			create = true;
		}
		game->update(updateEvent);

		Game::deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

		fpsthink();


		if (1000 / 360 > SDL_GetTicks() - startTime) {
			SDL_Delay(1000 / 360 - (SDL_GetTicks() - startTime));
		}

		frames++;

		if (frames > 30)
			frames = 0;

		timestamps[frames] = frames;
		fps[frames] = Game::gameFPS;
		deltaTimes[frames] = Game::deltaTime;


	}

	SDL_StopTextInput();

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	if (Game::steam != nullptr)
		Game::steam->ShutdownSteam();

	STR_ENCRYPT_END
	STR_ENCRYPTW_END
	VM_END
		
	return 0;
}