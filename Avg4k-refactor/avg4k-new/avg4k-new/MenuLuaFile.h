#pragma once
#include "includes.h"
#include "LuaFile.h"

namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:

		std::vector<Average4k::Lua::Base::gameObject> objects{};

		MenuLuaFile(std::string path) : LuaFile(path)
		{
			objects = {};
			Reload();
		}


		void Reload() override;

		void AddObject(Average4k::Lua::Base::gameObject& ob);

		void RemoveObject(Average4k::Lua::Base::gameObject& ob);

		void Load() override;
	};
}