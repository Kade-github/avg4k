/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"
using namespace Average4k::Api;

bool Average4k::Screens::Menu::MainMenu::loaded = false;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	eManager->Clear();
	if (reset)
	{
		lua->objects.clear();
		removeAll();
		lua->path = path;
		lua->load(Average4k::A4kGame::gameInstance->skin.GetPath(path));
	}
	else
	{
		lua = new AvgLuaFile(Average4k::A4kGame::gameInstance->skin.GetPath(path));
		lua->path = path;
	}

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm(this);

	lua->getState().set("currentMenu", cm);

	lua->getState().set_function("switchTo", [this](std::string path) {
		createFile(path);
	});

	lua->create();

	// setup event listeners

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		auto f = lua->getState().get<sol::optional<sol::function>>("keyPress");
		
		if (f.has_value())
			f.value()(e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		auto f = lua->getState().get<sol::optional<sol::function>>("keyRelease");

		if (f.has_value())
			f.value()(e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e) {
		auto f = lua->getState().get<sol::optional<sol::function>>("mouseDown");

		if (f.has_value())
			f.value()(e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseRelease, [&](AvgEngine::Events::Event e) {
		auto f = lua->getState().get<sol::optional<sol::function>>("mouseRelease");

		if (f.has_value())
			f.value()(e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseScroll, [&](AvgEngine::Events::Event e) {
		auto f = lua->getState().get<sol::optional<sol::function>>("mouseScroll");

		if (f.has_value())
			f.value()(e.data);
	});
}

void Average4k::Screens::Menu::MainMenu::load()
{
	loaded = true;
	createFile("Scripts/MainMenu.lua", false);

	
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();

	double mx, my;

	AvgEngine::Game::Instance->GetMousePos(&mx, &my);

	int mouse[2] = {(int)mx, (int)my};

	sol::table m = lua->getState().create_named_table("mouse");

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
