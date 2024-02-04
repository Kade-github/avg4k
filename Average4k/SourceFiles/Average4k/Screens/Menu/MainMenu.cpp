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

	lua->getState().set_function("getPacks", [this]() {
		lua->getState().collect_garbage();
		sol::table t = lua->getState().create_table();

		for (int i = 0; i < Average4k::Data::ChartFinder::Packs.size(); i++)
		{
			Average4k::Data::Pack& p = Average4k::Data::ChartFinder::Packs[i];

			sol::table pack = lua->getState().create_table();

			pack["name"] = p.name;
			pack["path"] = p.path;
			pack["banner"] = p.banner;
		}

		return t;
	});

	lua->getState().set_function("getCharts", [this](std::string packName) {
		lua->getState().collect_garbage();
		for (int i = 0; i < Average4k::Data::ChartFinder::Packs.size(); i++)
		{
			Average4k::Data::Pack& p = Average4k::Data::ChartFinder::Packs[i];

			if (p.name == packName)
			{
				sol::table charts = lua->getState().create_table();

				for (int j = 0; j < p.charts.size(); j++)
				{
					Average4k::Data::Chart::Providers::StepFile& c = p.charts[j];

					sol::table chart = lua->getState().create_table();

					chart["title"] = c.metadata.title;
					chart["titleTranslit"] = c.metadata.titleTranslit;
					chart["subtitle"] = c.metadata.subtitle;
					chart["subtitleTranslit"] = c.metadata.subtitleTranslit;
					chart["artist"] = c.metadata.artist;
					chart["artistTranslit"] = c.metadata.artistTranslit;
					chart["genre"] = c.metadata.genre;
					chart["credit"] = c.metadata.credit;
					chart["banner"] = c.metadata.banner;
					chart["background"] = c.metadata.background;
					chart["file"] = c.metadata.file;
					chart["offset"] = c.metadata.offset;
					chart["previewStart"] = c.metadata.previewStart;

					chart["type"] = c.metadata.type;

					chart["workshop"] = c.metadata.workshop;

					charts[j + 1] = chart;
				}

				return charts;
			}
		}

		return NULL;
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
	AvgEngine::Base::Menu::draw();

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
