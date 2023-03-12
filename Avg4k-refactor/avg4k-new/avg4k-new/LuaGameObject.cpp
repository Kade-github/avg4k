#include "LuaGameObject.h"

#include "Average4K.h"

void Average4k::Lua::Base::gameObject::destroy()
{
	Average4K::Instance->CurrentMenu->removeObject(id);
	delete base;
}