#include "ModManager.h"
#include "Gameplay.h"
#include "Game.h"
#include "Average4k.h"
// usertype structs

// first 4 id's of funkyPos are receptors

struct receptorMod {
	int col;

	float getX() {
		return Gameplay::instance->manager.funkyPositions[col].x;
	}

	float getY() {
		return Gameplay::instance->manager.funkyPositions[col].y;
	}

	void setX(float x) {
		Gameplay::instance->manager.funkyPositions[col].x = x;
	}

	void setY(float y) {
		Gameplay::instance->manager.funkyPositions[col].y = y;
	}
};

struct noteMod {
	float beat;
	int col;
	int id;

	float getX() const {
		return Gameplay::instance->manager.funkyPositions[id].x;
	}

	float getY() const {
		return Gameplay::instance->manager.funkyPositions[id].y;
	}

	float getScaleX() const {

	}

	float getScaleY() const {

	}

	void setX(float x) {
		Gameplay::instance->manager.funkyPositions[id].x = x;
	}

	void setY(float y) {
		Gameplay::instance->manager.funkyPositions[id].y = y;
	}

	void scaleX(float sx) {

	}

	void scaleY(float sy) {

	}


};


std::map<std::string, std::function<float()>>* luaFunctions;

float ModManager::beat = 0;
float ModManager::time = 0;

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
		std::cout << "Failed to run " << luaPath << ". " << std::endl;

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
}

void ModManager::runMods()
{
	for (AppliedMod m : appliedMods)
	{
		if (beat >= m.beat && m.beat + m.length > beat && !m.custom)
		{
			for (NoteObject* obj : Gameplay::instance->spawnedNotes)
			{
				noteMod nm;
				nm.id = obj->modId;
				nm.beat = obj->beat;
				nm.col = obj->lane;
				
				(*lua)["note"] = nm;

				sol::function_result res = (*lua)[m.mod.luaFunction]((*lua)["note"], nm);
				if (!res.valid())
				{
					sol::error err = res;
					std::cout << "Lua Error " << err.what() << std::endl;
					Game::instance->db_addLine("[LUA] Error on " + m.mod.name + ". Check log.txt for more information.");
					Average4k::dumpOutstream();
				}
			}
		}

		if (m.custom)
		{
			if (beat >= m.beat && m.beat + m.length > beat)
			{
				sol::function_result res = (*lua)[m.mod.luaFunction]();
				if (!res.valid())
				{
					sol::error err = res;
					std::cout << "Lua Error " << err.what() << std::endl;
					Game::instance->db_addLine("[LUA] Error on " + m.mod.name + ". Check log.txt for more information.");
					Average4k::dumpOutstream();
				}
			}
		}
	}
}


void consolePrint(std::string print)
{
	Game::instance->db_addLine("[LUA] " + print);
}

noteMod constructNoteMod(sol::table params)
{
	noteMod m;
	m.beat = params["beat"].get<float>();
	m.col = params["col"].get<int>();
	return m;
}

void ModManager::createFunctions()
{
	// simple helper functions
	for (std::map<std::string, std::function<float()>>::iterator iter = (*luaFunctions).begin(); iter != (*luaFunctions).end(); ++iter)
	{
		lua->set_function(iter->first, iter->second);
	}

	// user types

	lua->new_usertype<receptorMod>("receptor", "col", &receptorMod::col, "getX", &receptorMod::getX, "setX", &receptorMod::setX, "getY", &receptorMod::getY, "setY", &receptorMod::setY);

	auto note = lua->new_usertype<noteMod>("note", "beat", &noteMod::beat, "col", &noteMod::col);

	note["getX"] = &noteMod::getX;
	note["getY"] = &noteMod::getY;
	note["setX"] = &noteMod::setX;
	note["setY"] = &noteMod::getY;

	// special functions

	lua->set_function("consolePrint", consolePrint);

	lua->set_function("getReceptor", [&](int col) {
		
		receptorMod r;

		r.col = col;
		return r;
		
	});

	lua->set_function("createMod", [](std::string modFunction, std::string name) {
		Mod m;
		m.luaFunction = modFunction;
		m.name = name;
		Gameplay::instance->manager.mods.push_back(m);
	});

	lua->set_function("applyNoteMod", [](std::string name, float beat, float beatLength) {
		bool foundMod = false;
		Mod mod;
		for(Mod& m : Gameplay::instance->manager.mods)
			if (m.name == name)
			{
				mod = m;
				foundMod = true;
			}
		if (!foundMod)
		{
			Game::instance->db_addLine("[LUA] Error on applyNoteMod(" + name + "," + std::to_string(beat) + "," + std::to_string(beatLength) + "). Mod not found");
			return;
		}

		AppliedMod aMod;
		aMod.mod = mod;
		aMod.beat = beat;
		aMod.length = beatLength;
		aMod.custom = false;
		Gameplay::instance->manager.appliedMods.push_back(aMod);
	});

	lua->set_function("applyCustomMod", [](std::string name, float beat, float beatLength) {
		bool foundMod = false;
		Mod mod;
		for (Mod& m : Gameplay::instance->manager.mods)
			if (m.name == name)
			{
				mod = m;
				foundMod = true;
			}
		if (!foundMod)
		{
			Game::instance->db_addLine("[LUA] Error on applyCustomMod(" + name + "," + std::to_string(beat) + "," + std::to_string(beatLength) + "). Mod not found");
			return;
		}

		AppliedMod aMod;
		aMod.mod = mod;
		aMod.beat = beat;
		aMod.length = beatLength;
		aMod.custom = true;
		Gameplay::instance->manager.appliedMods.push_back(aMod);
		});

}
