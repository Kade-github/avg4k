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

	// Create playfield player #1
	float noteSize = std::stof(Average4K::settings->Get("Note Size").value);
	float xPos = Average4K::Instance->CurrentMenu->displayRect.w / 2 - (noteSize * 4);
	float yPos = (Average4K::Instance->CurrentMenu->displayRect.h / 2) - (140 * Average4K::skin->upscale);
	Average4k::Objects::Gameplay::Playfield* playfield = new Average4k::Objects::Gameplay::Playfield(xPos, yPos);

	addObject(playfield);
}


void Gameplay::draw()
{
	for (QueuedPacket& p : Average4ker::a4er->queuedPackets)
	{

	}

	Average4k::Lua::GameplayMenu::draw();
}