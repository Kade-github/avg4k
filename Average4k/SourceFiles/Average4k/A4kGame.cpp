/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "A4kGame.h"

#include <AvgEngine/External/Image/stbi.h>
#include "Data/Chart/ChartFinder.h"
#include "Console/CmdHandler.h"
#include <AvgEngine/External/Bass/BASS.h>
#include "Api/Functions/FData.h"
#include "Api/Functions/FGame.h"
#include <AvgEngine/Utils/Paths.h>

#include "Helpers/SteamHelper.h"

Average4k::Multiplayer::Connection* c;

Average4k::A4kGame* Average4k::A4kGame::gameInstance = nullptr;

Average4k::A4kGame::A4kGame(std::string _t, std::string _v, int w, int h) : Game(_t, _v, w, h)
{
	fpsCap = 144;
	gameInstance = this;
}

void Average4k::A4kGame::Destroy()
{
	saveData.Save("Assets/Save/Save.avg");

	// Stop all audio
	for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
	{
		if (c == NULL || c->id == -1)
			continue;
		if (c->isPlaying)
			c->Stop();
		delete c;
	}

	AvgEngine::External::BASS::Channels.clear();

	// Disconnect from server

	c->inQuotesGracefullyDisconnect();

	// Destroy steam

	Helpers::SteamHelper::Destroy();
}

void Average4k::A4kGame::Start()
{
	// Set window icon

	GLFWimage images[4];

	images[0].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon32.png", &images[0].width, &images[0].height);
	images[1].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon64.png", &images[3].width, &images[3].height);
	images[2].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon128.png", &images[2].width, &images[2].height);
	images[3].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon256.png", &images[1].width, &images[1].height);
	glfwSetWindowIcon(Window, 1, images);

	AvgEngine::Logging::writeLog("[Average4k] Set window icon!");

	// Get save file

	saveData = Data::SaveData::Load("Assets/Save/Save.avg");

	skin = Skin(saveData.skinData);

	skinExists = AvgEngine::Utils::Paths::pathExists("Assets/Skins/" + saveData.skinData.name);

	console.handler = new Average4k::Console::CmdHandler();


	if (!skinExists)
	{
		AvgEngine::Logging::writeLog("[Average4k] Skin \"" + saveData.skinData.name + "\" not found, trying to use the default skin!");
		saveData.skinData = Data::Types::SkinData();
		skin = Skin(saveData.skinData);

		skinExists = AvgEngine::Utils::Paths::pathExists("Assets/Skins/" + saveData.skinData.name);

		if (!skinExists)
		{
			AvgEngine::Logging::writeLog("[Average4k] [Error] Default skin not found, the game will not load.");
			A4kGame::gameInstance->setDebugText(true); // set fallback
			return;
		}
	}

	// Set res

	Data::Types::VideoData& v = saveData.videoData;

	int* res = AvgEngine::Render::DisplayHelper::getMonitorResolution();

	if (v.width >= res[0])
		v.width = res[0];
	if (v.height >= res[1])
		v.height = res[1];

	if (v.width < 640)
		v.width = 640;

	if (v.height < 480)
		v.height = 480;

	A4kGame::Instance->Resize(v.width, v.height);

	if (v.fullscreen)
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 1);
	}
	else if (v.borderless)
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 2);
	}
	else
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 0);
	}

	// Load charts

	Data::ChartFinder::FindCharts("Charts");

	// Init steam

	Helpers::SteamHelper::Initialize();

	if (Helpers::SteamHelper::IsSteamRunning)
	{
		Helpers::SteamHelper::SetPresence("Alpha Version: " + std::string(A_VERSION));

		// Connect to the server


		c = new Multiplayer::Connection();

		c->InitCrypto();

		std::jthread t(c->connect);

		t.detach();
	}
	else
		AvgEngine::Logging::writeLog("[Average4k] [Warning] Steam is not running, running in offline mode.");
}

