#include "ConsoleCommandHandler.h"
#include "Game.h"
#include "Console.h"
inline constexpr auto operator ""_sh(const char* str, size_t len) {
	return AvgEngine::Debug::hash_djb2a(std::string_view{ str, len });
}

void AvgEngine::Debug::ConsoleCommandHandler::Handle(std::string cmd)
{
	AvgEngine::Debug::Console* c = AvgEngine::Debug::Console::instance;
	std::string fullString = "";
	std::string s = cmd;
	Utils::StringTools::ToLower(s);
	switch (hash_djb2a(cmd))
	{
	case "help"_sh:
		Logging::writeLog("[Help] Commands:");
		for(int i = 0; i < commands.size(); i++)
		{
			std::string s = commands[i];
			fullString += s;
			if (i != commands.size() - 1)
				fullString += ", ";
		}
		Logging::writeLog("[Help] " + fullString);
		break;
	case "game"_sh:
		Logging::writeLog("[Game] " + Game::Instance->Title + ":" + Game::Instance->Version);
		break;
	case "performgraph"_sh:
		c->showPerformance = !c->showPerformance;
		break;
	default:
		Logging::writeLog("[Error] That command doesn't exist!");
		break;
	}
}