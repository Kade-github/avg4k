#include "includes.h"
#include "Game.h"
#include "Display.h"
#include "StartScreen.h"
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

using namespace AvgEngine;

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


	Game* g = new Game("Average4K", "b14");

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
			switch (action)
			{
			case GLFW_PRESS:
				e.type = Events::EventType::Event_MouseDown;
				e.data = button;
				Game::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = Events::EventType::Event_MouseRelease;
				e.data = button;
				Game::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	Render::Display::Init();
	Logging::writeLog("[Main] Initialized Display!");

	External::ImGuiHelper::Init(g->Window);
	Logging::writeLog("[Main] Initialized ImGUI!");

	g->SwitchMenu(new StartScreen());

	Logging::writeLog("[Main] Starting game...");

	glfwSwapInterval(0);

	double lastTime = glfwGetTime();
	double fTime = glfwGetTime();
	int frames = 0;

	int reportId = 0;

	while (!glfwWindowShouldClose(g->Window))
	{
		glClearColor(0.05f, 0.05f, 0.05f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Measure speed
		double currentTime = glfwGetTime();

		double d = std::abs(lastTime - fTime);

		if (d > 1)
		{
			fTime = glfwGetTime();
			g->fps = frames;
			frames = 0;
		}

		reportId++;
		if (reportId == 25)
			reportId = 0;

		g->console.fpsData[reportId] = g->fps;
		g->console.drawData[reportId] = g->CurrentMenu->camera.drawCalls.size();
		frames++;

		glfwPollEvents();

		External::ImGuiHelper::RenderStart();

		g->update();
		g->CurrentMenu->draw();

		External::ImGuiHelper::RenderEnd(g->Window);

		glfwSwapBuffers(g->Window);

		while (glfwGetTime() < lastTime + 1.0 / 244) {
			Sleep(1);
		}
		lastTime += 1.0 / 244;
	}

	External::ImGuiHelper::Destroy();

	glfwDestroyWindow(g->Window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}