#include "Avg4kCmdHandler.h"
#include "Game.h"
#include "Console.h"
#include "LuaMenu.h"

inline constexpr auto operator ""_sh(const char* str, size_t len) {
	return AvgEngine::Debug::hash_djb2a(std::string_view{ str, len });
}

void Avg4kCmdHandler::Handle(std::string cmd)
{
	using namespace AvgEngine;
	Debug::Console* c = Debug::Console::instance;
	Average4k::Lua::LuaMenu* m = NULL;

	std::vector<std::string> spl = Utils::StringTools::Split(cmd, " ");

	std::string s = "";

	if (spl.size() == 0)
		s = cmd;
	else
		s = Utils::StringTools::Split(cmd, " ")[0];
	Utils::StringTools::ToLower(s);
	switch (Debug::hash_djb2a(s))
	{
	case "reload"_sh:
		m = static_cast<Average4k::Lua::LuaMenu*>(Game::Instance->CurrentMenu);
		if (m->luaMenu)
		{
			m->file->Reload();
			Logging::writeLog("[Menu] Reloaded " + m->file->_path);
		}
		else
			Logging::writeLog("[Menu] Sorry, that menu isn't a lua menu and cannot be reloaded!");
		break;
#ifdef _DEBUG
	case "lua"_sh:
		m = static_cast<Average4k::Lua::LuaMenu*>(Game::Instance->CurrentMenu);
		if (m->luaMenu)
		{
			m->RunLua(cmd.substr(cmd.find_first_of(' ') + 1, cmd.size()));
		}
		else
			Logging::writeLog("[Menu] Sorry, the current menu has no lua capabilities!");
		break;
#endif
	default:
		ConsoleCommandHandler::Handle(cmd);
		break;
	}

}
