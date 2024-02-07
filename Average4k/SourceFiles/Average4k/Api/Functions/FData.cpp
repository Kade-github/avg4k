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

sol::table Average4k::Api::Functions::FData::GetVideoData()
{
	Data::Types::VideoData& v = A4kGame::gameInstance->saveData.videoData;

	sol::table data = FCharts::Lua->getState().create_table();

	data["width"] = v.width;
	data["height"] = v.height;

	return data;
}

sol::table Average4k::Api::Functions::FData::GetAudioData()
{
Data::Types::AudioData& a = A4kGame::gameInstance->saveData.audioData;

	sol::table data = FCharts::Lua->getState().create_table();

	data["volume"] = a.volume;
	data["sfxVolume"] = a.sfxVolume;

	return data;
}

void Average4k::Api::Functions::FData::SetVideoData(sol::table data)
{
	Data::Types::VideoData& v = A4kGame::gameInstance->saveData.videoData;

	v.width = data["width"];
	v.height = data["height"];

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
}

void Average4k::Api::Functions::FData::SetAudioData(sol::table data)
{
	Data::Types::AudioData& a = A4kGame::gameInstance->saveData.audioData;

	a.volume = data["volume"];
	a.sfxVolume = data["sfxVolume"];

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
}

void Average4k::Api::Functions::FData::SetSkinData(sol::table data)
{
	Data::Types::SkinData& s = A4kGame::gameInstance->saveData.skinData;

	s.name = data["name"];

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
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

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
}

void Average4k::Api::Functions::FData::SetGameplayData(sol::table data)
{
	Data::Types::GameplayData& g = A4kGame::gameInstance->saveData.gameplayData;

	g.constantMod = data["cmod"];
	g.multiplierMod = data["xmod"];

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
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
