/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma comment(lib,"Library\\x64_static\\opengl.lib")
#pragma comment(lib,"Library\\x64_static\\bass.lib")
#pragma comment(lib,"Library\\x64_static\\bass_fx.lib")
#pragma comment(lib,"Library\\x64_static\\msgpackc.lib")
#pragma comment(lib,"Library\\x64_static\\glfw3_mt.lib")
#pragma comment(lib,"Library\\x64_static\\lua51.lib")
#pragma comment(lib,"Library\\x64_static\\steam_api64.lib")
#pragma comment(lib,"Library\\x64_static\\libcrypto.lib")
#pragma comment(lib,"Library\\x64_static\\libssl.lib")

#ifdef _DEBUG
#pragma comment(lib,"Library\\x64_static\\Debug\\AvgEngine.lib")
#else
#pragma comment(lib,"Library\\x64_static\\Release\\AvgEngine.lib")
#endif
#include <thread>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Average4k/Data/SaveData.h"
#include "Average4k/A4kGame.h"
#include "Average4k/Screens/Menu/MainMenu.h"

#include <AvgEngine/External/Bass/BASS.h>
#include <AvgEngine/External/ImGui/ImGUIHelper.h>

typedef std::chrono::high_resolution_clock Clock;
double frametimes[60];
double frametimelast;
std::chrono::steady_clock::time_point startTime;
uint32_t framecount;

#include <DbgHelp.h>

void CrashDmp(_EXCEPTION_POINTERS* ExceptionInfo) {

#ifdef _DEBUG
	return;
#endif

	if (!ExceptionInfo)
	{
		AvgEngine::Logging::writeLog("no crash :)");
		return;
	}
	OFSTRUCT data;
	HANDLE file = CreateFile(L"crash.dmp", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	MINIDUMP_EXCEPTION_INFORMATION info;
	info.ClientPointers = false;
	info.ExceptionPointers = ExceptionInfo;
	info.ThreadId = GetCurrentThreadId();

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithFullMemory, &info, NULL, NULL);
	CloseHandle(file);
	AvgEngine::Logging::writeLog("Dumped crashlog");
	AvgEngine::Logging::closeLog();
}

void fpsthink(AvgEngine::Game* g) {
	uint32_t frametimesindex;
	uint32_t count;
	uint32_t i;

	frametimesindex = framecount % 60;

	double nowmills = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count());

	frametimes[frametimesindex] = nowmills - frametimelast;

	frametimelast = nowmills;

	framecount++;

	if (framecount < 60)
		count = framecount;
	else
		count = 60;

	g->fps = 0;
	for (i = 0; i < count; i++)
		g->fps += frametimes[i];

	g->fps /= count;
	g->fps = std::floorf(1000.f / (float)g->fps);
}

