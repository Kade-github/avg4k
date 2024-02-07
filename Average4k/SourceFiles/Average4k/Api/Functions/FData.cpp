/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FData.h"
#include "../../A4kGame.h"

sol::table Average4k::Api::Functions::FData::GetSkinData()
{
	Data::Types::SkinData& s = A4kGame::gameInstance->saveData.skinData;
	sol::table data = FCharts::Lua->getState().create_table();

	data["name"] = s.name;
	data["relativePath"] = s.relativePath;
	data["path"] = s.path;

	return data;
}

sol::table Average4k::Api::Functions::FData::GetKeybindData()
{
	Data::Types::KeybindData& k = A4kGame::gameInstance->saveData.keybindData;

	sol::table data = FCharts::Lua->getState().create_table();

	data["key0"] = k.key0;
	data["key1"] = k.key1;
	data["key2"] = k.key2;
	data["key3"] = k.key3;

	data["keyPause"] = k.keyPause;
	data["keyRestart"] = k.keyRestart;

	return data;
}

sol::table Average4k::Api::Functions::FData::GetGameplayData()
{
	Data::Types::GameplayData& g = A4kGame::gameInstance->saveData.gameplayData;

	sol::table data = FCharts::Lua->getState().create_table();

	data["cmod"] = g.constantMod;
	data["xmod"] = g.multiplierMod;

	return data;
}

void Average4k::Api::Functions::FData::SetSkinData(sol::table data)
{
	Data::Types::SkinData& s = A4kGame::gameInstance->saveData.skinData;

	s.name = data["name"];
}


void Average4k::Api::Functions::FData::SetKeybindData(sol::table data)
{
	Data::Types::KeybindData& k = A4kGame::gameInstance->saveData.keybindData;

	k.key0 = data["key0"];
	k.key1 = data["key1"];
	k.key2 = data["key2"];
	k.key3 = data["key3"];

	k.keyPause = data["keyPause"];
	k.keyRestart = data["keyRestart"];
}

void Average4k::Api::Functions::FData::SetGameplayData(sol::table data)
{
	Data::Types::GameplayData& g = A4kGame::gameInstance->saveData.gameplayData;

	g.constantMod = data["cmod"];
	g.multiplierMod = data["xmod"];
}

void Average4k::Api::Functions::FData::SetSkin(std::string skinName)
{
	Data::Types::SkinData s;
	s.name = skinName;
	s.path = "Assets/Skins/" + skinName;

	A4kGame::gameInstance->saveData.skinData = s;

	A4kGame::gameInstance->skin = Skin(s);

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");

	AvgEngine::Events::Event e;
	e.type = AvgEngine::Events::EventType::Event_ReloadFont;

	A4kGame::gameInstance->QueueEvent(e);
}
