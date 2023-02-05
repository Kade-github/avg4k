#include "StartScreen.h"

StartScreen::StartScreen()
{
	
}

void KeyPress(AvgEngine::Events::Event e)
{
	AvgEngine::Logging::writeLog("Key Press " + std::to_string(e.data));
}

void StartScreen::load()
{
	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, KeyPress);
}