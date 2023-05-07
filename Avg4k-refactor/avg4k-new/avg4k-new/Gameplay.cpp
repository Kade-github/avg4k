#include "Gameplay.h"
#include "Average4K.h"
using namespace AvgEngine;

namespace Average4ker
{
	Average4K* a4er = NULL;
}

void Gameplay::Reload()
{
	Logging::writeLog("[Gameplay] You cannot reload this menu!");
}

void Gameplay::load()
{
	Average4ker::a4er = static_cast<Average4K*>(Average4K::Instance);
	Average4k::Lua::GameplayMenu::load();


}


void Gameplay::draw()
{
	for (QueuedPacket& p : Average4ker::a4er->queuedPackets)
	{

	}

	Average4k::Lua::GameplayMenu::draw();
}