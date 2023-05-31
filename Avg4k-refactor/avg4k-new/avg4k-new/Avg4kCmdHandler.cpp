#include "Avg4kCmdHandler.h"
#include "Game.h"
#include "Console.h"
#include "LuaMenu.h"
#include "MainMenu.h"
#include "Average4K.h"
#include "GameplayMenu.h"
inline constexpr auto operator ""_sh(const char* str, size_t len) {
	return AvgEngine::Debug::hash_djb2a(std::string_view{ str, len });
}

int _totalObjects = 0;

void printTree(AvgEngine::Base::GameObject* tree, bool real, int amount = 0)
{
	using namespace AvgEngine;
	std::string tabs = "";
	for (int i = 0; i < amount; i++)
		tabs += "	";
	for (AvgEngine::Base::GameObject* ob : tree->Children)
	{
		_totalObjects++;
		auto t = ob->transform;
		if (real)
			t = ob->iTransform;
		Logging::writeLog(tabs + "Child -> Object-" + std::to_string(ob->id) + " Tag: " + ob->tag + ", z: " + std::to_string(ob->zIndex + tree->zIndex) + " (original: " + std::to_string(ob->zIndex) + "), " + t.toString() + ", Ratio: " + (ob->transformRatio ? "yes" : "no") + ", Children : " + std::to_string(ob->Children.size()) + ".");
		if (ob->Children.size() > 0) // recursive call
			printTree(ob, real, amount + 1);
	}
}

void Avg4kCmdHandler::Handle(std::string cmd)
{
	using namespace AvgEngine;
	Debug::Console* c = Debug::Console::instance;
	Average4k::Lua::LuaMenu* m = NULL;
	Average4k::Lua::GameplayMenu* gm = NULL;
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
			Logging::consoleLog.clear();
			m->Reload();
		}
		else
			Logging::writeLog("[Menu] [Error] Sorry, that menu isn't a lua menu and cannot be reloaded!");
		break;
	case "dontcare"_sh:
		Average4K::Instance->SwitchMenu(new MainMenu(Average4K::skin->GetLua("main-menu")));
		Logging::writeLog("[DontCare] This command will be removed in the future. It is only for use when online capabilities aren't avaliable.");
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
			_totalObjects = 0;
			bool real = AvgEngine::Utils::StringTools::Contains(cmd, " -real");
			for (AvgEngine::Base::GameObject* o : m->GameObjects)
			{
				auto t = o->transform;
				if (real)
					t = o->iTransform;
				Logging::writeLog("[Menu] Object-" + std::to_string(o->id) + " Tag: " + o->tag + ", z: " + std::to_string(o->zIndex) + ", " + t.toString() + ", Ratio : " + (o->transformRatio ? "yes" : "no") + ", Children : " + std::to_string(o->Children.size()));
				AvgEngine::Base::GameObject* tree = o;
				if (o->Children.size() > 0)
					printTree(tree, real, 1);
				_totalObjects++;
			}
			Logging::writeLog("[Menu] Total: " + std::to_string(_totalObjects));
		}
		else
			Logging::writeLog("[Menu] [Error] Sorry, that menu isn't a lua menu and the object list cannot be shown!");
		break;
	case "lua"_sh:
		if (static_cast<Average4K*>(Average4K::Instance)->options.inGameplay)
		{
			gm = static_cast<Average4k::Lua::GameplayMenu*>(Game::Instance->CurrentMenu);
			if (gm->luaMenu)
			{
				gm->RunLua(cmd.substr(cmd.find_first_of(' ') + 1, cmd.size()));
			}
			else
				Logging::writeLog("[Menu] Sorry, the current menu has no lua capabilities!");
			return;
		}
		m = static_cast<Average4k::Lua::LuaMenu*>(Game::Instance->CurrentMenu);
		if (m->luaMenu)
		{
			m->RunLua(cmd.substr(cmd.find_first_of(' ') + 1, cmd.size()));
		}
		else
			Logging::writeLog("[Menu] Sorry, the current menu has no lua capabilities!");
		break;
	case "basserror"_sh:
		Logging::writeLog("[BASS] Error: " + std::to_string(BASS_ErrorGetCode()));
		break;
	default:
		ConsoleCommandHandler::Handle(cmd);
		break;
	}

}
