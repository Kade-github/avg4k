#include "Gameplay.h"
#include "Average4K.h"
using namespace AvgEngine;

Average4K* a4er = NULL;

void Gameplay::Reload()
{
	Logging::writeLog("[Gameplay] You cannot reload this menu!");
}

void Gameplay::load()
{
	a4er = static_cast<Average4K*>(Average4K::Instance);
	Average4k::Lua::LuaMenu::load();


}


void Gameplay::draw()
{
	for (QueuedPacket& p : a4er->queuedPackets)
	{

	}

	Average4k::Lua::LuaMenu::draw();
}