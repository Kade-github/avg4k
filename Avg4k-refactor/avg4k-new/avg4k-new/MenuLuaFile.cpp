#include "MenuLuaFile.h"
#include "Average4K.h"
#include "LuaSprite.h"
#include "LuaRectangle.h"
#include "LuaPack.h"
#include "LuaChart.h"
#include "LoadingPacksMenu.h"
#include "LuaAnimatedSprite.h"
#include "SpriteAnimated.h"
#include "LuaChannel.h"
#include "Gameplay.h"
#include "RhythmChannel.h"

std::vector<Average4k::Lua::Menu::LuaChannel> luaChannels_mainmenu_luafile{};

void Average4k::Lua::MenuLuaFile::SetPacks(sol::global_table t)
{
	using namespace Average4k::Lua::Menu;
	float startTime = glfwGetTime();

	// Lets set the packs
	t["packs"] = lua->create_table();

	// loop over all the packs in loadingpacksmenu
	for (int i = 0; i < LoadingPacksMenu::packs.size(); i++)
	{
		auto& pl = LoadingPacksMenu::packs[i];

		// create a table for the pack
		pack p = pack();
		p.banner = pl.bannerPath;
		p.name = pl.name;
		p.showName = pl.showName;
		p.path = pl.path;
		p.base = &pl;
		// Loop over the files 
		for (int j = 0; j < pl.files.size(); j++)
		{
			Average4k::Chart::ChartFile& cf = pl.files[j];
			chart c = chart();
			// Set all the metadata types
			c.songArtist = cf.chartMetadata.Song_Artist;
			c.songTitle = cf.chartMetadata.Song_Title;
			c.songSubtitle = cf.chartMetadata.Song_Subtitle;
			c.previewStart = cf.chartMetadata.previewStart;
			c.songBackground = cf.chartMetadata.Song_Background;
			c.songBanner = cf.chartMetadata.Song_Banner;
			c.songCredit = cf.chartMetadata.Song_Credit;
			c.songOffset = cf.chartMetadata.Song_Offset;
			c.songFile = cf.chartMetadata.Song_File;
			c.chartType = cf.chartMetadata.Chart_Type;
			c.path = cf.path;
			c.folder = cf.folder;
			c.isSteam = cf.chartMetadata.isSteam;
			c.base = &cf;
			for (Average4k::Chart::Difficulty d : cf.chartMetadata.Difficulties)
			{
				difficulty df;
				df.charter = d.Charter;
				df.difficultyRating = d.DifficultyRating;
				df.name = d.Name;
				for (Average4k::Chart::Note n : d.Notes)
				{
					note nt;
					nt.beat = n.Beat;
					nt.lane = n.Lane;
					nt.type = n.Type;
					df.notes.push_back(nt);
				}
				c.difficulties.push_back(df);
			}

			for (Average4k::Chart::StopPoint s : cf.chartMetadata.Stops)
			{
				stopPoint sp;
				sp.length = s.Length;
				sp.startBeat = s.StartBeat;
				sp.startTime = s.StartTimestamp;
				c.stopPoints.push_back(sp);
			}

			for (Average4k::Chart::TimingPoint t : cf.chartMetadata.TimingPoints)
			{
				timingPoint tp;
				tp.bpm = t.Bpm;
				tp.endBeat = t.EndBeat;
				tp.endTime = t.EndTimestamp;
				tp.length = t.LengthTimestamp;
				tp.startBeat = t.StartBeat;
				tp.startTime = t.StartTimestamp;
				c.timingPoints.push_back(tp);
			}

			p.files.push_back(c);
		}

		// Set the pack in the lua table
		t["packs"][i + 1] = p;
	}

	float endTime = glfwGetTime();
	AvgEngine::Logging::writeLog("[MenuLuaFile] Set packs in " + std::to_string(endTime - startTime) + " seconds");
}


