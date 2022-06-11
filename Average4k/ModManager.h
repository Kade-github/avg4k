#pragma once
#include "includes.h"
#include <sol.hpp>
#include "Menu.h"
struct Mod {
	std::string luaFunction;
	std::string name;
};

struct AppliedMod {
	Mod mod;
	float beat;
	float length;
	bool custom = false;
};

class ModManager
{
public:

	std::unique_ptr<sol::state> lua;

	std::map<std::string, sol::function>* luaMap;

	std::map<int, vec2> funkyPositions;

	std::vector<Mod> mods;

	std::vector<AppliedMod> appliedMods;


	static float beat;
	static float time;

	ModManager()
	{
		// nada
	}

	ModManager(std::string modFile);

	static void initLuaFunctions();

	void runMods();

	void createFunctions();
	void populateLuaMap();
	void callEvent(std::string event, std::string args);
	void callEvent(std::string event, int args);
	void callEvent(std::string event, float args);
	void destroy()
	{
		delete luaMap;
		lua.reset();
	}
};
