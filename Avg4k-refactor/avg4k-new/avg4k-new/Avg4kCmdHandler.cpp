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
			m->Reload();
		}
		else
			Logging::writeLog("[Menu] [Error] Sorry, that menu isn't a lua menu and cannot be reloaded!");
		break;
	case "bind"_sh:
		if (spl.size() < 2)
		{
			Logging::writeLog("[Bind] Syntax: bind <key> <command>");
			return;
		}

		

		break;
	case "objects"_sh:
		m = static_cast<Average4k::Lua::LuaMenu*>(Game::Instance->CurrentMenu);
		if (m->luaMenu)
		{
			for (AvgEngine::Base::GameObject* o : m->GameObjects)
			{
				Logging::writeLog("[Menu] Object-" + std::to_string(o->id) + ", " + o->transform.toString() + ", Ratio: " + (o->transformRatio ? "yes" : "no") + ", Children: " + std::to_string(o->Children.size()));
				for (AvgEngine::Base::GameObject* ob : o->Children)
				{
					Logging::writeLog("		Child -> Object-" + std::to_string(ob->id) + ", " + ob->transform.toString() + ", Ratio: " + (ob->transformRatio ? "yes" : "no") + ", Children: " + std::to_string(ob->Children.size()) + ".");
				}
			}
		}
		else
			Logging::writeLog("[Menu] [Error] Sorry, that menu isn't a lua menu and the object list cannot be shown!");
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
