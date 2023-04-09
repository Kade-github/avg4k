#include "LuaFile.h"
#include "Average4K.h"
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

void LuaFile::Launch()
{
	for (AvgEngine::OpenGL::Texture* t : textures)
		delete t;
	textures.clear();
	
	lua = std::make_unique<sol::state>();

	lua->open_libraries(sol::lib::base, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::utf8, sol::lib::debug, sol::lib::jit);

	lua->set_panic(sol::c_call<decltype(&lua_panic), &lua_panic>);
	lua->set_exception_handler(&lua_exception);

	Load();
}

void LuaFile::Load()
{
	using namespace Average4k::Lua;

	lua->set_function("cprint", [&](std::string log) {
		AvgEngine::Logging::writeLog("[Lua] " + log);
		});

	lua->set_function("cprinte", [&](std::string log) {
		AvgEngine::Logging::writeLog("[Lua] [Error] " + log);
		});

	lua->set_function("cprintw", [&](std::string log) {
		AvgEngine::Logging::writeLog("[Lua] [Warning] " + log);
		});


	// Setup base types

	using namespace Average4k::Lua::Base;


	sol::usertype<texture> tex_type = lua->new_usertype<texture>("texture",
		sol::constructors<texture(std::string)>(),
		"id", &texture::id,
		"width", &texture::w,
		"height", &texture::h,
		"path", &texture::path
		);
	lua->set_function("loadTexture", [&](texture& tex) {
		AvgEngine::OpenGL::Texture* t = Average4K::skin->GetTexture(tex.path, true);
		t->dontDelete = true;
		textures.push_back(t);
		tex.w = t->width;
		tex.h = t->height;
		tex.id = t->id;
		});

	lua->set_function("loadChartTexture", [&](texture& tex) {
		AvgEngine::OpenGL::Texture* t = Average4K::skin->GetChartTexture(tex.path, true);
		t->dontDelete = true;
		textures.push_back(t);
		tex.w = t->width;
		tex.h = t->height;
		tex.id = t->id;
	});

	lua->set_function("loadTextureData", [&](texture& tex) {
		size_t out;
		char* data = macaron::Base64::Decode(tex.path.c_str(),tex.path.length(),&out);
		AvgEngine::OpenGL::Texture* t = AvgEngine::OpenGL::Texture::loadTextureFromData(data, out);
		t->dontDelete = true;
		textures.push_back(t);
		tex.w = t->width;
		tex.h = t->height;
		tex.id = t->id;
	});

	lua->set_function("getTime", [&]() {
		return glfwGetTime();
	});

	lua->set_function("getMousePos", [&]() {
		double x, y;
		glfwGetCursorPos(Average4K::Instance->Window, &x, &y);
		return std::vector<double>({ x, y });
	});

	lua->set_function("getMousePosRelative", [&]() {
		double x, y;
		glfwGetCursorPos(Average4K::Instance->Window, &x, &y);
		return std::vector<double>({ x / AvgEngine::Render::Display::width, y / AvgEngine::Render::Display::height });
	});


}