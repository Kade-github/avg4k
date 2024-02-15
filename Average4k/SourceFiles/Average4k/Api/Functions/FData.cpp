/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FData.h"
#include "../../A4kGame.h"

#include "../../Helpers/SteamHelper.h"
#include "../../Steam/UGCHandler.h"
#include <AvgEngine/Utils/Paths.h>

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

	data["useCmod"] = g.useCmod;
	data["cmod"] = g.constantMod;
	data["xmod"] = g.multiplierMod;
	data["noteskin"] = g.noteskin;
	data["backgroundDim"] = g.backgroundDim;
	data["downscroll"] = g.downscroll;
	data["noteSpace"] = g.noteSpace;
	data["startTime"] = g.startTime;

	return data;
}

sol::table Average4k::Api::Functions::FData::GetVideoData()
{
	Data::Types::VideoData& v = A4kGame::gameInstance->saveData.videoData;

	sol::table data = FCharts::Lua->getState().create_table();

	data["width"] = v.width;
	data["height"] = v.height;
	data["fullscreen"] = v.fullscreen;
	data["borderless"] = v.borderless;

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
	v.fullscreen = data["fullscreen"];
	v.borderless = data["borderless"];

	int* res = AvgEngine::Render::DisplayHelper::getMonitorResolution();

	if (v.width == -1)
		v.width = res[0];

	if (v.height == -1)
		v.height = res[1];

	if (v.width >= res[0])
		v.width = res[0];
	if (v.height >= res[1])
		v.height = res[1];

	if (v.width < 640)
		v.width = 640;

	if (v.height < 480)
		v.height = 480;


	A4kGame::Instance->Resize(v.width, v.height);

	if (v.fullscreen)
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 1);
	}
	else if (v.borderless)
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 2);
	}
	else
	{
		AvgEngine::Render::Display::Fullscreen(A4kGame::Instance->Window, 0);
	}

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
}

void Average4k::Api::Functions::FData::SetAudioData(sol::table data)
{
	Data::Types::AudioData& a = A4kGame::gameInstance->saveData.audioData;

	a.volume = data["volume"];
	a.sfxVolume = data["sfxVolume"];

	for (auto c : AvgEngine::External::BASS::Channels)
	{
		if (c->name.contains("sfx_"))
		{
			c->SetVolume(a.sfxVolume);
		}
		else
		{
			c->SetVolume(a.volume);
		}
	}

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
	g.useCmod = data["useCmod"];
	g.noteskin = data["noteskin"];
	g.backgroundDim = data["backgroundDim"];
	g.downscroll = data["downscroll"];
	g.noteSpace = data["noteSpace"];
	g.startTime = data["startTime"];

	if (g.startTime < 0.01)
		g.startTime = 0.01;

	if (g.noteSpace < 0)
		g.noteSpace = 0;

	if (g.constantMod < 400)
		g.constantMod = 400;

	if (g.multiplierMod < 0.25)
		g.multiplierMod = 0.25;

	if (g.constantMod > 3000)
		g.constantMod = 3000;

	if (g.multiplierMod > 10.0)
		g.multiplierMod = 10.0;


	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");
}


void Average4k::Api::Functions::FData::SetSkin(std::string skinName)
{
	Data::Types::SkinData s;
	s.name = skinName;
	std::string rp = "Assets/Skins/" + skinName;

	if (!AvgEngine::Utils::Paths::pathExists(rp))
	{
		if (Helpers::SteamHelper::IsSteamRunning)
		{
			for (auto sk : Steam::UGCHandler::Instance->subscribedThemes)
			{
				if (sk.first == skinName)
				{
					rp = sk.second;
					s.relativePath = false;
					break;
				}
			}
		}
		if (!AvgEngine::Utils::Paths::pathExists(rp))
		{
			AvgEngine::Logging::writeLog("[Data] [Error] Skin not found: " + skinName);
			return;
		}

	}
	else
		s.relativePath = true;

	s.path = rp;

	A4kGame::gameInstance->saveData.skinData = s;

	A4kGame::gameInstance->skin = Skin(s);

	A4kGame::gameInstance->saveData.Save("Assets/Save/Save.avg");

	AvgEngine::Events::Event e;
	e.type = AvgEngine::Events::EventType::Event_ReloadFont;

	A4kGame::gameInstance->QueueEvent(e);
}