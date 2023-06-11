#pragma once
#include "includes.h"

#include "LuaGameObject.h"
#define SOL_ALL_SAFETIES_ON 1
#define SOL_LUAJIT 1
#include <sol.hpp>

namespace Average4k::Lua
{
	class LuaFile {
	public:
		std::unique_ptr<sol::state> lua;
		std::vector<Average4k::Lua::Base::gameObject> objects{};

		std::string _path;

		bool good = false;

		std::vector<AvgEngine::OpenGL::Texture*> textures;

		AvgEngine::OpenGL::Texture* getTexture(int id)
		{
			for (int i = 0; i < textures.size(); i++)
				if (textures[i]->id == id)
					return textures[i];
			return NULL;
		}

		LuaFile(std::string path)
		{
			_path = path;
			textures = std::vector<AvgEngine::OpenGL::Texture*>();
		}

		~LuaFile()
		{
			for (int i = 0; i < textures.size(); i++)
				delete textures[i];
		}

		void loadSettings();

		void Start()
		{
			auto result = lua->safe_script_file(_path, &sol::script_pass_on_error);

			if (result.valid())
				AvgEngine::Logging::writeLog("[Lua] Loaded " + _path);
			else
			{
				sol::error error = result;
				AvgEngine::Logging::writeLog("[Lua] [Error] Failed to load " + _path);
				AvgEngine::Logging::writeLog("[Lua] [Error] Lua Error!\n" + std::string(error.what()));
				return;
			}

			good = true;
			Function("Create", "");
		}

		virtual void Launch();

		virtual void Load();

		void CreateObject(Average4k::Lua::Base::gameObject& ob);

		void AddObject(Average4k::Lua::Base::gameObject& ob);

		void RemoveObject(Average4k::Lua::Base::gameObject& ob);

		virtual void Reload()
		{

		}

		virtual void Function(std::string func, std::string args)
		{
			sol::protected_function f = (*lua)[func];

			if (!f.valid())
				return;

			sol::function_result x = f(args);
			if (!x.valid()) {
				sol::error errorstring = x;
				AvgEngine::Logging::writeLog("[Lua] [Error] Lua Error while running " + func + "!\n" + std::string(errorstring.what()));
			}
		}

	};
}