void Average4k::Lua::MenuLuaFile::Load()
{
	luaChannels_mainmenu_luafile.clear();
	Average4k::Lua::LuaFile::Load();

	using namespace Average4k::Lua::Menu;

	sol::usertype<pack> pack_type = lua->new_usertype<pack>("pack",
		sol::constructors<pack()>(),
		"name", &pack::name,
		"banner", &pack::banner,
		"showName", &pack::showName,
		"files", &pack::files,
		"path", &pack::path
		);

	sol::usertype<note> note_type = lua->new_usertype<note>("note",
		sol::constructors<note()>(),
		"beat", &note::beat,
		"lane", &note::lane,
		"type", &note::type
		);

	sol::usertype<stopPoint> stopPoint_type = lua->new_usertype<stopPoint>("stopPoint",
		sol::constructors<stopPoint()>(),
		"startBeat", &stopPoint::startBeat,
		"startTime", &stopPoint::startTime,
		"length", &stopPoint::length,
		"endBeat", &stopPoint::endBeat,
		"endTime", &stopPoint::endTime
		);

	sol::usertype<timingPoint> timingPoint_type = lua->new_usertype<timingPoint>("timingPoint",
		sol::constructors<timingPoint()>(),
		"startBeat", &timingPoint::startBeat,
		"endBeat", &timingPoint::endBeat,
		"startTime", &timingPoint::startTime,
		"endTime", &timingPoint::endTime,
		"length", &timingPoint::length,
		"bpm", &timingPoint::bpm
		);

	sol::usertype<difficulty> difficulty_type = lua->new_usertype<difficulty>("difficulty",
		sol::constructors<difficulty()>(),
		"name", &difficulty::name,
		"charter", &difficulty::charter,
		"difficultyRating", &difficulty::difficultyRating,
		"notes", &difficulty::notes
		);

	sol::usertype<chart> chart_type = lua->new_usertype<chart>("chart",
		sol::constructors<chart()>(),
		"songArtist", &chart::songArtist,
		"songTitle", &chart::songTitle,
		"songSubtitle", &chart::songSubtitle,
		"songCredit", &chart::songCredit,
		"songBanner", &chart::songBanner,
		"songBackground", &chart::songBackground,
		"songFile", &chart::songFile,
		"songOffset", &chart::songOffset,
		"chartType", &chart::chartType,
		"previewStart", &chart::previewStart,
		"timingPoints", &chart::timingPoints,
		"stopPoints", &chart::stopPoints,
		"difficulties", &chart::difficulties,
		"path", &chart::path,
		"folder", &chart::folder
		);

	sol::usertype<LuaChannel> LuaChannel_type = lua->new_usertype<LuaChannel>("LuaChannel",
		sol::constructors<LuaChannel(std::string, std::string)>(),
		"path", &LuaChannel::path,
		"name", &LuaChannel::name,
		"isFreed", &LuaChannel::isFreed,
		"Free", &LuaChannel::Free,
		"Play", &LuaChannel::Play,
		"Stop", &LuaChannel::Stop,
		"time", sol::property(&LuaChannel::GetPos, &LuaChannel::SetPos),
		"volume", sol::property(&LuaChannel::GetVolume, &LuaChannel::SetVolume),
		"RateChange", &LuaChannel::RateChange,
		"sampleRate", sol::property(&LuaChannel::GetSampleRate),
		"ConvertToFX", &LuaChannel::ConvertToFX
		);

	lua->set_function("playChannelAsync", [&](std::string path, std::string channelTag) {
		// general idea here is that, 
		// 1. you have a channel tag. 
		// 2. when ever this completes, and the other channel is still existing. murder it and play the new one on the same tag. 
		// 3. profit.
		// (also make this async later)

		Average4k::Audio::RhythmChannel* c = Average4k::Audio::RhythmBASSHelper::CreateChannel(channelTag, path);
		c->Play();
		LuaChannel lc = LuaChannel(path, channelTag);
		lc.base = c;
		luaChannels_mainmenu_luafile.push_back(lc);
		return lc;
	});

	lua->set_function("getChannel", [&](std::string name) {
		for (LuaChannel& c : luaChannels_mainmenu_luafile)
			if (c.name == name)
				return c;
	});

	lua->set_function("removeChannel", [&](std::string name) {
		int i = 0;
		for (LuaChannel& c : luaChannels_mainmenu_luafile)
		{
			if (c.name == name)
			{
				c.Free();
				Average4k::Audio::RhythmBASSHelper::RemoveChannel(name);
				// remove the channel from the vector
				luaChannels_mainmenu_luafile.erase(luaChannels_mainmenu_luafile.begin() + i);
				break;
			}
			i++;
		}
	});

	lua->set_function("setResolution", [&](int w, int h) {
		if (w <= 0 || h <= 0)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Cannot set the resolution to a negative value (or 0)!");
			return;
		}
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);

		c->SetResolution(std::to_string(w) + "x" + std::to_string(h));

	});

	lua->set_function("setFullscreenResolution", [&](int w, int h) {
		if (w <= 0 || h <= 0)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Cannot set the fullscreen resolution to a negative value (or 0)!");
			return;
		}
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);
		if (c->settings->Get("Display").value != "Fullscreen")
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Cannot set the fullscreen resolution when you aren't in fullscreen!");
			return;
		}

		AvgEngine::Render::Display::width = w;
		AvgEngine::Render::Display::height = h;

		c->SetFullscreen("Fullscreen");
		c->SetResolution(std::to_string(w) + "x" + std::to_string(h), false);

	});


	lua->set_function("setFullscreen", [&](std::string type) {
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);

		c->SetFullscreen(type);

	});


	lua->set_function("getSkins", [&]() {
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);

		std::vector<std::string> skins = c->GetSkins();

		sol::table t = lua->create_table();
		for (int i = 0; i < skins.size(); i++)
		{
			t.add(skins[i]);
		}

		return t;

		});

	lua->set_function("setSkin", [&](std::string skin) {
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);
		c->SetSkin(skin,false);
	});

	lua->set_function("setChart", [&](chart& c, int diffIndex) {
		Average4K* s = static_cast<Average4K*>(Average4K::Instance);
		s->options.currentFile = c.base;
		s->options.currentFile_diff = diffIndex - 1;
	});


	lua->set_function("reload", [&]() {
		Average4K* s = static_cast<Average4K*>(Average4K::Instance);
		LuaMenu* lm = static_cast<LuaMenu*>(s->CurrentMenu);
		lm->Reload();
		});


	lua->set_function("startChart", [&]() {

		Average4K* c = static_cast<Average4K*>(Average4K::Instance);
		if (c->options.currentFile == NULL)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Can't call 'startChart()'; No chart selected!");
			return;
		}
		c->SwitchMenu(new Gameplay(c->skin->GetLua("gameplay")));
	});
	
	lua->set_function("reloadPacks", [&]() {
		// TODO: reload packs

	});

	lua->set_function("setSetting", [&](std::string name, std::string value) {
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);
		c->settings->Set(name, value);
		c->settings->Save();
		loadSettings();
	});

	sol::global_table t = lua->globals();

	t["channels"] = lua->create_table();

	SetPacks(t);

}
