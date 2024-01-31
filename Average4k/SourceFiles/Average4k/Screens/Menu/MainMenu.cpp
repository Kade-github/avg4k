/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"
using namespace Average4k::Api;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	if (reset)
		lua->load(Average4k::A4kGame::gameInstance->skin.GetPath(path));
	else
		lua = new AvgLuaFile(Average4k::A4kGame::gameInstance->skin.GetPath(path));

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm(this);

	lua->getState().set("currentMenu", cm);

	lua->getState().set_function("switchTo", [this](std::string path) {
		createFile(path);
	});

	lua->create();
}

void Average4k::Screens::Menu::MainMenu::load()
{
	createFile("Scripts/MainMenu.lua", false);

	// setup event listeners

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		lua->getState()["keyPress"](e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e) {
		lua->getState()["keyRelease"](e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e) {
		lua->getState()["mousePress"](e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseRelease, [&](AvgEngine::Events::Event e) {
		lua->getState()["mouseRelease"](e.data);
	});

	eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseScroll, [&](AvgEngine::Events::Event e) {
		lua->getState()["mouseScroll"](e.data);
	});
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();

	double mx, my;

	AvgEngine::Game::Instance->GetMousePos(&mx, &my);

	int mouse[2] = {mx, my};

	lua->getState().set("mouse", mouse);
	
	if (lua->getState().get("draw") != sol::nil)
		lua->getState()["draw"]();
}
