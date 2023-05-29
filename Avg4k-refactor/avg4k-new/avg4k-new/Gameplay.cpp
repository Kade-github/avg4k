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
	float xPos = Average4K::Instance->CurrentMenu->displayRect.w / 2 - (((64 * noteSize) * 4) / 2);
	float yPos = (64 * noteSize);
	Average4k::Objects::Gameplay::Playfield* playfield = new Average4k::Objects::Gameplay::Playfield(xPos, yPos);

	addObject(playfield);

	using namespace Average4k::Audio;

	c = RhythmBASSHelper::GetChannel("menu");
	if (c == NULL)
	{
		Logging::writeLog("[Error] Rhythm Channel 'menu' doesn't exist! Did lua create it?");
		return;
	}
	c->Stop();

	startTimestamp = glfwGetTime();
	songStart = startTimestamp + std::stof(Average4K::settings->Get("Start Offset").value);

	// Key press subscribe
	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		for (Average4k::Objects::Gameplay::Playfield* p : playfields)
			p->keyPress(e.data);
	});
}


void Gameplay::draw()
{

	for (QueuedPacket& p : Average4ker::a4er->queuedPackets)
	{

	}


	if (c == NULL)
	{
		Average4k::Lua::GameplayMenu::draw();
		return;
	}

	// check for the start offset
	if (glfwGetTime() >= songStart && !c->isPlaying)
	{
		c->Play();
		Logging::writeLog("[Gameplay] Song started at " + std::to_string(songStart));
	}

	Average4k::Lua::GameplayMenu::draw();
}