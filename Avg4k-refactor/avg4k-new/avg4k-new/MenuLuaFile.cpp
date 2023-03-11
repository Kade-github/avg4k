#include "MenuLuaFile.h"
#include "Average4K.h"

void Average4k::Lua::MenuLuaFile::Reload()
{
	using namespace AvgEngine::Base;
	std::vector<GameObject*>* gameObjects = &Average4K::Instance->CurrentMenu->GameObjects;
	for (GameObject* obj : *gameObjects)
		delete obj;

	gameObjects->clear();

	LuaFile::Reload();
}

void Average4k::Lua::MenuLuaFile::AddObject(Average4k::Lua::Base::gameObject ob)
{
	switch (ob.type) // creation of that type
	{
	case 0: // default ass game object
		GameObject * o = new GameObject(ob.transform.x, ob.transform.y);
		ob.id = o->id;
		Average4K::Instance->CurrentMenu->addObject(o);
		break;
	}
}

void Average4k::Lua::MenuLuaFile::RemoveObject(Average4k::Lua::Base::gameObject ob)
{
	GameObject* o = Average4K::Instance->CurrentMenu->getObject(ob.id);
	if (o == NULL)
	{
		AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove " + std::to_string(ob.id) + ", it doesn't exist!");
		return;
	}

	Average4K::Instance->CurrentMenu->removeObject(ob.id);
}

void Average4k::Lua::MenuLuaFile::Load()
{
	using namespace Average4k::Lua::Base;

	lua->set_function("tween", [&](std::string tween, gameObject ob, Base::rect endRect) {
			GameObject* o = Average4K::Instance->CurrentMenu->getObject(ob.id);
		if (o == NULL)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to tween " + std::to_string(ob.id) + ", it doesn't exist!");
			return;
		}
	});

	lua->set_function("add", [&](gameObject ob) {
		AddObject(ob);
	});

	lua->set_function("remove", [&](gameObject ob) {
		RemoveObject(ob);
	});

	Function("create", "");
}
