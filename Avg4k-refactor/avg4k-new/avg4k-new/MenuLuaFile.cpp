#include "MenuLuaFile.h"
#include "Average4K.h"

void Average4k::Lua::MenuLuaFile::Reload()
{
	using namespace AvgEngine::Base;
	std::vector<GameObject*>* gameObjects = &Average4K::Instance->CurrentMenu->GameObjects;
	for (GameObject* obj : *gameObjects)
		delete obj;

	gameObjects->clear();

	Launch();
	Start();
}

void Average4k::Lua::MenuLuaFile::AddObject(Average4k::Lua::Base::gameObject& ob)
{
	int t = ob.type;
	GameObject* o = NULL;
	Sprite* sp = NULL;
	AvgEngine::OpenGL::Texture* tex = NULL;
	switch (t) // creation of that type
	{
	case 0: // default ass game object
		o = new GameObject(ob.transform.x, ob.transform.y);
		ob.id = o->id;
		ob.transform.base = &o->transform;
		o->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y, 
			ob.transform.w, ob.transform.h, 
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a, 
			ob.transform.scale, ob.transform.deg);
		ob.base = o;
		Average4K::Instance->CurrentMenu->addObject(o);
		break;
	case 1: // sprite
		Average4k::Lua::Base::sprite s = static_cast<Average4k::Lua::Base::sprite&>(ob);
		tex = getTexture(s.tex.id);
		if (tex == NULL)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(s.id) + ", did you forget a loadTexture?");
			return;
		}
		sp = new Sprite(s.transform.x, s.transform.y, tex);
		sp->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.id = sp->id;
		ob.transform.base = &sp->transform;
		ob.base = sp;
		Average4K::Instance->CurrentMenu->addObject(sp);
		break;
	}
}

void Average4k::Lua::MenuLuaFile::RemoveObject(Average4k::Lua::Base::gameObject& ob)
{
	GameObject* o = Average4K::Instance->CurrentMenu->getObject(ob.id);
	if (o == NULL)
	{
		AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove " + std::to_string(ob.id) + ", it doesn't exist!");
		return;
	}

	objects.erase(std::ranges::remove_if(objects,
		[&](const Average4k::Lua::Base::gameObject x) { return x.id == ob.id; }).begin(), objects.end());
	Average4K::Instance->CurrentMenu->removeObject(ob.id);
}

void Average4k::Lua::MenuLuaFile::Load()
{
	Average4k::Lua::LuaFile::Load();
	using namespace Average4k::Lua::Base;

	sol::usertype<rect> rect_type = lua->new_usertype<rect>("rect",
		// 4 constructors, x y, x y w h, x y w h r g b a, x y w h r g b a s d
		sol::constructors<rect(), rect(float, float), rect(float, float, float, float), rect(float, float, float, float, float, float, float, float), rect(float, float, float, float, float, float, float, float, float, float)>(),
		"x", sol::property(&rect::getX, &rect::setX),
		"y", sol::property(&rect::getY, &rect::setY),
		"w", sol::property(&rect::getW, &rect::setW),
		"h", sol::property(&rect::getH, &rect::setH),
		"r", sol::property(&rect::getR, &rect::setR),
		"g", sol::property(&rect::getG, &rect::setG),
		"b", sol::property(&rect::getB, &rect::setB),
		"alpha", sol::property(&rect::getA, &rect::setA),
		"angle", sol::property(&rect::getD, &rect::setD),
		"scale", sol::property(&rect::getS, &rect::setS)
		);
	 
	sol::usertype<gameObject> object_type = lua->new_usertype<gameObject>("gameObject",
		sol::constructors<gameObject(int, int), gameObject()>(),
		"id", &gameObject::id,
		"transform", &gameObject::transform,
		"parent", &gameObject::parent,
		"children", &gameObject::children,
		"add", &gameObject::add,
		"remove", &gameObject::removeObject
		);

	sol::usertype<sprite> sprite_type = lua->new_usertype<sprite>("sprite",
		sol::constructors<sprite(int, int, texture)>(),
		"texture", &sprite::tex,
		sol::base_classes, sol::bases<gameObject>()
		);

	lua->set_function("add", [&](gameObject& ob) {
		objects.push_back(ob);
		AddObject(ob);
	});

	lua->set_function("remove", [&](gameObject& ob) {
		RemoveObject(ob);
	});

}
