#pragma once
#include "includes.h"
#include "LuaFile.h"

namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:
		std::vector<GameObject*> gameObjects;


		MenuLuaFile(std::string path) : LuaFile(path)
		{

		}

		GameObject* GetGameObject(int id)
		{
			for (int i = 0; i < gameObjects.size(); i++)
				if (gameObjects[i]->id == id)
					return gameObjects[i];
			return NULL;
		}

		void Reload() override
		{
			for (GameObject* obj : gameObjects)
				delete obj;
			
			gameObjects.clear();

			LuaFile::Reload();
		}

		void AddObject(Average4k::Lua::Base::gameObject ob)
		{
			gameObjects.push_back(ob.o);
			Average4K::Instance->CurrentMenu->addObject(ob.o);
		}

		void RemoveObject(Average4k::Lua::Base::gameObject ob)
		{

		}

		void Load() override
		{
			using namespace Average4k::Lua::Base;

			lua->set_function("tween", [&](std::string tween, gameObject ob, Base::rect endRect) {
				GameObject* o = Average4K::Instance->CurrentMenu->getObject(ob.o->id);
				if (o == NULL)
				{
					AvgEngine::Logging::writeLog("[Lua] [Error] Failed to tween " + std::to_string(ob.o->id) + ", it doesn't exist!");
					return;
				}
			});

			lua->set_function("add", [&](gameObject ob) {
				GameObject* o = GetGameObject(ob.o->id);
				if (o == NULL)
				{
					AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(ob.o->id) + ", it doesn't exist!");
					return;
				}
			});

			lua->set_function("remove", [&](int id) {

			});

			Function("create", "");
		}
	};
}