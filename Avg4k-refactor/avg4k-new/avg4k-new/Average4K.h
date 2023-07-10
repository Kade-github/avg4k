#pragma once
#include "includes.h"
#include "Game.h"
#include "Multiplayer.h"
#include "Paths.h"
#include "Settings.h"
#include "LoadingPacksMenu.h"
#include "StartScreen.h"
#include "SteamIncludes.h"
#include "Avg4kCmdHandler.h"
#include "Notifications.h"
#include "SongGatherer.h"

namespace Average4k
{
	struct GameOptions {
		Average4k::Chart::ChartFile* currentFile = NULL;
		int currentFile_diff;

		float drawbeats = 8;

		std::vector<float> judgeWindow = {22.5f, 45.0f, 90.0f, 135.0f, 180.0f, 250.0f, 75.0f};
		bool inGameplay = false;
	};
}

class Average4K : public AvgEngine::Game
{
	double _startTrans = -1;
	bool _out = false;
	Menu* _toSwitch;

public:
	static std::vector <Average4k::Chart::Pack> packs;
	static Average4k::Settings* settings;
	static Average4k::Skin* skin;

	static Average4k::Utils::Notifications* notif;

	static bool instant;

	bool skinInitOnLoad = true;

	Average4k::GameOptions options{};

	Steam::SteamInterface* steam;
	Steam::SteamWorkshop* workshop;

	std::vector<QueuedPacket> queuedPackets;

	Average4K(std::string _t, std::string _v, int w = 1920, int h = 1080) : Game(_t, _v, w, h)
	{
		steam = new Steam::SteamInterface();
		workshop = new Steam::SteamWorkshop();
		settings = new Average4k::Settings(AvgEngine::Utils::Paths::getAppData("Average4K") + "settings.ave");
		notif = new Average4k::Utils::Notifications(0,0);
		queuedPackets = {};
	}

	void SkinInit()
	{
		skinInitOnLoad = false;
		if (skin)
			skin->EmptyCache();
		skin = new Average4k::Skin(settings->Get("Skin").value, "assets/noteskin/");

		AvgEngine::Fnt::Fnt::ClearCache();

		std::string displayV = settings->Get("Display").value;

		SetResolution(settings->Get("Resolution").value);
		SetFullscreen(displayV);
		settings->Set("Resolution", std::to_string(AvgEngine::Render::Display::width) + "x" + std::to_string(AvgEngine::Render::Display::height));

		fpsText->SetFont(skin->GetFontPath(), "FuturaBold.fnt");
		alphaText->SetFont(skin->GetFontPath(), "FuturaBold.fnt");

		QueueEvent({ AvgEngine::Events::EventType::Event_ReloadFont,0,0, {}, skin->GetFontPath() });

		notif->InitNotifications(skin);
	}

	void SetSkin(std::string skinName, bool setValue = true, bool startScreen = true)
	{
		if (!AvgEngine::Utils::Paths::pathExists("assets/noteskin/" + skinName))
		{
			if (skinName == "arrow")
			{
				AvgEngine::Logging::writeLog("[Fatal Error] Skin " + skinName + " is invalid! (Not located at assets/noteskins/" + skinName + "). This will cause the game to crash!");
				glfwSetWindowShouldClose(Window, GLFW_TRUE);
				return;
			}
			AvgEngine::Logging::writeLog("[Error] Skin " + skinName + " is invalid! (Not located at assets/noteskins/" + skinName + ").");
			if (!startScreen)
			{
				AvgEngine::Logging::writeLog("[Error] Trying to reset to default skin.");
				SetSkin("arrow", true, false);
			}
			return;
		}
		const std::string n = skinName;
		if (setValue)
		{
			Average4k::Setting& s = settings->Get("Skin");
			s.value = n;
		}


		if (startScreen)
		{
			SwitchMenu(new StartScreen());
			skinInitOnLoad = true;
		}
		else
		{
			SkinInit();
			SwitchNoTrans(new LoadingPacksMenu());
		}
	}

	std::vector<std::string> GetSkins()
	{
		std::vector<std::string> skins = {};
		for (const auto& entry : std::filesystem::directory_iterator("assets/noteskin/"))
		{
			if (entry.is_directory())
			{
				std::string name = entry.path().filename().string();
				if (AvgEngine::Utils::Paths::pathExists("assets/noteskin/" + name + "/config.skin"))
					skins.push_back(name);
			}
		}
		return skins;
	}

	void SetFullscreen(std::string type)
	{
		if (type == "Windowed")
			AvgEngine::Render::Display::Fullscreen(Window, 0);
		else if (type == "Fullscreen")
			AvgEngine::Render::Display::Fullscreen(Window, 1);
		else if (type == "Borderless")
		{
			AvgEngine::Render::Display::Fullscreen(Window, 2);
			skin->upscale = static_cast<float>(AvgEngine::Render::Display::width) / 1920.0f;
			fpsText->transform.scale = skin->upscale;
			alphaText->transform.scale = skin->upscale;
		}
		else
			AvgEngine::Render::Display::Fullscreen(Window, -1);

		if (CurrentMenu)
		{
			CurrentMenu->camera.resize(AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);
			AvgEngine::Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
		}
		Event(AvgEngine::Events::Event(AvgEngine::Events::EventType::Event_Resize));

		fpsText->transform.scale = skin->upscale;
		alphaText->transform.scale = skin->upscale;
	}

