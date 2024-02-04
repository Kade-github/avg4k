/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"

#include "../../Data/Chart/ChartFinder.h"

using namespace Average4k::Api;

bool Average4k::Screens::Menu::MainMenu::loaded = false;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	lua = new AvgLuaFile(Average4k::A4kGame::gameInstance->skin.GetPath(path));
	lua->path = path;

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm(this);

	lua->getState().set("currentMenu", cm);

	m = lua->getState().create_named_table("mouse");

	lua->getState().set_function("switchTo", [this](std::string path) {
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu(path));
	});

	lua->getState().set_function("scanCharts", [this]() {
		Average4k::Data::ChartFinder::FindCharts("Charts");
	});

	lua->create();

	// setup event listeners

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::function>>("keyPress");
		
		if (f.has_value())
		{
			sol::function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::function>>("keyRelease");

		if (f.has_value())
		{
			sol::function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::function>>("mouseDown");

		if (f.has_value())
		{
			sol::function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseRelease, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::function>>("mouseRelease");

		if (f.has_value())
		{
			sol::function_result result = f.value()(e.data);

			if (!result.valid())
			{
				sol::error err = result;
				AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
			}
		}
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseScroll, [&](AvgEngine::Events::Event e) {
		sol::state& s = lua->getState();
		auto f = s.get<sol::optional<sol::function>>("mouseScroll");

		if (f.has_value())
		{
			sol::function_result result = f.value()(e.data);

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
	static std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();

	AvgEngine::Base::Menu::draw();

	static bool other = false;


	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// diff

	std::chrono::duration<double> diff = now - last;

	// 1 milisecond

	if (diff.count() > 0.01)
	{
		last = now;
		if (Average4k::Data::ChartFinder::Charts.size() > 0)
		{
			std::lock_guard<std::mutex> lock(Average4k::Data::ChartFinder::m_lock);
			for(Average4k::Data::ChartFile& c : Average4k::Data::ChartFinder::Charts)
			{
				sol::table l_c = lua->getState().create_table_with("path", c.path, 
					"title", c.metadata.title, 
					"artist", c.metadata.artist, 
					"subtitle", c.metadata.subtitle,
					"genre", c.metadata.genre,
					"background", c.metadata.background,
					"banner", c.metadata.banner,
					"audio", c.metadata.file,
					"previewStart", c.metadata.previewStart,
					"credit", c.metadata.credit,
					"offset", c.metadata.offset,
					"type", c.metadata.type);

				auto f = lua->getState().get<sol::optional<sol::function>>("chartFound");

				if (f.has_value())
				{
					sol::function_result result = f.value()(l_c);

					if (!result.valid())
					{
						sol::error err = result;
						AvgEngine::Logging::writeLog("[Lua] Error in function.\n" + std::string(err.what()));
					}
				}

			}

			Average4k::Data::ChartFinder::Charts.clear();
		}

	}


	double mx, my;

	AvgEngine::Game::Instance->GetMousePos(&mx, &my);

	int mouse[2] = {(int)mx, (int)my};

	m.set("x", mouse[0]);
	m.set("y", mouse[1]);

	auto f = lua->getState().get<sol::optional<sol::function>>("draw");
	if (f.has_value())
	{
		sol::function_result result = f.value()();

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] Error in function draw.\n" + std::string(err.what()));
		}
	}
}
