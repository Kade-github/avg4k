/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"

#include <AvgEngine/Utils/Paths.h>

#include "../../Data/Chart/ChartFinder.h"
#include "../../Data/Chart/AsyncChartLoader.h"

using namespace Average4k::Api;

bool Average4k::Screens::Menu::MainMenu::loaded = false;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	eManager->Clear(); // just in case
	Average4k::Data::AsyncChartLoader::ClearAll();
	lua = new AvgLuaFile(Average4k::A4kGame::gameInstance->skin.GetPath(path));
	lua->path = path;

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());
	Average4k::Api::Stubs::LuaSong::Register(lua->getState());

	cm = Average4k::Api::Stubs::LuaMenu(this);

	lua->getState().set("currentMenu", cm);

	Functions::FMenu::CurrentMenu = &cm;
	Functions::FCharts::Lua = lua;

	m = lua->getState().create_named_table("mouse");

	// FMenu

	lua->getState().set_function("switchTo", Functions::FMenu::SwitchTo);

	// FCharts

	lua->getState().set_function("scanCharts",Functions::FCharts::ScanCharts);
	lua->getState().set_function("getPacks", Functions::FCharts::GetPacks);
	lua->getState().set_function("getCharts", Functions::FCharts::GetCharts);
	lua->getState().set_function("loadChart", Functions::FCharts::LoadChart);
	lua->getState().set_function("loadAsyncTexture", Functions::FCharts::LoadAsyncTexture);
	lua->getState().set_function("loadSong", Functions::FCharts::LoadAsyncAudio);
	lua->getState().set_function("clearAudio", Functions::FCharts::ClearAudio);
	lua->getState().set_function("getAsyncSong", Functions::FCharts::GetAsyncAudio);
	lua->getState().set_function("getAsyncTexture", Functions::FCharts::GetAsyncTexture);
	lua->getState().set_function("checkChart", Functions::FCharts::CheckChart);
	lua->getState().set_function("clearAsync", Functions::FCharts::ClearAsync);

	// FData

	lua->getState().set_function("getGameplayData", Functions::FData::GetGameplayData);
	lua->getState().set_function("getSkinData", Functions::FData::GetSkinData);
	lua->getState().set_function("getKeybindData", Functions::FData::GetKeybindData);
	lua->getState().set_function("getVideoData", Functions::FData::GetVideoData);
	lua->getState().set_function("getAudioData", Functions::FData::GetAudioData);
	lua->getState().set_function("setGameplayData", Functions::FData::SetGameplayData);
	lua->getState().set_function("setSkinData", Functions::FData::SetSkinData);
	lua->getState().set_function("setKeybindData", Functions::FData::SetKeybindData);
	lua->getState().set_function("setVideoData", Functions::FData::SetVideoData);
	lua->getState().set_function("setAudioData", Functions::FData::SetAudioData);
	lua->getState().set_function("setSkin", Functions::FData::SetSkin);

	// FSkins

	lua->getState().set_function("getSkins", Functions::FSkins::GetSkins);

	// FGame

	lua->getState().set_function("quitGame", Functions::FGame::Quit);

	lua->create();

	// setup event listeners

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::protected_function>>("keyPress");
		
		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::protected_function>>("keyRelease");

		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::protected_function>>("mouseDown");

		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseRelease, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::protected_function>>("mouseRelease");

		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseScroll, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::protected_function>>("mouseScroll");

		if (f.has_value())
		{
			sol::protected_function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_ReloadFont, [&](AvgEngine::Events::Event e) {
		cm.removeAll();
		AvgEngine::Game::Instance->SwitchMenu(new Average4k::Screens::Menu::MainMenu("Scripts/MainMenu.lua"));
	});
}

void Average4k::Screens::Menu::MainMenu::load()
{
	loaded = true;

	if (!AvgEngine::Utils::Paths::pathExists(Average4k::A4kGame::gameInstance->skin.GetPath(path)))
	{
		errorOut = true;
		AvgEngine::Logging::writeLog("[MainMenu] [Error] " + path + " could not be found.");
		AvgEngine::Logging::writeLog("[MainMenu] [Error] This is either one of two issues:");
		AvgEngine::Logging::writeLog("[MainMenu] [Error] 1. The skin doesn't implement this file, and it should. If you are the skinner, please do this.");
		AvgEngine::Logging::writeLog("[MainMenu] [Error] 2. You deleted something important in a default (or current) skin, and you should re-download/verify your files.");
		AvgEngine::Logging::writeLog("[MainMenu] [Error] If you know what you are doing, add the file. Once done, press F3 (to open the console),");
		AvgEngine::Logging::writeLog("[MainMenu] [Error] then input the \"reload\" command to reload the skin.");
		return;
	}

	createFile(path, false);
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();

	if (errorOut)
	{
		A4kGame::gameInstance->DrawDebugText(20, 20, path + " could not be found.", 32);
		A4kGame::gameInstance->DrawDebugText(20, 64, "This is either one of two issues:", 32);
		A4kGame::gameInstance->DrawDebugText(20, 108, "1. The skin doesn't implement this file, and it should. If you are the skinner, please do this.", 32);
		A4kGame::gameInstance->DrawDebugText(20, 152, "2. You deleted something important in a default (or current) skin, and you should re-download/verify your files.", 32);

		A4kGame::gameInstance->DrawDebugText(20, 214, "If you know what you are doing, add the file. Once done, press F3 (to open the console),", 32);
		A4kGame::gameInstance->DrawDebugText(20, 258, "then input the \"reload\" command to reload the skin.", 32);
		return;
	}

	double mx, my;

	AvgEngine::Game::Instance->GetMousePos(&mx, &my);

	int mouse[2] = {(int)mx, (int)my};

	m.set("x", mouse[0]);
	m.set("y", mouse[1]);

	auto f = lua->getState().get<sol::optional<sol::protected_function>>("draw");
	if (f.has_value())
	{
		sol::protected_function_result result = f.value()();

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] Error in function draw.\n" + std::string(err.what()));
		}
	}
}
