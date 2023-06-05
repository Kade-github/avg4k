#include "MainMenu.h"
#include "StartScreen.h"
#include "Average4K.h"

using namespace AvgEngine;
Average4K* a4er = NULL;

void MainMenu::Reload()
{
	Average4K::Instance->SwitchMenu(new MainMenu(Average4K::skin->GetLua("main-menu")));
}

void MainMenu::load()
{
	a4er = static_cast<Average4K*>(Average4K::Instance);
	a4er->options.inGameplay = false;
	Average4k::Lua::LuaMenu::load();

	eManager->Subscribe(AvgEngine::Events::EventType::Event_Resize, [&](AvgEngine::Events::Event e) {
		Reload();
	}, true, true);
}

void MainMenu::draw()
{
	for (QueuedPacket& p : a4er->queuedPackets)
	{
		
	}

	Average4k::Lua::LuaMenu::draw();
}