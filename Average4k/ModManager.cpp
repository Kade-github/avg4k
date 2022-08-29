#include "ModManager.h"
#include "Gameplay.h"
#include "Game.h"
#include "Average4k.h"
#include "ArrowEffects.h"
// usertype structs

struct configMod {
	bool downscroll;
	double noteSize;
};

struct gameMod {

};


std::map<std::string, std::function<float()>>* luaFunctions;

float ModManager::beat = 0;
float ModManager::time = 0;
float ModManager::bpm = 0;

ModManager::ModManager(std::string luaPath)
{
	lua = std::make_unique<sol::state>();

	lua->open_libraries(sol::lib::base);

	lua->open_libraries(sol::lib::math);

	createFunctions();

	auto bad_code_result = lua->safe_script_file(luaPath);

	if (bad_code_result.valid())
		std::cout << "Ran " << luaPath << " successfully!" << std::endl;
	else
	{
		sol::error error = bad_code_result;
		std::cout << "Failed to run " << luaPath << ". " << error.what() << std::endl;
	}

	luaMap = new std::map<std::string, sol::function>();

	populateLuaMap();
}

void ModManager::populateLuaMap()
{
	(*luaMap)["create"] = (*lua)["create"];
	(*luaMap)["update"] = (*lua)["update"];
	(*luaMap)["destroy"] = (*lua)["destroy"];
}

void ModManager::callEvent(std::string event, std::string args)
{
	(*luaMap)[event](args);
}

void ModManager::callEvent(std::string event, int args)
{
	(*luaMap)[event](args);
}

void ModManager::callEvent(std::string event, float args)
{
	(*luaMap)[event](args);
}

void ModManager::initLuaFunctions()
{
	if (luaFunctions)
		delete luaFunctions;
	luaFunctions = new std::map<std::string, std::function<float()>>();

	(*luaFunctions)["getBeat"] = [&] {
		// returns the current beat
		return beat;
	};

	(*luaFunctions)["getTime"] = [&] {
		// returns the current time
		return time;
	};

	(*luaFunctions)["getBPM"] = [&] {
		// returns the current bpm
		return time;
	};
}

void consolePrint(std::string print)
{
	Game::instance->db_addLine("[LUA] " + print);
}


void ModManager::runMods()
{
	for (AppliedMod& m : appliedMods)
	{
		if (m.done)
			continue;
		if (beat >= m.tweenStart)
		{
			float dur = beat - m.tweenStart;

			float perc = dur / m.tweenLen;

			float tween = m.tweenCurve(perc);

			//consolePrint(m.mod + " | " + std::to_string(tween) + " | " + std::to_string(perc * 100) + "%");

			if (m.mod == "drunk")
				ArrowEffects::drunk = std::lerp(m.modStartAmount, m.amount, tween);
			if (m.mod == "tipsy")
				ArrowEffects::tipsy = std::lerp(m.modStartAmount, m.amount, tween);
			if (m.mod == "reverse")
				ArrowEffects::reverse[m.col] = std::lerp(m.modStartAmount, m.amount, tween);

			if (perc >= 1)
			{
				m.done = true;
				continue;
			}
		}
	}
}


void ModManager::createFunctions()
{
	// simple helper functions
	for (std::map<std::string, std::function<float()>>::iterator iter = (*luaFunctions).begin(); iter != (*luaFunctions).end(); ++iter)
	{
		lua->set_function(iter->first, iter->second);
	}

	// user types

	lua->new_usertype<configMod>("config", "downscroll", &configMod::downscroll, "noteSize", &configMod::noteSize);

	configMod cm;
	cm.downscroll = Game::save->GetBool("downscroll");
	cm.noteSize = 64 * Game::save->GetDouble("Note Size");

	(*lua)["config"] = cm;

	// special functions

	lua->set_function("consolePrint", consolePrint);

	lua->set_function("activateMod", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount) {
		AppliedMod aMod;
		aMod.mod = name;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.amount = amount;
		if (name == "drunk")
			aMod.modStartAmount = ArrowEffects::drunk;
		if (name == "tipsy")
			aMod.modStartAmount = ArrowEffects::tipsy;

		Gameplay::instance->manager.appliedMods.push_back(aMod);

	});

	lua->set_function("activateModMap", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount, int col) {
		AppliedMod aMod;
		aMod.mod = name;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.col = col;
		aMod.amount = amount;
		if (name == "reverse")
			aMod.modStartAmount = ArrowEffects::reverse[col];

		Gameplay::instance->manager.appliedMods.push_back(aMod);

	});

}
