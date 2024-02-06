/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"

#include "../../Data/Chart/ChartFinder.h"
#include "../../Data/Chart/AsyncChartLoader.h"
#include "../../Api/Stubs/LuaSprite.h"
#include "../../Api/Stubs/LuaSong.h"
#include "../../Api/Functions/FMenu.h"
#include "../../Api/Functions/FCharts.h"

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
	lua->getState().set_function("loadSong", Functions::FCharts::LoadAsyncTexture);
	lua->getState().set_function("clearAudio", Functions::FCharts::ClearAudio);
	lua->getState().set_function("getAsyncSong", Functions::FCharts::GetAsyncAudio);
	lua->getState().set_function("getAsyncTexture", Functions::FCharts::GetAsyncTexture);
	lua->getState().set_function("checkChart", Functions::FCharts::CheckChart);
	lua->getState().set_function("clearAsync", Functions::FCharts::ClearAsync);

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
}

void Average4k::Screens::Menu::MainMenu::load()
{
	loaded = true;
	createFile(path, false);
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();

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
