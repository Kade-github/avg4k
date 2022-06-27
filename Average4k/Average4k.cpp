#include "includes.h"
#include "GL.h"
#include "Steam.h"
#include "Game.h"
#include <DbgHelp.h>
#include "AvgSprite.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "log_stream.h"
#include "Helpers.h"
#include <timeapi.h>
#include "Average4k.h"
using namespace std;

//#define NOBUF

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
#pragma comment(lib,"x64\\discord_game_sdk.lib")
#pragma comment(lib,"x64_static\\bz2.lib")
#pragma comment(lib,"x64_static\\zlib.lib")
//brotlidec-static.lib
#pragma comment(lib,"x64_static\\brotlidec-static.lib")
#pragma comment(lib,"x64_static\\brotlienc-static.lib")
#pragma comment(lib,"x64_static\\lua.lib")
#pragma comment(lib,"x64_static\\libpng16.lib")
#pragma comment(lib, "x64_static\\boost_atomic-vc140-mt.lib")
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

typedef std::chrono::high_resolution_clock Clock;

#define FRAME_VALUES 60


double frametimes[FRAME_VALUES];

double frametimelast;

std::chrono::steady_clock::time_point startTime;

Uint32 framecount;

std::string Average4k::path = "";

log_stream* outstream;



void fpsinit() {
	memset(frametimes, 0, sizeof(frametimes));
	framecount = 0;
	frametimelast = 0;
}

void fpsthink() {
	Uint32 frametimesindex;
	Uint32 count;
	Uint32 i;

	frametimesindex = framecount % FRAME_VALUES;

	double nowmills = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count());

	frametimes[frametimesindex] = nowmills - frametimelast;

	frametimelast = nowmills;

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

	if (!ExceptionInfo )
	{
		std::cout << "no crash :)" << std::endl;
		if (outstream)
			outstream->dump();
		return;
	}
	OFSTRUCT data;
	HANDLE file = CreateFile(L"crash.dmp", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	MINIDUMP_EXCEPTION_INFORMATION info;;
	info.ClientPointers = false;
	info.ExceptionPointers = ExceptionInfo;
	info.ThreadId = GetCurrentThreadId();

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithFullMemory, &info, NULL, NULL);
	CloseHandle(file);
	std::cout << "Dumped crashlog" << std::endl;


	if (outstream)
		outstream->dump();
}

