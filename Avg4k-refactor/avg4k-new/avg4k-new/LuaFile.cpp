#include "LuaFile.h"

using namespace Average4k::Lua;

inline void lua_panic(sol::optional<std::string> maybe_msg) {
	if (maybe_msg) {
		const std::string& msg = maybe_msg.value();
		AvgEngine::Logging::writeLog("[Lua] Lua Panic!\n" + msg);
	}
	else
		AvgEngine::Logging::writeLog("[Lua] Lua Panic!\nNo message found.");
	AvgEngine::Logging::writeLog("[Lua] 90% of the time (I think 100%) lua_atpanic will return with abort. meaning the program will shut down.");
}


int lua_exception(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	if (maybe_exception) {
		const std::exception& ex = *maybe_exception;
		AvgEngine::Logging::writeLog("[Lua] Lua Exception!\n" + std::string(ex.what()));
	}
	else
		AvgEngine::Logging::writeLog("[Lua] Lua Exception!\n" + std::string(description.data()));
	return sol::stack::push(L, description);
}

void LuaFile::Launch(std::string path)
{
	if (lua)
		delete lua;
	
	lua = new sol::state();

	lua->open_libraries(sol::lib::base);
	lua->open_libraries(sol::lib::table);
	lua->open_libraries(sol::lib::math);
	lua->open_libraries(sol::lib::string);
	lua->open_libraries(sol::lib::utf8);

	lua->set_panic(sol::c_call<decltype(&lua_panic), &lua_panic>);
	lua->set_exception_handler(&lua_exception);

	// Setup base types

	using namespace Average4k::Lua::Base;

	sol::usertype<rect> rect_type = lua->new_usertype<rect>("rect",
		// 4 constructors, x y, x y w h, x y w h r g b a, x y w h r g b a s d
		sol::constructors<rect(), rect(float, float), rect(float, float, float, float), rect(float, float, float, float, float, float, float, float), rect(float, float, float, float, float, float, float, float, float, float)>());



	using namespace Average4k::Lua;

	auto result = lua->safe_script_file(path, &sol::script_pass_on_error);

	if (result.valid())
		AvgEngine::Logging::writeLog("[Lua] Ran " + path);
	else
	{
		sol::error error = result;
		AvgEngine::Logging::writeLog("[Lua] Failed to run " + path);
		AvgEngine::Logging::writeLog("[Lua] Lua Error!\n" + std::string(error.what()));
		return;
	}

	good = true;
	Load();
}
