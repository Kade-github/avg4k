#include "ModManager.h"
#include "Gameplay.h"
#include "Game.h"
#include "Average4k.h"
#include "ArrowEffects.h"

bool ModManager::doMods = false;
ModManager* ModManager::instance = NULL;

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
		Game::instance->db_addLine("Lua Error");
		Game::instance->db_addLine(error.what());
		return;
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
	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
}

void ModManager::callEvent(std::string event, int args)
{
	(*luaMap)[event](args);
	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
}

void ModManager::callEvent(std::string event, float args)
{
	(*luaMap)[event](args);
	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
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
		if (beat >= m.tweenStart && ((beat < m.tweenStart + m.tweenLen) || (beat < m.repeatEndBeat)))
		{
				if (!m.started)
				{
					m.started = true;
					if (m.mod == "drunk")
						m.modStartAmount = ArrowEffects::drunk;
					if (m.mod == "tipsy")
						m.modStartAmount = ArrowEffects::tipsy;
					if (m.mod == "dizzy")
						m.modStartAmount = ArrowEffects::dizzy;
					if (m.mod == "amovex")
						m.modStartAmount = ArrowEffects::amovex;
					if (m.mod == "amovey")
						m.modStartAmount = ArrowEffects::amovey;
					if (m.mod == "reverse")
						m.modStartAmount = ArrowEffects::reverse[m.col];
					if (m.mod == "movex")
						m.modStartAmount = ArrowEffects::movex[m.col];
					if (m.mod == "movey")
						m.modStartAmount = ArrowEffects::movey[m.col];
				}

				float dur = (beat - m.tweenStart);

				float perc = dur / m.tweenLen;

				float tween = m.tweenCurve(perc);

				if (m.mod == "drunk")
					ArrowEffects::drunk = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "tipsy")
					ArrowEffects::tipsy = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "dizzy")
					ArrowEffects::dizzy = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "reverse")
					ArrowEffects::reverse[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "movex")
					ArrowEffects::movex[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "movey")
					ArrowEffects::movey[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "amovex")
					ArrowEffects::amovex = std::lerp(m.modStartAmount, m.amount, tween);
				if (m.mod == "amovey")
					ArrowEffects::amovey = std::lerp(m.modStartAmount, m.amount, tween);
			}

	}
}

void ModManager::runMods(AppliedMod m, float beat)
{
		if (m.mod == "drunk")
			m.modStartAmount = ArrowEffects::drunk;
		if (m.mod == "tipsy")
			m.modStartAmount = ArrowEffects::tipsy;
		if (m.mod == "dizzy")
			m.modStartAmount = ArrowEffects::dizzy;
		if (m.mod == "reverse")
			m.modStartAmount = ArrowEffects::reverse[m.col];
		if (m.mod == "movex")
			m.modStartAmount = ArrowEffects::movex[m.col];
		if (m.mod == "movey")
			m.modStartAmount = ArrowEffects::movey[m.col];
		if (m.mod == "amovex")
			m.modStartAmount = ArrowEffects::amovex;
		if (m.mod == "amovey")
			m.modStartAmount = ArrowEffects::amovey;

		float dur = (beat - m.tweenStart);

		float realDur = dur - m.tweenOffset;

		if (realDur > 1 && m.repeatEndBeat != -1)
		{
			m.tweenOffset = dur;
		}

		float perc = realDur / m.tweenLen;

		float tween = m.tweenCurve(perc);

		if (m.mod == "drunk")
			ArrowEffects::drunk = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "tipsy")
			ArrowEffects::tipsy = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "dizzy")
			ArrowEffects::dizzy = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "reverse")
			ArrowEffects::reverse[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "movex")
			ArrowEffects::movex[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "movey")
			ArrowEffects::movey[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "amovex")
			ArrowEffects::amovex = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "amovey")
			ArrowEffects::amovey = std::lerp(m.modStartAmount, m.amount, tween);
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
		aMod.modStartAmount = -999;

		instance->appliedMods.push_back(aMod);
		consolePrint(name + " | Mod applied! " + std::to_string(instance->appliedMods.size()));
	});

	lua->set_function("activateModMap", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount, int col) {
		AppliedMod aMod;
		aMod.mod = name;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.col = col;
		aMod.amount = amount;
		aMod.modStartAmount = -999;


		instance->appliedMods.push_back(aMod);
		consolePrint(name + " | Mod applied! " + std::to_string(instance->appliedMods.size()));
	});

}
