#include "includes.h"
#include "GL.h"
#include "Steam.h"
#include "Game.h"
#include <DbgHelp.h>
#include "AvgSprite.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "log_stream.h"
using namespace std;

#define NOBUF

#undef main

#ifdef STATIC_LINK
#pragma comment(lib,"x64_static\\SDL2.lib")
#pragma comment(lib,"x64\\bass.lib")
#pragma comment(lib,"x64_static\\msgpackc.lib")
#pragma comment(lib,"x64\\bass_fx.lib")
#pragma comment(lib,"x64_static\\SDL2_mixer.lib")
#pragma comment(lib,"x64_static\\SDL2_image.lib")
#pragma comment(lib,"x64_static\\freetype.lib")
#pragma comment(lib,"x64\\steam_api64.lib")
#pragma comment(lib,"x64_static\\libcrypto.lib")
#pragma comment(lib,"x64_static\\libssl.lib")
#pragma comment(lib,"x64_static\\zip.lib")
#pragma comment(lib,"x64_static\\libcurl.lib")
#pragma comment(lib,"x64_static\\boost_regex-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_system-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_exception-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_chrono-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_container-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_context-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_coroutine-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_date_time-vc140-mt.lib")
#pragma comment(lib,"x64_static\\boost_random-vc140-mt.lib")
#pragma comment(lib,"x64_static\\brotlicommon-static.lib")
#pragma comment(lib,"x64_static\\bz2.lib")
#pragma comment(lib,"x64_static\\zlib.lib")
//brotlidec-static.lib
#pragma comment(lib,"x64_static\\brotlidec-static.lib")
#pragma comment(lib,"x64_static\\brotlienc-static.lib")
#pragma comment(lib,"x64_static\\lua.lib")
#pragma comment(lib,"x64_static\\libpng16.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "version.lib")

#else
#pragma comment(lib,"x64\\SDL2.lib")
#pragma comment(lib,"x64\\bass.lib")
#pragma comment(lib,"x64\\msgpackc.lib")
#pragma comment(lib,"x64\\bass_fx.lib")
#pragma comment(lib,"x64\\SDL2_mixer.lib")
#pragma comment(lib,"x64\\SDL2_image.lib")
#pragma comment(lib,"x64\\libfreetype-6.lib")
#pragma comment(lib,"x64\\steam_api64.lib")
#pragma comment(lib,"x64\\libcrypto.lib")
#pragma comment(lib,"x64\\libssl.lib")
#pragma comment(lib,"x64\\zip.lib")
#pragma comment(lib,"x64\\libcurl.lib")
#pragma comment(lib,"x64\\boost_random-vc140-mt.lib")
#pragma comment(lib,"x64\\lua54.lib")
#pragma comment(lib, "dbghelp.lib")
#endif

#define FRAME_VALUES 60


Uint32 frametimes[FRAME_VALUES];

Uint32 frametimelast;

Uint32 framecount;

log_stream* outstream;



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


	if (outstream)
		outstream->dump();
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

void atexit_handler()
{
	if (outstream)
		outstream->dump();
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	
		

	SetUnhandledExceptionFilter(UnhandledExceptionFilterHandler);
	//AddVectoredExceptionHandler(1, &PvectoredExceptionHandler);

	std::atexit(&atexit_handler);
	std::at_quick_exit(atexit_handler);
	
#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#else
	freopen("log.txt", "w", stdout);
	std::cout << "log init" << std::endl;
#ifndef NOBUF
	std::streambuf* origBuf = std::cout.rdbuf();
	log_stream* logstream = new log_stream(origBuf);
	std::cout.set_rdbuf(logstream);
	outstream = logstream;
#endif
#endif
	
	VM_START

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL2 video subsystem couldn't be initialized. Error: "
			<< SDL_GetError()
			<< std::endl;
		exit(1);
	}
	Game::version = "b10";

	curl_global_init(CURL_GLOBAL_ALL);
	Multiplayer::InitCrypto();

	

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
		SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 12);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	// Create a OpenGL context on SDL2
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	// Load GL extensions using glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize the OpenGL context." << std::endl;
		exit(1);
	}

	std::cout << "OpenGL version loaded: " << GLVersion.major << "."
		<< GLVersion.minor << std::endl;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;

	io.DisplaySize.x = (float)200;
	io.DisplaySize.y = (float)200;

	ImGui_ImplSDL2_InitForOpenGL(Game::window, gl_context);
	ImGui_ImplOpenGL3_Init("#version 150");


	ImGui::StyleColorsDark();


	GL::projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	GL::genShader = new Shader();
	GL::genShader->GL_CompileShader(NULL, NULL);
	GL::genShader->GL_Use();
	GL::genShader->setProject(GL::projection);

	Rendering::Render_GLInit(GL::genShader);

	glViewport(0, 0, 1280, 720);

	Game* game = new Game();

	Game::instance = game;

	game->gameWidth = 1280;
	game->gameHeight = 720;

	SDL_StartTextInput();

	bool run = true;

	int lastTime = 0;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	float time = 0;
	float bruh = 0;
	double next_tick = (double)SDL_GetTicks();
	SDL_GL_SetSwapInterval(0);

	VM_END
	MUTATE_START
	while (run)
	{
		double now_tick = (double)SDL_GetTicks();
		if (now_tick >= next_tick)
		{
			glViewport(0, 0, game->wW, game->wH);
			bruh = time;
			const Uint32 startTime = SDL_GetTicks();
			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			Game::startTick = startTime;
			SDL_Event event;
			int wheel = 0;

			while (SDL_PollEvent(&event) > 0)
			{
				ImGui_ImplSDL2_ProcessEvent(&event);
				switch (event.type) {
				case SDL_QUIT: {
					run = false;
					break;
				}
				case SDL_KEYDOWN: {
					game->keyDown(event.key);
					break;
				}
				case SDL_KEYUP: {
					game->keyUp(event.key);
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					game->mouseButtonDown();
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					game->mouseButtonUp();
					break;
				}
				case SDL_TEXTINPUT: {
					game->textInput(event.text);
					break;
				}
				case SDL_MOUSEWHEEL: {
					wheel = event.wheel.y;
					game->mouseWheel(wheel);
					break;
				}
				}

			}

			Events::updateEvent updateEvent;

			updateEvent.renderer = renderer;
			updateEvent.window = window;

			if (!create)
			{
				Game::window = window;
				game->createGame();
				create = true;
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			
			glClear(GL_COLOR_BUFFER_BIT);


			game->update(updateEvent);



			Rendering::drawBatch();

			ImGui::Render();
			glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	MUTATE_END

	VM_START

	SDL_StopTextInput();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	if (Game::steam != nullptr)
		Game::steam->ShutdownSteam();

	VM_END	

		
	return 0;
}