void Average4k::A4kGame::update()
{
	// Run steam callbacks
	SteamAPI_RunCallbacks();

	if (!skinExists)
	{
		DrawOutlinedDebugText(20 * Average4k::Api::Functions::FGame::GetWidthScale(), 20 * Average4k::Api::Functions::FGame::GetHeightScale(), "The game couldn't find a skin to use, please re-download/verify your game.", 32 * Average4k::Api::Functions::FGame::GetHeightScale());

		DrawOutlinedDebugText(20 * Average4k::Api::Functions::FGame::GetWidthScale(), 64 * Average4k::Api::Functions::FGame::GetHeightScale(), "Please make sure at least one skin is named \"Default\" so the game doesn't show this message.", 32 * Average4k::Api::Functions::FGame::GetHeightScale());

		if (CurrentMenu != NULL)
			CurrentMenu->cameraDraw();
		return;
	}
	if (Average4k::Data::ChartFinder::pack_pool.get_tasks_queued() || Average4k::Data::ChartFinder::pack_pool.get_tasks_running() > 0 || Average4k::Data::ChartFinder::startedSearching)
	{

		DrawOutlinedDebugText(200 * Average4k::Api::Functions::FGame::GetWidthScale(), 100 * Average4k::Api::Functions::FGame::GetHeightScale(), "please hold... im loading packs", 84 * Average4k::Api::Functions::FGame::GetHeightScale());

		DrawOutlinedDebugText(200 * Average4k::Api::Functions::FGame::GetWidthScale(), 200 * Average4k::Api::Functions::FGame::GetHeightScale(), "Packs queued to be loaded: " + std::to_string(Average4k::Data::ChartFinder::pack_pool.get_tasks_queued()), 64 * Average4k::Api::Functions::FGame::GetHeightScale());

		DrawOutlinedDebugText(200 * Average4k::Api::Functions::FGame::GetWidthScale(), 300 * Average4k::Api::Functions::FGame::GetHeightScale(), "Packs currently loading: " + std::to_string(Average4k::Data::ChartFinder::pack_pool.get_tasks_running()), 64 * Average4k::Api::Functions::FGame::GetHeightScale());

		DrawOutlinedDebugText(200 * Average4k::Api::Functions::FGame::GetWidthScale(), 400 * Average4k::Api::Functions::FGame::GetHeightScale(), "Packs loaded: " + std::to_string(Average4k::Data::ChartFinder::Packs.size()), 64 * Average4k::Api::Functions::FGame::GetHeightScale());

		if (CurrentMenu != NULL)
			CurrentMenu->cameraDraw();
		return;
	}

	HandleGamepad();

	if (queuedEvents.size() != 0 && !switchOnFadeout)
	{
		std::vector<int> listtoDelete = {};
		for (AvgEngine::Events::Event& e : queuedEvents)
		{
			Event(e);
			if (e.type == AvgEngine::Events::EventType::Event_SwitchMenu)
			{
				if (eventMutex.try_lock())
				{
					queuedEvents.clear();
					eventMutex.unlock();
				}
				switchOnFadeout = true;
				return;
			}
			listtoDelete.push_back(e.id);
		}
		if (eventMutex.try_lock())
		{
			int index = 0;
			for (int toDelete : listtoDelete)
			{
				for (AvgEngine::Events::Event& e : queuedEvents)
				{
					if (e.id == toDelete)
					{
						queuedEvents.erase(queuedEvents.begin() + index);
						break;
					}
				}
			}
			eventMutex.unlock();
		}
	}

	HandlePackets();

	if (CurrentMenu == NULL)
		return;
	if (switchOnFadeout)
	{
		AvgEngine::Render::Rect r = AvgEngine::Render::Rect(0,0, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);

		fadeout -= 0.05;

		r.a = std::lerp(1.0f, 0.0f,fadeout);
		r.r = 0;
		r.g = 0;
		r.b = 0;

		AvgEngine::Base::Primitives::DrawRectangle(&CurrentMenu->camera, 999, r);

		if (fadeout < 0.01f)
		{
			switchOnFadeout = false;
			fadedOut = true;
			fadeout = 1.0f;
			fadeoutSong = false;
			Switch();
			return;
		}
	}
	if (fadedOut)
	{
		AvgEngine::Render::Rect r = AvgEngine::Render::Rect(0, 0, AvgEngine::Render::Display::width, AvgEngine::Render::Display::height);

		fadeout -= 0.05;


		r.a = std::lerp(0.0f, 1.0f, fadeout);
		r.r = 0;
		r.g = 0;
		r.b = 0;

		AvgEngine::Base::Primitives::DrawRectangle(&CurrentMenu->camera, 999, r);

		if (fadeout < 0.01f)
		{
			fadedOut = false;
			fadeout = 1.0f;
		}
	}

	debugTextOutlined->transform.a = 0.25f;
	debugTextOutlined->centerLines = true;

	DrawOutlinedDebugText(AvgEngine::Render::Display::width / 2,24, "A4k Alpha Build " + std::string(A_VERSION) + " | Not even close to being done.", 32 * Average4k::Api::Functions::FGame::GetHeightScale());

	debugTextOutlined->transform.a = 1;
	debugTextOutlined->centerLines = false;

	CurrentMenu->draw();


}

void Average4k::A4kGame::SubmitPacket(msgpack::v2::object obj, Multiplayer::PacketType type)
{
	std::lock_guard<std::mutex> lock(packetQueueMutex);

	packetQueue.push_back(QueuedPacket(obj, type));
}

void Average4k::A4kGame::HandlePackets()
{
	std::lock_guard<std::mutex> lock(packetQueueMutex);

	if (packetQueue.size() == 0)
		return;

	for (QueuedPacket p : packetQueue)
	{
		// nothing atm

	}

	packetQueue.clear();
}
