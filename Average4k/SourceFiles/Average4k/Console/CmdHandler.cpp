/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "CmdHandler.h"
#include "AvgEngine/Utils/StringTools.h"
#include "../Screens/Menu/MainMenu.h"
#include "../Data/Chart/AsyncChartLoader.h"
#include <AvgEngine/Game.h>
#include "../Data/Chart/ChartFinder.h"

Average4k::Console::CmdHandler* Average4k::Console::CmdHandler::instance = NULL;

inline constexpr auto operator ""_sh(const char* str, size_t len) {
	return AvgEngine::Debug::hash_djb2a(std::string_view{ str, len });
}

void Average4k::Console::CmdHandler::Handle(std::string cmd)
{
	std::string s = "";

	std::vector<std::string> spl = AvgEngine::Utils::StringTools::Split(cmd, " ");
	if (spl.size() == 0)
		s = cmd;
	else
		s = AvgEngine::Utils::StringTools::Split(cmd, " ")[0];

	Average4k::Screens::Menu::MainMenu* m = NULL;

	int total = 0;
	AvgEngine::Utils::StringTools::ToLower(s);
	switch (AvgEngine::Debug::hash_djb2a(s))
	{
	case "reload"_sh:
		if (Average4k::Screens::Menu::MainMenu::loaded)
		{
			m = static_cast<Average4k::Screens::Menu::MainMenu*>(AvgEngine::Game::Instance->CurrentMenu);
			AvgEngine::Game::Instance->SwitchMenu(new Average4k::Screens::Menu::MainMenu(m->lua->path));
			AvgEngine::Logging::writeLog("[Debug] [CmdHandler] [reload] Reloaded lua.");
		}
		else
			AvgEngine::Logging::writeLog("[Debug] [CmdHandler] [reload] MainMenu not loaded");
		break;
	case "packs"_sh:
		for (auto& p : Average4k::Data::ChartFinder::Packs)
		{
			AvgEngine::Logging::writeLog(AvgEngine::Utils::StringTools::Ws2s(p.name) + " - Charts: " + std::to_string(p.charts.size()));

			total += p.charts.size();
		}

		AvgEngine::Logging::writeLog("Total packs: " + std::to_string(Average4k::Data::ChartFinder::Packs.size()) + " Total charts: " + std::to_string(total));

		break;
	default:

		ConsoleCommandHandler::Handle(cmd);
		break;
	}
}