LONG PvectoredExceptionHandler(
	_EXCEPTION_POINTERS* ExceptionInfo
)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x40010006)
		return EXCEPTION_CONTINUE_SEARCH;


	CrashDmp(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

long WINAPI UnhandledExceptionFilterHandler(LPEXCEPTION_POINTERS ex) {

	CrashDmp(ex);
	return EXCEPTION_CONTINUE_SEARCH;
}

void atexit_handler()
{
	if (outstream)
		outstream->dump();
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{


	std::string cmdLine(lpCmdLine);

	SetUnhandledExceptionFilter(UnhandledExceptionFilterHandler);

	AddVectoredExceptionHandler(1, &PvectoredExceptionHandler);


	std::atexit(&atexit_handler);
	std::at_quick_exit(atexit_handler);

#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#else
	freopen("log.txt", "w", stdout);
	std::cout << "log init" << std::endl;
#ifndef NOBUF

	if (cmdLine.find("-nologbuf") != std::string::npos) {
		std::cout << "Not using log buffers - could introduce stuttering." << std::endl;
	}
	else {
		std::streambuf* origBuf = std::cout.rdbuf();
		log_stream* logstream = new log_stream(origBuf);
		std::cout.set_rdbuf(logstream);
		outstream = logstream;
	}
#endif
#endif
	
	VM_START

	startTime = Clock::now();

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL2 video subsystem couldn't be initialized. Error: "
			<< SDL_GetError()
			<< std::endl;
		exit(1);
	}
	Game::version = "b12.0";

	curl_global_init(CURL_GLOBAL_ALL);
	Multiplayer::InitCrypto();

	Game::frameLimit = 10;

	BASS_Init(-1,44100,0,NULL,NULL);
	BASS_SetConfig(BASS_CONFIG_ASYNCFILE_BUFFER, 32000000); //32MB

	int test = 123;

	CHECK_CODE_INTEGRITY(test, 6969);

	//Game::patched = true;

	if (test != 6969) {

		Game::patched = true;
		//FUCK SHIT WE PATCHED
	}

	bool disableFrameSleeping = cmdLine.find("-disableframesleep") != std::string::npos;

	if (disableFrameSleeping) {
		std::cout << "Frame sleep disabled, increased cpu usage expected." << std::endl;
	}

	const char* (CDECL * pwine_get_version)(void);
	HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
	if (!hntdll)
	{
		std::cout << "Error retrieving NTDLL handle" << std::endl;
	}
	
	pwine_get_version = (const char* (*)())GetProcAddress(hntdll, "wine_get_version");

	if (pwine_get_version) {
		std::cout << "Running on wine version " << pwine_get_version() << std::endl;
	}

	bool fullscreen = false;

	TTF_Init();

	fpsinit();
	
	SDL_Window* window = SDL_CreateWindow("Average4k", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	
	int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	if (status != 0) {
		std::cout << "Failed to initialize the OpenGL context (" << status << ")" << std::endl;
		exit(1);
	}

	status = SDL_GL_MakeCurrent(window, gl_context);
	
	if (status != 0) {
		std::cout << "Error making current: " << SDL_GetError() << std::endl;
	}
	


	//NO MORE SDL RENDERER!!!! IT IS MUTUALLY EXCLUSIVE TO OPENGL!!! READ THE FUCKING DOCS KADE!!!!!!!
	//SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	std::cout << "OpenGL version loaded: " << GLVersion.major << "."
		<< GLVersion.minor << std::endl;

	glEnable(GL_MULTISAMPLE);

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

	Average4k::path = std::filesystem::current_path().string();

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
	//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	float time = 0;
	float bruh = 0;
	auto next_tick = Clock::now();
	SDL_GL_SetSwapInterval(0);

	double performanceFrequency = SDL_GetPerformanceFrequency();

	int lastFramelimit = 0;

	VM_END
	MUTATE_START
	while (run)
	{
		auto now_tick = Clock::now();
		if (now_tick >= next_tick)
		{
			if (lastFramelimit != Game::frameLimit)
			{
				lastFramelimit = Game::frameLimit;
				now_tick = Clock::now();
				next_tick = Clock::now();
			}
			glViewport(0, 0, game->wW, game->wH);
			bruh = time;
			const Uint32 startTick = SDL_GetTicks();
			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			Game::startTick = startTick;
			SDL_Event event;
			int wheel = 0;

			while (SDL_PollEvent(&event) > 0)
			{
				if(Game::useImGUI)
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
				case SDL_DROPFILE: {
					game->dropFile(event.drop);
				}
				}

			}

			Events::updateEvent updateEvent;

			updateEvent.renderer = nullptr;
			updateEvent.window = window;

			if (!create)
			{
				Game::window = window;
				game->createGame();
				create = true;
			}


			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			game->update(updateEvent);

			Rendering::drawBatch();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			SDL_GL_SwapWindow(window);

			Game::deltaTime = (double)((NOW - LAST) * 1000 / (double)performanceFrequency);
			fpsthink();


			frames++;

			if (frames > 30)
				frames = 0;

			timestamps[frames] = frames;
			fps[frames] = Game::gameFPS;
			deltaTimes[frames] = Game::deltaTime;

			if (Clock::now() - startTime < std::chrono::seconds(5))
				Game::frameLimit = 60;

			next_tick = now_tick + std::chrono::nanoseconds((int)(1e9 / Game::frameLimit));

			if (!disableFrameSleeping) {
				timeBeginPeriod(1);
				Helpers::preciseSleep(std::chrono::duration_cast<std::chrono::microseconds>(next_tick - Clock::now()).count() / 1e6);
				timeEndPeriod(1);
			}
		}
		else
		{
		
		}


	}
	MUTATE_END

	VM_START

	SDL_StopTextInput();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);

	SDL_Quit();

	if (Game::steam != nullptr)
		Game::steam->ShutdownSteam();

	VM_END	

		
	return 0;
}

void Average4k::dumpOutstream()
{
	if (outstream)
		outstream->dump();
}
