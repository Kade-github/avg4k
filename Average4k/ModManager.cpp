#include "ModManager.h"
#include "Gameplay.h"
#include "Game.h"
#include "Average4k.h"
// usertype structs

// first 4 id's of funkyPos are receptors

struct receptorMod {
	int col;
	std::string modName;


	float getX() {
		return Gameplay::instance->manager.funkyPositions[col].x;
	}

	float getY() {
		return Gameplay::instance->manager.funkyPositions[col].y;
	}

	void setX(float x) {
		Gameplay::instance->manager.modPositions[modName][col].x = x;
	}

	void setY(float y) {
		Gameplay::instance->manager.modPositions[modName][col].y = y;
	}
};

struct holdMod {
	float beat;
	int id;
	std::string modName;

	float getX() const {
		return Gameplay::instance->manager.funkyPositions[id].x;
	}

	float getY() const {
		return Gameplay::instance->manager.funkyPositions[id].y;
	}
	float getCMod()
	{
		return Gameplay::instance->manager.funkyCMod[id];
	}

	void setCMod(float cmod) {
		Gameplay::instance->manager.modCMod[modName][id] = cmod;
	}

	void setX(float x) {
		Gameplay::instance->manager.modPositions[modName][id].x = x;
	}

	void setY(float y) {
		Gameplay::instance->manager.modPositions[modName][id].y = y;
	}
};

struct noteMod {
	float beat;
	int col;
	int id;
	std::string modName;

	std::vector<holdMod> holds;

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

	float getCMod()
	{
		return Gameplay::instance->manager.funkyCMod[id];
	}

	void setCMod(float cmod) {
		Gameplay::instance->manager.modCMod[modName][id] = cmod;
	}

	void setX(float x) {
		Gameplay::instance->manager.modPositions[modName][id].x = x;
	}

	void setY(float y) {
		Gameplay::instance->manager.modPositions[modName][id].y = y;
	}

	void scaleX(float sx) {

	}

	void scaleY(float sy) {

	}


};

struct configMod {
	bool downscroll;
	double noteSize;
};

struct gameMod {
	std::vector<noteMod> notes;
	std::vector<receptorMod> receptors;
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

void ModManager::storePositions()
{
	for (const auto& p : funkyPositions)
	{
		storedPositions[p.first] = p.second;
	}
	for (const auto& p : funkyCMod)
	{
		storedCMod[p.first] = p.second;
	}
}

void ModManager::runMods()
{
	storePositions();
	for (AppliedMod m : appliedMods)
	{
		if (beat >= m.tweenStart)
		{
			std::vector<noteMod> noteMods;
			std::vector<receptorMod> receptorMods;

			for (NoteObject* obj : Gameplay::instance->spawnedNotes)
			{
				noteMod nm;
				nm.id = obj->modId;
				nm.beat = obj->beat;
				nm.col = obj->lane;
				nm.modName = m.mod.name;

				for (holdTile tile : obj->heldTilings)
				{
					holdMod hm;
					hm.beat = tile.beat;
					hm.id = nm.id + (tile.index + 1);
					hm.modName = m.mod.name;

					nm.holds.push_back(hm);
				}

				noteMods.push_back(nm);
			}

			for (ReceptorObject* obj : Gameplay::instance->receptors)
			{
				receptorMod rm;
				rm.col = obj->type;
				rm.modName = m.mod.name;

				receptorMods.push_back(rm);
			}

			gameMod mod;
			mod.notes = noteMods;
			mod.receptors = receptorMods;
			(*lua)["game"] = mod;

			//gmUser["notes"] = sol::as_table(std::vector<noteMod>(noteMods));
			//gmUser["receptors"] = sol::as_table(std::vector<receptorMod>(receptorMods));

			sol::function_result res = (*lua)[m.mod.luaFunction]((*lua)["game"], mod);
			if (!res.valid())
			{
				sol::error err = res;
				std::cout << "Lua Error " << err.what() << std::endl;
				Game::instance->db_addLine("[LUA] Error on " + m.mod.name + ". Check log.txt for more information.");
				Average4k::dumpOutstream();
			}

			float dur = beat - m.tweenStart;

			float tween = m.tweenCurve(dur / m.tweenLen);

			if (dur > m.tweenLen)
				tween = 1;

			for (const auto& p : modPositions[m.mod.name])
			{
				float startX = storedPositions[p.first].x;
				float endX = p.second.x;
				float startY = storedPositions[p.first].y;
				float endY = p.second.y;
				funkyPositions[p.first].x = startX + ((endX - startX) * tween);
				funkyPositions[p.first].y = startY + ((endY - startY) * tween);
			}

			for (const auto& p : modCMod[m.mod.name])
			{
				if (p.second != 0)
				{
					float startX = storedCMod[p.first];
					float endX = p.second;
					float startY = storedCMod[p.first];
					float endY = p.second;
					funkyCMod[p.first] = startX + ((endX - startX) * tween);
					funkyCMod[p.first] = startY + ((endY - startY) * tween);
				}
			}

			storePositions();
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

	lua->new_usertype<gameMod>("game", "notes", &gameMod::notes, "receptors", &gameMod::receptors);

	lua->new_usertype<configMod>("config", "downscroll", &configMod::downscroll, "noteSize", &configMod::noteSize);

	configMod cm;
	cm.downscroll = Game::save->GetBool("downscroll");
	cm.noteSize = 64 * Game::save->GetDouble("Note Size");

	(*lua)["config"] = cm;

	auto note = lua->new_usertype<noteMod>("note", "beat", &noteMod::beat, "col", &noteMod::col);
	auto hold = lua->new_usertype<holdMod>("note", "beat", &holdMod::beat);

	hold["getX"] = &holdMod::getX;
	hold["getY"] = &holdMod::getY;
	hold["setX"] = &holdMod::setX;
	hold["setY"] = &holdMod::getY;
	hold["getCMod"] = &holdMod::getCMod;
	hold["setCMod"] = &holdMod::setCMod;

	note["holds"] = &noteMod::holds;
	note["getX"] = &noteMod::getX;
	note["getY"] = &noteMod::getY;
	note["getCMod"] = &noteMod::getCMod;
	note["setCMod"] = &noteMod::setCMod;
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

	lua->set_function("activateMod", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc) {
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
			Game::instance->db_addLine("[LUA] Error on activateMod. Mod " + name + " not found!");
			return;
		}

		AppliedMod aMod;
		aMod.mod = mod;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		Gameplay::instance->manager.appliedMods.push_back(aMod);
	});

}
