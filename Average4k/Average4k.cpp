#include "includes.h"
#include "GL.h"
#include "Steam.h"
#include "Game.h"
#include <DbgHelp.h>
#include "AvgSprite.h"
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
#pragma comment(lib,"x64\\libcrypto.lib")
#pragma comment(lib,"x64\\libssl.lib")
#pragma comment(lib,"x64\\zip.lib")
#pragma comment(lib,"x64\\libcurl.lib")
#pragma comment(lib,"x64\\boost_random-vc140-mt.lib")
#pragma comment(lib,"x64\\lua54.lib")
#pragma comment(lib, "dbghelp.lib")
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

void CrashDmp(_EXCEPTION_POINTERS* ExceptionInfo) {

#ifdef _DEBUG
	return;
#endif
	OFSTRUCT data;
	HANDLE file = CreateFile(L"crash.dmp", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	MINIDUMP_EXCEPTION_INFORMATION info;;
	info.ClientPointers = false;
	info.ExceptionPointers = ExceptionInfo;
	info.ThreadId = GetCurrentThreadId();

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithFullMemory, &info, NULL, NULL);
	CloseHandle(file);
}

LONG PvectoredExceptionHandler(
	_EXCEPTION_POINTERS* ExceptionInfo
)
{
	CrashDmp(ExceptionInfo);
	return 0;
}

long WINAPI UnhandledExceptionFilterHandler(LPEXCEPTION_POINTERS ex) {

	CrashDmp(ex);
	return 0;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	VM_START
		

	SetUnhandledExceptionFilter(UnhandledExceptionFilterHandler);
	//AddVectoredExceptionHandler(1, &PvectoredExceptionHandler);
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL2 video subsystem couldn't be initialized. Error: "
			<< SDL_GetError()
			<< std::endl;
		exit(1);
	}
	Game::version = "b8";

	curl_global_init(CURL_GLOBAL_ALL);
	Multiplayer::InitCrypto();

	#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
	#else
	freopen("log.txt", "w", stdout);
	#endif

	BASS_Init(-1,44100,0,NULL,NULL);

	int test = 123;

	CHECK_CODE_INTEGRITY(test, 6969);

	//Game::patched = true;

	if (test != 6969) {

		Game::patched = true;
		//FUCK SHIT WE PATCHED
	}

	bool fullscreen = false;

	TTF_Init();

	fpsinit();

	SDL_Window* window = SDL_CreateWindow("Average4k", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Create a OpenGL context on SDL2
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);

	// Load GL extensions using glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize the OpenGL context." << std::endl;
		exit(1);
	}

	std::cout << "OpenGL version loaded: " << GLVersion.major << "."
		<< GLVersion.minor << std::endl;

	GL::projection = glm::ortho(0.0f, (float)1280, (float)720, 0.0f, -1.0f, 1.0f);

	GL::genShader = new Shader();
	GL::genShader->GL_CompileShader(NULL, NULL);
	GL::genShader->GL_Use();
	glUniformMatrix4fv(glGetUniformLocation(GL::genShader->program, "u_projection"), 1, GL_FALSE, &GL::projection[0][0]);

	Rendering::Render_GLInit(GL::genShader);

	glViewport(0, 0, 1280, 720);

	Game* game = new Game();

	Game::instance = game;

	game->gameWidth = 1280;
	game->gameHeight = 720;

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
	//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	float time = 0;
	float bruh = 0;
	double next_tick = (double)SDL_GetTicks();
	SDL_GL_SetSwapInterval(0);
	while (run)
	{
		double now_tick = (double)SDL_GetTicks();
		if (now_tick >= next_tick)
		{
			bruh = time;
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
				case SDL_MOUSEBUTTONDOWN:
					game->mouseButtonDown();
					break;
				case SDL_TEXTINPUT:
					game->textInput(event.text);
					break;
				case SDL_MOUSEWHEEL:
					wheel = event.wheel.y;
					break;
				}

			}

			glClear(GL_COLOR_BUFFER_BIT);

			Events::updateEvent updateEvent;

			updateEvent.renderer = renderer;
			updateEvent.window = window;

			if (!create)
			{
				Game::window = window;
				game->createGame();
				create = true;
			}


			game->update(updateEvent);


			Rendering::drawBatch();

			SDL_GL_SwapWindow(window);

			Game::deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

			fpsthink();


			frames++;

			if (frames > 30)
				frames = 0;

			timestamps[frames] = frames;
			fps[frames] = Game::gameFPS;
			deltaTimes[frames] = Game::deltaTime;
			next_tick += (1000.0 / 240.0);
		}
		else
		{
			SDL_Delay((int)(next_tick - now_tick));
		}


	}

	SDL_StopTextInput();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	if (Game::steam != nullptr)
		Game::steam->ShutdownSteam();

	
	VM_END
		
	return 0;
}