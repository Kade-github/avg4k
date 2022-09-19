#pragma once
#include "includes.h"
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include "Menu.h"
#include "Easing.h"
#include "AvgSprite.h"

struct FunctionMod {
	sol::function toCall;
	float beat;
	bool hit = false;
};

struct SpriteMod {
	AvgSprite* spr;
	Object* def;
	
	std::string anchor;

	float offsetX = 0;
	float offsetY = 0;

	std::string finish;

	bool isPlayField = false;

	float stealth = 0;

	bool notModCreated = false;

	float movex;
	float movey;
	float confusion;
};

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
	std::string spriteName = "-1";
	std::string shader;
	std::string param;
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
	bool killed = false;
	static ModManager* instance;

	AvgGroup* modGame;

	static bool doMods;
	std::unique_ptr<sol::state> lua;

	std::map<std::string, sol::function>* luaMap;

	std::map<std::string, SpriteMod> sprites;

	std::map<std::string, Shader*> shaders;

	std::vector<FunctionMod> funcMod;

	std::vector<AppliedMod> appliedMods;


	std::string assetPath;

	AvgGroup* spriteCamera;

	AvgCamera* cam;

	static float beat;
	static float time;
	static float bpm;

	ModManager()
	{
		// nada
	}

	ModManager(std::string modFile);

	static void initLuaFunctions();
	void kill();

	void runMods();

	void setModStart(AppliedMod& m);
	void setModProperties(AppliedMod& m, float tween);

	void runMods(AppliedMod mod, float beat);

	void createFunctions();
	void populateLuaMap();
	void callEvent(std::string event, std::string args);
	void callEvent(std::string event, int args);
	void callEvent(std::string event, float args);

	void destroy()
	{
		delete luaMap;
	};
};
