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
	switch (Debug::hash_djb2a(cmd))
	{
	case "reload"_sh:
		Average4k::Lua::LuaMenu* m = static_cast<Average4k::Lua::LuaMenu*>(Game::Instance->CurrentMenu);
		if (m->luaMenu)
		{
			m->file.Reload();
			Logging::writeLog("[Menu] Reloaded " + m->file._path);
		}
		else
			Logging::writeLog("[Menu] Sorry, that menu isn't a lua menu and cannot be reloaded!");
		break;
	}

	ConsoleCommandHandler::Handle(cmd);
}
