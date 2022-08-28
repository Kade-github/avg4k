#pragma once
#include "includes.h"
#include <sol.hpp>
#include "Menu.h"
#include "Easing.h"

struct AppliedMod {
	std::string mod;
	float tweenStart;
	float tweenLen;
	float amount;
	float modStartAmount;
	bool done = false;
	Easing::easingFunction tweenCurve;
};

class ModManager
{
public:

	std::unique_ptr<sol::state> lua;

	std::map<std::string, sol::function>* luaMap;

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
