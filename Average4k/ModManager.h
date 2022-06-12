#pragma once
#include "includes.h"
#include <sol.hpp>
#include "Menu.h"
#include "Easing.h"
struct Mod {
	std::string luaFunction;
	std::string name;
};

struct AppliedMod {
	Mod mod;
	float tweenStart;
	float tweenLen;
	Easing::easingFunction tweenCurve;
};

class ModManager
{
public:

	std::unique_ptr<sol::state> lua;

	std::map<std::string, sol::function>* luaMap;

	std::map<int, float> storedCMod;
	std::map<int, vec2> storedPositions; // storedMods
	std::map<int, float> funkyCMod;
	std::map<int, vec2> funkyPositions; // activeMods
	std::map<std::string, std::map<int, float>> modCMod;
	std::map<std::string, std::map<int, vec2>> modPositions; // targetMods

	std::vector<Mod> mods;

	std::vector<AppliedMod> appliedMods;


	static float beat;
	static float time;
	static float bpm;

	ModManager()
	{
		// nada
	}

	ModManager(std::string modFile);

	static void initLuaFunctions();

	void storePositions();

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
