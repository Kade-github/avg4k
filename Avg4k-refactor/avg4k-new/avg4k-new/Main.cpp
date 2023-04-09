#include <numeric>

#include "Average4K.h"
#include "includes.h"
#include "Game.h"
#include "Display.h"
#include "ImGUIHelper.h"


#ifdef STATIC_LINK
#pragma comment(lib,"x64\\bass.lib")
#pragma comment(lib,"x64_static\\msgpackc.lib")
#pragma comment(lib,"x64\\bass_fx.lib")
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
#pragma comment(lib,"x64_static\\zlib.lib")
#pragma comment(lib,"x64_static\\glfw3_mt.lib") // multi-threaded version of glfw
//#pragma comment(lib,"x64_static\\ryml.lib")
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
#pragma comment(lib,"x64\\glfw3_mt.lib") // multi-threaded version of glfw
#pragma comment(lib,"x64\\bass.lib")
#pragma comment(lib,"x64\\msgpackc.lib")
#pragma comment(lib,"x64\\bass_fx.lib")
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

#define SOL_LUAJIT 1

class Average4K;
using namespace AvgEngine;

#define FRAME_VALUES 60

typedef std::chrono::high_resolution_clock Clock;

double frametimes[FRAME_VALUES];

double frametimelast;

std::chrono::steady_clock::time_point startTime;

uint32_t framecount;

void preciseSleep(double seconds) {
	using namespace std;
	using namespace std::chrono;

	static double estimate = 5e-3;
	static double mean = 5e-3;
	static double m2 = 0;
	static int64_t count = 1;

	while (seconds > estimate) {
		auto start = high_resolution_clock::now();
		Sleep(1);
		auto end = high_resolution_clock::now();

		double observed = (end - start).count() / 1e9;
		seconds -= observed;

		++count;
		double delta = observed - mean;
		mean += delta / count;
		m2 += delta * (observed - mean);
		double stddev = sqrt(m2 / (count - 1));
		estimate = mean + stddev;
	}

	// spin lock
	auto start = high_resolution_clock::now();
	while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void fpsthink(Game* g) {
	uint32_t frametimesindex;
	uint32_t count;
	uint32_t i;

	frametimesindex = framecount % FRAME_VALUES;

	double nowmills = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count());

	frametimes[frametimesindex] = nowmills - frametimelast;

	frametimelast = nowmills;

	framecount++;

	if (framecount < FRAME_VALUES)
		count = framecount;
	else
		count = FRAME_VALUES;

	g->fps = 0;
	for (i = 0; i < count; i++)
		g->fps += frametimes[i];

	g->fps /= count;
	g->fps = std::floorf(1000.f / (float)g->fps);
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#else
	Logging::openLog();
#endif
	Logging::writeLog("[Main] Logging system initialized.");

	startTime = Clock::now();

	if (!glfwInit())
	{
		Logging::writeLog("[GLFW] Failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}
	Logging::writeLog("[Main] Initialized GLFW!");

	glfwSetErrorCallback([](int error, const char* description)
		{
			Logging::writeLog("[GLFW] [Error] " + std::string(description) + " - Code: " + std::to_string(error));
		});

	External::BASS::Initialize();
	Logging::writeLog("[Main] Initialized BASS!");
	curl_global_init(CURL_GLOBAL_ALL);

	// Wine check
	const char* (CDECL * pwine_get_version)(void);
	HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
	if (!hntdll)
	{
		Logging::writeLog("[Main] Error retrieving NTDLL handle");
	}

	pwine_get_version = (const char* (*)())GetProcAddress(hntdll, "wine_get_version");

	if (pwine_get_version) {
		Logging::writeLog("[Main] Running on wine version " + std::string(pwine_get_version()));
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);


	Average4K* g = new Average4K("Average4K", "b14");
	g->alpha = true;

	glfwMakeContextCurrent(g->Window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(g->Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Events::Event e;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = Events::EventType::Event_KeyPress;
				e.data = key;
				Game::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = Events::EventType::Event_KeyRelease;
				e.data = key;
				Game::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetMouseButtonCallback(g->Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			Events::Event e;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			Events::Vec v;
			v.x = xpos;
			v.y = ypos;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = Events::EventType::Event_MouseDown;
				e.data = button;
				e.vector = v;
				Game::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = Events::EventType::Event_MouseRelease;
				e.data = button;
				e.vector = v;
				Game::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetScrollCallback(g->Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			Events::Event e;
			e.type = Events::EventType::Event_MouseScroll;
			e.data = yoffset;
			Game::Instance->QueueEvent(e);
		});

	Render::Display::Init();
	Logging::writeLog("[Main] Initialized Display!");

	External::ImGuiHelper::Init(g->Window);
	Logging::writeLog("[Main] Initialized ImGUI!");


	Logging::writeLog("[Main] Starting game...");

	g->Start();

	int frames = 0;

	int reportId = 0;

	glEnable(GL_MULTISAMPLE);

	glfwSwapInterval(0);
	Render::Display::defaultShader->setProject(g->CurrentMenu->camera.projection);
	auto start = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(g->Window))
	{
		++frames;
		auto now = std::chrono::steady_clock::now();
		auto diff = now - start;
		auto end = now + std::chrono::milliseconds(static_cast<int>((1.0f / g->fpsCap) * 1000));

		fpsthink(g);

		glClearColor(0.05f, 0.05f, 0.05f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		External::ImGuiHelper::RenderStart();

		g->update();

		g->console.drawData[reportId] = g->CurrentMenu->camera.drawCalls.size();
		g->console.update();

		g->CurrentMenu->cameraDraw();

		External::ImGuiHelper::RenderEnd(g->Window);

		glfwSwapBuffers(g->Window);
		glfwPollEvents();

		if (g->fps < 0)
			g->fps = 0;
		if (g->fps > g->fpsCap) // heehee
			g->fps = g->fpsCap;

		reportId++;
		if (reportId == 50)
			reportId = 0;

		g->console.fpsData[reportId] = g->fps;
		std::this_thread::sleep_until(end);
	}

	// let it rain the color of blood. あか

	g->Destroy();

	External::ImGuiHelper::Destroy();

	glfwDestroyWindow(g->Window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}