#include "GameplayLuaFile.h"
#include "LuaChart.h"
#include "Average4K.h"
#include "ChartFile.h"
void Average4k::Lua::GameplayLuaFile::Launch()
{
	for (AvgEngine::OpenGL::Texture* t : textures)
		delete t;
	textures.clear();

	lua = std::make_unique<sol::state>();

	lua->open_libraries(sol::lib::base, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::utf8, sol::lib::debug, sol::lib::jit);

	Load();
}

void Average4k::Lua::GameplayLuaFile::Load()
{
	Average4k::Lua::LuaFile::Load();

	using namespace Average4k::Lua::Base;

	lua->set_function("setObject", [&](gameObject& object, int id)
	{
		setObjects[id] = object;
	});

	sol::global_table t = lua->globals();

	t["options"] = lua->create_table();
	t["options"]["chart"] = lua->create_table();
	Average4K* c = static_cast<Average4K*>(Average4K::Instance);
	t["options"]["chart"]["title"] = c->options.currentFile->chartMetadata.Song_Title;
	t["options"]["chart"]["artist"] = c->options.currentFile->chartMetadata.Song_Artist;
	t["options"]["chart"]["credit"] = c->options.currentFile->chartMetadata.Song_Credit;
	Average4k::Chart::Difficulty& diff = c->options.currentFile->chartMetadata.Difficulties[c->options.currentFile_diff];
	t["options"]["chart"]["diff"] = diff.Name;
	t["options"]["chart"]["diffRating"] = diff.DifficultyRating;
	t["options"]["chart"]["charter"] = diff.Charter;
	t["options"]["chart"]["offset"] = c->options.currentFile->chartMetadata.Song_Offset;
	t["options"]["chart"]["background"] = c->options.currentFile->chartMetadata.Song_Background;
	t["options"]["chart"]["banner"] = c->options.currentFile->chartMetadata.Song_Banner;
	t["options"]["chart"]["folder"] = c->options.currentFile->folder;


}
