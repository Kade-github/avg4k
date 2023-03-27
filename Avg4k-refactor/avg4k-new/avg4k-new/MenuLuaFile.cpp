#include "MenuLuaFile.h"
#include "Average4K.h"



void Average4k::Lua::MenuLuaFile::CreateObject(Average4k::Lua::Base::gameObject& ob)
{
	int t = ob.type;
	GameObject* o = NULL;
	Sprite* sp = NULL;
	AvgEngine::OpenGL::Texture* tex = NULL;
	Text* te = NULL;
	Average4k::Lua::Base::sprite s = Average4k::Lua::Base::sprite(0, 0, Average4k::Lua::Base::texture(""));
	Average4k::Lua::Base::textObject text = Average4k::Lua::Base::textObject(0, 0, "", "");
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
		break;
	case 1: // sprite
		s = static_cast<Average4k::Lua::Base::sprite&>(ob);
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
		break;
	case 2:
		text = static_cast<Average4k::Lua::Base::textObject&>(ob);
		te = new Text(text.transform.x, text.transform.y, Average4K::skin->GetFontPath(), text.font, text.text, text.size);
		ob.base = te;
		text.setText(text.text);
		te->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.id = te->id;
		ob.transform.base = &te->transform;
		ob.base = te;
		break;
	}
}

void Average4k::Lua::MenuLuaFile::AddObject(Average4k::Lua::Base::gameObject& ob)
{
	if (!ob.base)
		CreateObject(ob);
	Average4K::Instance->CurrentMenu->addObject(ob.base);
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
	ob.destroy();
}

void Average4k::Lua::MenuLuaFile::Load()
{
	Average4k::Lua::LuaFile::Load();
	using namespace Average4k::Lua::Base;

	sol::usertype<rect> rect_type = lua->new_usertype<rect>("rect",
		// 4 constructors, x y, x y w h, x y w h r g b a, x y w h r g b a s d
		sol::constructors<rect(), rect(double, double), rect(double, double, double, double), rect(double, double, double, double, double, double, double, double), rect(double, double, double, double, double, double, double, double, double, double)>(),
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
		sol::constructors<gameObject(double, double), gameObject()>(),
		"id", &gameObject::id,
		"transform", &gameObject::transform,
		"parent", &gameObject::parent,
		"children", &gameObject::children,
		"ratio", sol::property(&gameObject::getRatio, &gameObject::setRatio),
		"add", &gameObject::add,
		"remove", &gameObject::removeObject,
		"destroy", &gameObject::destroy
		);

	sol::usertype<sprite> sprite_type = lua->new_usertype<sprite>("sprite",
		sol::constructors<sprite(double, double, texture)>(),
		"texture", &sprite::tex,
		"center", sol::property(&sprite::getCenter, &sprite::setCenter),
		sol::base_classes, sol::bases<gameObject>()
		);


	sol::usertype<textObject> text_type = lua->new_usertype<textObject>("text",
		sol::constructors<textObject(double, double, std::string, std::string)>(),
		"font", sol::property(&textObject::getFont, &textObject::setFont),
		"text", sol::property(&textObject::getText, &textObject::setText),
		"wrap", sol::property(&textObject::getWrapped, &textObject::setWrapped),
		"center", sol::property(&textObject::getCentered, &textObject::setCenter),
		"size", sol::property(&textObject::getSize, &textObject::setSize),
		"characterSpacing", sol::property(&textObject::getSpacing, &textObject::setSpacing),
		sol::base_classes, sol::bases<gameObject>()
		);

	sol::global_table t = lua->globals();

	t["online"] = lua->create_table_with("version", Average4K::Instance->Version, "connected",Multiplayer::loggedIn,"username",Multiplayer::username, "avatarData", Multiplayer::currentUserAvatar);

	sol::table ta = lua->create_table_with("version", Average4K::settings->f.settingsVersion);
	for (Setting& s : Average4K::settings->f.settings)
		ta.add(&s.name, &s.value);

	t["settings"] = ta;

	t["skin"] = lua->create_table_with("upscale", Average4K::skin->upscale);

	lua->set_function("create", [&](gameObject& ob) {
		if (ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to create " + std::to_string(ob.id) + ", it already exists!");
			return;
		}
		objects.push_back(ob);
		CreateObject(ob);
	});


	lua->set_function("add", [&](gameObject& ob) {
		if (ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to add " + std::to_string(ob.id) + ", it already exists!");
			return;
		}
		objects.push_back(ob);
		AddObject(ob);
	});

	lua->set_function("remove", [&](gameObject& ob) {
		if (!ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to remove " + std::to_string(ob.id) + ", it doesn't exist!");
			return;
		}
		RemoveObject(ob);
	});

	lua->set_function("copyRect", [&](rect& r) {
		return rect(r.x, r.y, r.w, r.h, r.r, r.g, r.b, r.a, r.scale, r.deg);
	});

	lua->set_function("resource", [&](std::string fileName) {
		Average4K* k = static_cast<Average4K*>(Average4K::Instance);
		return k->skin->GetPath(fileName);
	});

	lua->set_function("tween", [&](Average4k::Lua::Base::gameObject& ob, Average4k::Lua::Base::rect endRect, double length, std::string easing) {
		if (!ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failure to start tween! Object does not have a base. (did you forget to create it?)");
			return;
		}
		Average4K::Instance->CurrentMenu->tween.CreateTween(&ob.base->transform,
			AvgEngine::Render::Rect(endRect.x, endRect.y, 
				endRect.w, endRect.h, 
				endRect.r, endRect.g, endRect.b, endRect.a, 
				endRect.scale, 
				endRect.deg), 
			length, AvgEngine::Easing::Easing::getEasingFunction(easing), 
			NULL);
	});

}
