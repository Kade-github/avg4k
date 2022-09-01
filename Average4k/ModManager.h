#pragma once
#include "includes.h"
#include <sol.hpp>
#include "Menu.h"
#include "Easing.h"

struct AppliedMod {
	std::string mod;
	float tweenStart;
	float tweenLen;
	int col;
	float amount;
	bool started = false;
	float modStartAmount;
	float repeatEndBeat = -1;
	int tweenOffset = 0;
	std::string function = "-1";
	bool done = false;
	Easing::easingFunction tweenCurve;

	inline bool operator() (const AppliedMod& struct1, const AppliedMod& struct2)
	{
		return (struct1.tweenStart < struct2.tweenStart);
	}
};

class ModManager
{
public:
	static ModManager* instance;
	static bool doMods;
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

	void runMods(AppliedMod mod, float beat);

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
