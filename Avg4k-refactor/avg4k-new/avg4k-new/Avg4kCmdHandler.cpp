#include "Avg4kCmdHandler.h"
#include "Game.h"
#include "Console.h"
#include "LuaMenu.h"
#include "MainMenu.h"
#include "Average4K.h"
#include "GameplayMenu.h"

Avg4kCmdHandler* Avg4kCmdHandler::instance = NULL;

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
		if (!ob->drawn)
			continue;
		_totalObjects++;
		auto t = ob->transform;
		if (real)
			t = ob->iTransform;
		Logging::writeLog(tabs + "Child -> Object-" + std::to_string(ob->id) + " Tag: " + ob->tag + ", z: " + std::to_string(ob->zIndex + tree->zIndex) + " (original: " + std::to_string(ob->zIndex) + "), " + t.toString() + ", Ratio: " + (ob->transformRatio ? "yes" : "no") + ", Children : " + std::to_string(ob->Children.size()) + ".");
		if (ob->Children.size() > 0) // recursive call
			printTree(ob, real, amount + 1);
	}
}



bool add_bind(Average4k::External::ConfigValue v)
{
	Avg4kCmdHandler* ins = Avg4kCmdHandler::instance;
	// take the reference of the event manager.
	AvgEngine::Events::EventManager* eManager = &Average4K::Instance->eManager;
	// find all the bind info
	std::string sp = AvgEngine::Utils::StringTools::Split(v.name, "_")[1];
	Bind b = Bind();
	b.bind_name = v.name;
	// set it into uppercase
	AvgEngine::Utils::StringTools::ToUpper(sp);
	// convert it to a keycode using std magic
	b.key = AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(sp);
	b.command = v.value;
	if (b.key == GLFW_KEY_UNKNOWN) // shit is unknown, and the user sucks!!
	{
		AvgEngine::Logging::writeLog("[Console] [Warning] Unknown key '" + sp + "' for bind '" + v.name + "'!");
		return false;
	}
	ins->binds.push_back(b);

	// get the reference of the recently pushed bind.
	Bind* bRef = &ins->binds.back();
	// tell the bind what event id it is (so we can remove it later), than subscribe to the event with the bind's reference. Also let us reference "this"
	bRef->eventId = eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [bRef, ins](AvgEngine::Events::Event e) {
		if (e.data == bRef->key)
			ins->Handle(bRef->command);
		}, false);
	return true;
}

void Avg4kCmdHandler::load_binds()
{
	// take the reference of the event manager.
	AvgEngine::Events::EventManager* eManager = &Average4K::Instance->eManager;
	// Remove all current binds
	for (Bind& b : binds)
	{
		// remove the id
		if (b.eventId != -1) // (if it isn't -1)
			eManager->RemoveById(b.eventId);
	}
	// clear the vector!!
	binds.clear();

	// Read the misc config
	for (Average4k::External::ConfigValue& v : Average4K::cfg->reader->values)
	{
		if (v.name.starts_with("bind")) // if it starts with bind, it's a bind (hopefully)
		{
			// add le bind
			add_bind(v);
		}
	}
}

void Avg4kCmdHandler::Handle(std::string cmd)
{
	using namespace AvgEngine;
	Debug::Console* c = Debug::Console::instance;
	Average4k::Lua::LuaMenu* m = NULL;
	Average4K* a = NULL;
	Average4k::Lua::GameplayMenu* gm = NULL;
	std::vector<std::string> spl = Utils::StringTools::Split(cmd, " ");

	a = static_cast<Average4K*>(Average4K::Instance);

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
			Logging::writeLog("[Bind] Syntax: bind <key> <command> - Create a bind");
			Logging::writeLog("[Bind] Syntax: bind remove <key> - Remove a bind");
			Logging::writeLog("[Bind] Syntax: bind list - List all binds");
			return;
		}
		if (spl[1] == "remove")
		{
			if (spl.size() < 3)
			{
				Logging::writeLog("[Bind] Syntax: bind remove <key> - Remove a bind");
				return;
			}
			std::string key = spl[2];
			Utils::StringTools::ToUpper(key);
			int i = 0;
			// take the reference of the event manager.
			AvgEngine::Events::EventManager* eManager = &Average4K::Instance->eManager;
			int keyy = Utils::StringTools::convertKeyNameToKeyCode(key);
			if (keyy == GLFW_KEY_UNKNOWN)
			{
				Logging::writeLog("[Bind] Unknown key '" + key + "'!");
				return;
			}
			for (Bind& b : binds)
			{
				if (b.key == keyy)
				{
					// remove the id
					if (b.eventId != -1) // (if it isn't -1)
						eManager->RemoveById(b.eventId);
					// remove the bind
					binds.erase(binds.begin() + i);
					Average4K::cfg->removeValue(b.bind_name);
					Logging::writeLog("[Bind] Removed bind '" + b.bind_name + "'!");
					return;
				}
				i++;
			}
			Logging::writeLog("[Bind] No bind found with key '" + key + "'!");
			return;
		}
		else if (spl[1] == "list")
		{
			if (binds.size() == 0)
			{
				Logging::writeLog("[Bind] No custom binds.");
				return;
			}
			Logging::writeLog("[Bind] Listing all binds:");
			for (Bind& b : binds)
			{
				Logging::writeLog("[Bind] " + b.bind_name + " -> " + b.command);
			}
			return;
		}
		else
		{
			if (spl.size() < 3)
			{
				Logging::writeLog("[Bind] Syntax: bind <key> <command> - Create a bind");
				return;
			}
			std::string key = spl[1];

			Utils::StringTools::ToUpper(key);
			int keyy = Utils::StringTools::convertKeyNameToKeyCode(key);
			if (keyy == GLFW_KEY_UNKNOWN)
			{
				Logging::writeLog("[Bind] Unknown key '" + key + "'!");
				return;
			}

			// check if the key is already bound
			for (Bind& b : binds)
			{
				if (b.key == keyy)
				{
					Logging::writeLog("[Bind] Key '" + key + "' is already bound to '" + b.bind_name + "'!");
					return;
				}
			}

			// get everything after the key
			std::string command = cmd.substr(cmd.find(spl[1]) + key.length() + 1);

			// add the bind
			Average4k::External::ConfigValue v;
			v.name = "bind_" + key;
			v.value = command;
			add_bind(v);
			Average4K::cfg->setValue(v.name, v.value);
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
				if (!o->drawn)
					continue;
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
	case "hitboxes"_sh:
		a->hitboxes = !a->hitboxes;
		break;
	default:
		ConsoleCommandHandler::Handle(cmd);
		break;
	}

}