	void SetResolution(std::string res, bool windowed = true)
	{
		if (res == "max")
		{
			int* maxRes = AvgEngine::Render::DisplayHelper::getMonitorResolution();
			skin->upscale = static_cast<float>(maxRes[0]) / 1920.0f;
			fpsText->transform.scale = skin->upscale;
			alphaText->transform.scale = skin->upscale;
			AvgEngine::Render::Display::width = maxRes[0];
			AvgEngine::Render::Display::height = maxRes[1];
			if (CurrentMenu)
			{
				CurrentMenu->camera.resize(AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);
				AvgEngine::Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
			}
			Event(AvgEngine::Events::Event(AvgEngine::Events::EventType::Event_Resize));

			settings->Set("Resolution", std::to_string(maxRes[0]) + "x" + std::to_string(maxRes[1]));
			return;
		}
		std::vector<std::string> s = AvgEngine::Utils::StringTools::Split(res, "x");
		int w = std::stoi(s[0]);
		int h = std::stoi(s[1]);

		skin->upscale = static_cast<float>(w) / 1920.0f;

		if (windowed)
			Game::Resize(w, h);
		if (CurrentMenu)
		{
			CurrentMenu->camera.resize(AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);
			AvgEngine::Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
		}

		Event(AvgEngine::Events::Event(AvgEngine::Events::EventType::Event_Resize));
		fpsText->transform.scale = skin->upscale;
		alphaText->transform.scale = skin->upscale;
	}

	void Start()
	{
		fpsText = new Text(4, 4, "", "", "FPS: 0", 12);
		fpsText->transform.a = 0.6f;

		alphaText = new Text(4, 4, "", "", "- " + Title + " ALPHA " + Version + " - EVERYTHING IS SUBJECT TO CHANGE -", 14);
		alphaText->transform.a = 0.6f;

		console.handler = new Avg4kCmdHandler();
		if (!steam->good)
			return;

		notif->eManager = &eManager;
		notif->Added();

		steam->SetPresence(Version + " Alpha | Testing");
		InitMultiplayer();

		SwitchNoTrans(new LoadingPacksMenu());
	}

	void update() override
	{
		SteamAPI_RunCallbacks();

		Game::update();

		if (!CurrentMenu)
			return;


		// Special text draws'

		using namespace AvgEngine::Render;

		fpsText->parent = &CurrentMenu->displayRect;
		alphaText->parent = &CurrentMenu->displayRect;

		if (alpha)
		{
			alphaText->transform.x += 1;
			alphaText->camera = &CurrentMenu->camera;
			if (alphaText->transform.x > Display::width + alphaText->transform.w)
				alphaText->transform.x = -alphaText->transform.w;
			alphaText->draw();
		}

		fpsText->text = "FPS: " + std::to_string(fps);
		fpsText->camera = &CurrentMenu->camera;
		fpsText->draw();

		// Rect transition
		Rect r = Rect(
			0.0f,0.0f, Display::width, Display::height,
			0.0f,0.0f,0.0f,1.0f);

		notif->camera = &CurrentMenu->camera;
		notif->parent = &CurrentMenu->displayRect;
		notif->draw();

		// 0.2 second trans time
		if (!_out && _startTrans > 0)
		{
			float t = std::min(static_cast<float>(std::abs(_startTrans - glfwGetTime())) / 0.20f, 1.0f);
			r.a = std::lerp(0, 1, t);
			drawCall c = Camera::FormatDrawCall(0, NULL, NULL, DisplayHelper::RectToVertex(r, { 0,0,1,1 }));
			c.zIndex = 99;
			CurrentMenu->camera.addDrawCall(c);

			if (t >= 1)
			{
				_startTrans = glfwGetTime();
				if (skinInitOnLoad)
					SkinInit();
				SwitchNoTrans(_toSwitch);

				_toSwitch->camera.resize(AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);
				AvgEngine::Render::Display::defaultShader->setProject(_toSwitch->camera.projection);
				Event(AvgEngine::Events::Event(AvgEngine::Events::EventType::Event_Resize));
				_out = true;
			}
		}
		else
		{
			float t = std::min(static_cast<float>(std::abs(_startTrans - glfwGetTime())) / 0.20f, 1.0f);
			r.a = std::lerp(1, 0, t);
			drawCall c = Camera::FormatDrawCall(0, NULL, NULL, DisplayHelper::RectToVertex(r, { 0,0,1,1 }));
			c.zIndex = 99;
			CurrentMenu->camera.addDrawCall(c);
		}

		if (queuedPackets.size() > 0)
			queuedPackets.clear();

	}

	void SwitchNoTrans(Menu* menu)
	{
		Game::SwitchMenu(menu);
	}

	void SwitchMenu(Menu* menu) override
	{
		if (instant)
		{
			instant = false;
			SwitchNoTrans(menu);
			return;
		}
		_toSwitch = menu;
		_startTrans = glfwGetTime();
		_out = false;
	}

	void Destroy()
	{
		steam->destroy();
		workshop->destroy();
	}

	void InitMultiplayer()
	{
		Multiplayer::InitCrypto();

		std::jthread MultiplayerThread = std::jthread([this] {
			Multiplayer::connect(this);
			});
		MultiplayerThread.detach();
	}
};