/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"

#include "../../Data/Chart/ChartFinder.h"
#include "../../Data/Chart/AsyncChartLoader.h"
#include "../../Api/Stubs/LuaSprite.h"

using namespace Average4k::Api;

bool Average4k::Screens::Menu::MainMenu::loaded = false;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	eManager->Clear(); // just in case
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

			t[i + 1] = pack;
		}

		return t;
	});

	lua->getState().set_function("getCharts", [this](std::wstring packName) {
		lua->getState().collect_garbage();
		for (int i = 0; i < Average4k::Data::ChartFinder::Packs.size(); i++)
		{
			Average4k::Data::Pack& p = Average4k::Data::ChartFinder::Packs[i];

			if (p.name == packName)
			{
				sol::table charts = lua->getState().create_table();

				std::wstring_convert<std::codecvt_utf8<char8_t>, char8_t> convert;

				for (int j = 0; j < p.charts.size(); j++)
				{
					Average4k::Data::Chart::Providers::StepFile c = p.charts[j];

					sol::table chart = lua->getState().create_table();



					chart["title"] = convert.from_bytes(AvgEngine::Utils::StringTools::Ws2s(c.metadata.title));
					chart["titleTranslit"] = c.metadata.titleTranslit;
					chart["subtitle"] = convert.from_bytes(AvgEngine::Utils::StringTools::Ws2s(c.metadata.subtitle));
					chart["subtitleTranslit"] = c.metadata.subtitleTranslit;
					chart["artist"] = convert.from_bytes(AvgEngine::Utils::StringTools::Ws2s(c.metadata.artist));
					chart["artistTranslit"] = c.metadata.artistTranslit;
					chart["genre"] = c.metadata.genre;
					chart["credit"]= convert.from_bytes(AvgEngine::Utils::StringTools::Ws2s(c.metadata.credit));
					chart["banner"] = c.metadata.banner;
					chart["background"] = c.metadata.background;
					chart["file"] = c.metadata.file;
					chart["offset"] = c.metadata.offset;
					chart["previewStart"] = c.metadata.previewStart;

					chart["type"] = c.metadata.type;

					chart["workshop"] = c.metadata.workshop;
					// get folder from path
					chart["folder"] = c.path.substr(0, c.path.find_last_of(L"\\"));

					charts[j + 1] = chart;
				}

				return charts;
			}
		}

		return lua->getState().create_table();
	});

	lua->getState().set_function("loadChart", [this](std::wstring path) {
		Average4k::Data::AsyncChartLoader::AsyncLoadChart(path);
	});

	lua->getState().set_function("loadAsyncTexture", [this](std::wstring path) {
		return Average4k::Data::AsyncChartLoader::AsyncLoadTexture(path);
	});

	lua->getState().set_function("getAsyncTexture", [this](int id) {
		Average4k::Data::asyncImage img = Average4k::Data::AsyncChartLoader::checkTexture(id);

		if (img.data == nullptr)
			return Average4k::Api::Stubs::LuaSprite();

		lua->getState().collect_garbage();

		Average4k::Api::Stubs::LuaSprite spr = Average4k::Api::Stubs::LuaSprite(new AvgEngine::Base::Sprite(-100,-100,img.data, img.width, img.height));

		return spr;
	});


	lua->getState().set_function("checkChart", [this]() {
		Average4k::Data::ChartFile chart = Average4k::Data::AsyncChartLoader::CheckChart();

		lua->getState().collect_garbage();

		sol::table t = lua->getState().create_table();

		if (chart.metadata.title == L"")
		{
			t["title"] = "";
			return t;
		}

		t["title"] = chart.metadata.title;
		t["titleTranslit"] = chart.metadata.titleTranslit;
		t["subtitle"] = chart.metadata.subtitle;
		t["subtitleTranslit"] = chart.metadata.subtitleTranslit;
		t["artist"] = chart.metadata.artist;
		t["artistTranslit"] = chart.metadata.artistTranslit;
		t["genre"] = chart.metadata.genre;
		t["credit"] = chart.metadata.credit;
		t["banner"] = chart.metadata.banner;
		t["background"] = chart.metadata.background;
		t["file"] = chart.metadata.file;
		t["offset"] = chart.metadata.offset;
		t["previewStart"] = chart.metadata.previewStart;

		t["type"] = chart.metadata.type;

		t["workshop"] = chart.metadata.workshop;

		t["timingPoints"] = lua->getState().create_table();

		for (int i = 0; i < chart.timingPoints.size(); i++)
		{
			Average4k::Data::Chart::TimingPoint& tp = chart.timingPoints[i];

			sol::table timingPoint = lua->getState().create_table();

			timingPoint["startBeat"] = tp.startBeat;
			timingPoint["endBeat"] = tp.endBeat;
			timingPoint["bpm"] = tp.bpm;

			timingPoint["startTimestamp"] = tp.startTimestamp;
			timingPoint["endTimestamp"] = tp.endTimestamp;
			timingPoint["length"] = tp.length;

			t["timingPoints"][i + 1] = timingPoint;
		}

		t["stopPoints"] = lua->getState().create_table();

		for (int i = 0; i < chart.stopPoints.size(); i++)
		{
			Average4k::Data::Chart::StopPoint& sp = chart.stopPoints[i];

			sol::table stopPoint = lua->getState().create_table();

			stopPoint["startBeat"] = sp.startBeat;
			stopPoint["startTimestamp"] = sp.startTimestamp;
			stopPoint["length"] = sp.length;


			t["stopPoints"][i + 1] = stopPoint;
		}

		t["difficulties"] = lua->getState().create_table();

		for (int i = 0; i < chart.difficulties.size(); i++)
		{
			Average4k::Data::Chart::Difficulty& d = chart.difficulties[i];

			sol::table difficulty = lua->getState().create_table();

			difficulty["name"] = d.name;
			difficulty["charter"] = d.charter;
			difficulty["rating"] = d.rating;

			difficulty["notes"] = lua->getState().create_table();

			for (int j = 0; j < d.notes.size(); j++)
			{
				Average4k::Data::Chart::Note& n = d.notes[j];

				sol::table note = lua->getState().create_table();

				note["beat"] = n.beat;
				note["lane"] = n.lane;
				note["type"] = n.type;

				difficulty["notes"][j + 1] = note;
			}
			

			t["difficulties"][i + 1] = difficulty;
		}

		return t;
	});

	lua->getState().set_function("clearAsync", [this]() {
		Average4k::Data::AsyncChartLoader::ClearAll();
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