LONG PvectoredExceptionHandler(
	_EXCEPTION_POINTERS* ExceptionInfo
)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x40010006 || ExceptionInfo->ExceptionRecord->ExceptionCode == 0xE24C4A02 || ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000003 || ExceptionInfo->ExceptionRecord->ExceptionCode == 0xE06D7363)
		return EXCEPTION_CONTINUE_SEARCH;


	CrashDmp(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

long WINAPI UnhandledExceptionFilterHandler(LPEXCEPTION_POINTERS ex) {

	if (ex->ExceptionRecord->ExceptionCode == 0x40010006 || ex->ExceptionRecord->ExceptionCode == 0xE24C4A02 || ex->ExceptionRecord->ExceptionCode == 0x80000003 || ex->ExceptionRecord->ExceptionCode == 0xE06D7363)
		return EXCEPTION_CONTINUE_SEARCH;

	CrashDmp(ex);
	return EXCEPTION_CONTINUE_SEARCH;
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#else
	AvgEngine::Logging::openLog();
#endif
	AvgEngine::Logging::writeLog("[Main] Logging system initialized.");

	SetUnhandledExceptionFilter(UnhandledExceptionFilterHandler);

	AddVectoredExceptionHandler(1, &PvectoredExceptionHandler);

	AvgEngine::Logging::writeLog("[Main] Set crash handlers...");

	if (!glfwInit())
	{
		AvgEngine::Logging::writeLog("[GLFW] Failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	AvgEngine::Logging::writeLog("[GLFW] GLFW initialized.");

	glfwSetErrorCallback([](int error, const char* description)
		{
			AvgEngine::Logging::writeLog("[GLFW] [Error] " + std::string(description) + " - Code: " + std::to_string(error));
		});

	// Wine check
	const char* (CDECL * pwine_get_version)(void);
	HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
	if (!hntdll)
	{
		AvgEngine::Logging::writeLog("[Main] Error retrieving NTDLL handle");
	}

	pwine_get_version = (const char* (*)())GetProcAddress(hntdll, "wine_get_version");

	if (pwine_get_version)
	{
		AvgEngine::Logging::writeLog("[Main] Running on wine version " + std::string(pwine_get_version()));
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	using namespace Average4k;

	A4kGame* game = new A4kGame("Average4k", A_VERSION);

	glfwMakeContextCurrent(game->Window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwMakeContextCurrent(NULL);

	AvgEngine::Logging::writeLog("[GLAD] GLAD initialized.");

	glfwSetKeyCallback(game->Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			AvgEngine::Events::Event e;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = AvgEngine::Events::EventType::Event_KeyPress;
				e.data = key;
				A4kGame::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = AvgEngine::Events::EventType::Event_KeyRelease;
				e.data = key;
				A4kGame::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetMouseButtonCallback(game->Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			AvgEngine::Events::Event e;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			AvgEngine::Events::Vec v;
			v.x = xpos;
			v.y = ypos;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = AvgEngine::Events::EventType::Event_MouseDown;
				e.data = button;
				e.vector = v;
				A4kGame::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = AvgEngine::Events::EventType::Event_MouseRelease;
				e.data = button;
				e.vector = v;
				A4kGame::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetWindowSizeCallback(game->Window, [](GLFWwindow* window, int width, int height)
		{
			AvgEngine::Events::Event e;
			e.type = AvgEngine::Events::EventType::Event_WindowResize;
			e.data = width;
			e.vector.x = width;
			e.vector.y = height;
			A4kGame::Instance->QueueEvent(e);
		});

	glfwSetScrollCallback(game->Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			AvgEngine::Events::Event e;
			e.type = AvgEngine::Events::EventType::Event_MouseScroll;
			e.data = yoffset;
			A4kGame::Instance->QueueEvent(e);
		});

	AvgEngine::Logging::writeLog("[Main] Starting game...");

	game->Start();

	// Render thread

	bool shouldQuit = false;

	Screens::Menu::MainMenu* cm = new Screens::Menu::MainMenu("Scripts/MainMenu.lua");

	std::thread t = std::thread([&] {
		glfwMakeContextCurrent(game->Window);
		glfwSwapInterval(0);

		AvgEngine::External::BASS::Initialize();
		AvgEngine::Logging::writeLog("[Main] Initialized BASS!");;

		AvgEngine::Render::Display::Init();
		AvgEngine::Logging::writeLog("[Main] Initialized Display!");

		AvgEngine::External::ImGuiHelper::Init(game->Window);
		AvgEngine::Logging::writeLog("[Main] Initialized ImGUI!");

		game->NextMenu = cm;
		game->Switch();

		auto start = std::chrono::steady_clock::now();
		int frames = 0;

		while (!shouldQuit)
		{
			++frames;
			auto now = std::chrono::steady_clock::now();
			auto diff = now - start;
			auto end = now + std::chrono::milliseconds(static_cast<int>((1.0f / game->fpsCap) * 1000));

			fpsthink(game);

			glClearColor(0.05f, 0.05f, 0.05f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			AvgEngine::External::ImGuiHelper::RenderStart();

			game->update();

			if (game->CurrentMenu != NULL)
			{
				game->console.update();

				game->CurrentMenu->cameraDraw();
			}

			AvgEngine::External::ImGuiHelper::RenderEnd(game->Window);

			glfwSwapBuffers(game->Window);

			std::this_thread::sleep_until(end);

		}

		AvgEngine::External::ImGuiHelper::Destroy();

	});

	while (!glfwWindowShouldClose(game->Window))
	{
		glfwWaitEvents();
	}

	shouldQuit = true;

	game->Destroy();
	glfwDestroyWindow(game->Window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}