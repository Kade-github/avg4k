/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "AvgLuaFile.h"
#include "Stubs/LuaSprite.h"
#include "Stubs/LuaText.h"
std::vector<std::shared_ptr<AvgEngine::Base::GameObject>> Average4k::Api::AvgLuaFile::objects = std::vector<std::shared_ptr<AvgEngine::Base::GameObject>>();

Average4k::Api::AvgLuaFile::AvgLuaFile(const std::string& ppath)
{
	state = std::make_unique<sol::state>(sol::c_call<decltype(&Average4k::Api::AvgLuaFile::luaPanic), &Average4k::Api::AvgLuaFile::luaPanic>);

	this->_path = ppath;
	reload();
}


void Average4k::Api::AvgLuaFile::load(const std::string& ppath)
{
	if (!loaded)
	{
		loaded = true;
		state = std::make_unique<sol::state>(sol::c_call<decltype(&Average4k::Api::AvgLuaFile::luaPanic), &Average4k::Api::AvgLuaFile::luaPanic>);
		registerTypes();
	}

	this->_path = ppath;
	state->safe_script_file(_path);

	auto result = state->get<sol::optional<std::string>>("error");

	if (result.has_value())
	{
		AvgEngine::Logging::writeLog("[Lua] Error in file: " + _path + "\n" + result.value());
		return;
	}

	AvgEngine::Logging::writeLog("[Lua] Loaded file: " + _path);

	state->set_function("print", AvgEngine::Logging::writeLog);



}

void Average4k::Api::AvgLuaFile::reload()
{
	load(_path);
}

void Average4k::Api::AvgLuaFile::create()
{
	auto create = state->get<sol::optional<sol::protected_function>>("create");

	if (create.has_value())
	{
		sol::protected_function_result result = create.value()();

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
	state.reset();
}

void Average4k::Api::AvgLuaFile::registerTypes()
{
	state->open_libraries(sol::lib::base, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::utf8, sol::lib::debug, sol::lib::jit);

	Average4k::Api::Stubs::LuaSprite::Register(*state);
	Average4k::Api::Stubs::LuaText::Register(*state);
}

sol::state& Average4k::Api::AvgLuaFile::getState()
{
	return *state;
}

void Average4k::Api::AvgLuaFile::luaPanic(sol::optional<std::string> maybe_msg)
{
	std::string msg = maybe_msg.value_or("No message provided");
	AvgEngine::Logging::writeLog("[Lua] PANIC: " + msg);

	throw std::runtime_error(msg);
}
