/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "AvgLuaFile.h"
#include "Stubs/LuaSprite.h"
std::vector<AvgEngine::Base::GameObject*> Average4k::Api::AvgLuaFile::objects = std::vector<AvgEngine::Base::GameObject*>();

Average4k::Api::AvgLuaFile::AvgLuaFile(const std::string& path)
{
	state = std::make_unique<sol::state>();

	this->path = path;
	reload();
}

Average4k::Api::AvgLuaFile::~AvgLuaFile()
{
	reset();
}

void Average4k::Api::AvgLuaFile::load(const std::string& path)
{
	if (state)
	{
		reset();
		state = std::make_unique<sol::state>();
		registerTypes();
	}

	this->path = path;
	state->safe_script_file(path);

	auto result = state->get<sol::optional<std::string>>("error");

	if (result.has_value())
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + path + "\n" + result.value());
		return;
	}

	AvgEngine::Logging::writeLog("[Lua] Loaded file: " + path);

	state->set_function("print", AvgEngine::Logging::writeLog);



}

void Average4k::Api::AvgLuaFile::reload()
{
	load(path);
}

void Average4k::Api::AvgLuaFile::create()
{
	auto create = state->get<sol::optional<sol::function>>("create");

	if (create.has_value())
	{
		sol::function_result result = create.value()();

		if (!result.valid())
		{
			sol::error err = result;
			AvgEngine::Logging::writeLog("[Lua] Error in file: " + path + "\n" + err.what());
		}
	}
	else
	{
		AvgEngine::Logging::writeLog("[Lua] No 'create' function in file: " + path);
	}
}

void Average4k::Api::AvgLuaFile::reset()
{
	state->collect_garbage();
	state.reset();
}

void Average4k::Api::AvgLuaFile::registerTypes()
{
	state->open_libraries(sol::lib::base, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::utf8, sol::lib::debug, sol::lib::jit);

	Average4k::Api::Stubs::LuaSprite::Register(*state);
}

sol::state& Average4k::Api::AvgLuaFile::getState()
{
	return *state;
}
