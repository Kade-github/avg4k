#include "LuaFile.h"
#include "Average4K.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/bind.hpp>
#include "stbi.h"
#include "LuaSprite.h"
#include "LuaRectangle.h"
#include "LuaPack.h"
#include "LuaChart.h"
#include "LoadingPacksMenu.h"
#include "LuaAnimatedSprite.h"
#include "SpriteAnimated.h"
using namespace Average4k::Lua;

std::mutex chart_mutex;
std::string endFunction = "";

sol::state* luaPtr = NULL;
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

void Average4k::Lua::LuaFile::loadSettings()
{
	sol::global_table t = lua->globals();

	sol::table ta = lua->create_table_with("version", Average4K::settings->f.settingsVersion);
	for (Setting& s : Average4K::settings->f.settings)
		ta[s.name] = s.value;

	t["settings"] = ta;
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

void Average4k::Lua::LuaFile::CreateObject(Average4k::Lua::Base::gameObject& ob)
{
	int t = ob.type;
	GameObject* o = NULL;
	Sprite* sp = NULL;
	Average4k::External::Spritesheet::AnimatedSprite* aso = NULL;
	AvgEngine::OpenGL::Texture* tex = NULL;
	Text* te = NULL;
	AvgEngine::Base::Rectangle* re = NULL;
	Average4k::Lua::Base::sprite s = Average4k::Lua::Base::sprite(0, 0, Average4k::Lua::Base::texture(""));
	Average4k::Lua::Base::textObject text = Average4k::Lua::Base::textObject(0, 0, "", "");
	Average4k::Lua::Base::rectangle r = Average4k::Lua::Base::rectangle(0, 0, 0, 0);
	Average4k::Lua::Base::animatedSprite as = Average4k::Lua::Base::animatedSprite(0, 0, Average4k::Lua::Base::texture(""));
	switch (t) // creation of that type
	{
	case 0: // default ass game object
		o = new GameObject(ob.transform.x, ob.transform.y);
		ob.id = o->id;
		o->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.transform.base = &o->transform;
		ob.transformOffset.base = &o->transformOffset;
		ob.clipRect.base = &o->clipRect;
		ob.base = o;
		break;
	case 1: // sprite
		s = static_cast<Average4k::Lua::Base::sprite&>(ob);
		tex = getTexture(s.tex.id);
		if (tex == NULL)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to create sprite " + std::to_string(s.id) + ", did you forget a loadTexture?");
			return;
		}
		sp = new Sprite(s.transform.x, s.transform.y, tex);
		sp->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.id = sp->id;
		ob.transform.base = &sp->transform;
		ob.transformOffset.base = &sp->transformOffset;
		ob.clipRect.base = &sp->clipRect;
		ob.base = sp;
		break;
	case 2: // text
		text = static_cast<Average4k::Lua::Base::textObject&>(ob);
		te = new Text(text.transform.x, text.transform.y, Average4K::skin->GetFontPath(), text.font, text.text, text.size);
		text.setText(text.text);
		te->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		te->outlineThickness = text.outline;
		te->centerLines = text.center;
		ob.id = te->id;
		ob.transform.base = &te->transform;
		ob.transformOffset.base = &te->transformOffset;
		ob.clipRect.base = &te->clipRect;
		ob.base = te;
		break;
	case 3: // rectangle
		re = new AvgEngine::Base::Rectangle(ob.transform.x, ob.transform.y, ob.transform.w, ob.transform.h);
		re->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.id = re->id;
		ob.transform.base = &re->transform;
		ob.transformOffset.base = &re->transformOffset;
		ob.clipRect.base = &re->clipRect;
		ob.base = re;
		break;
	case 4: // animatedSprite
		as = static_cast<Average4k::Lua::Base::animatedSprite&>(ob);
		tex = getTexture(as.tex.id);
		if (tex == NULL)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to create animated sprite " + std::to_string(as.id) + ", did you forget a loadTexture?");
			return;
		}
		aso = new Average4k::External::Spritesheet::AnimatedSprite(as.transform.x, as.transform.y, tex);
		aso->transform = AvgEngine::Render::Rect(ob.transform.x, ob.transform.y,
			ob.transform.w, ob.transform.h,
			ob.transform.r, ob.transform.g, ob.transform.b, ob.transform.a,
			ob.transform.scale, ob.transform.deg);
		ob.id = aso->id;
		ob.transform.base = &aso->transform;
		ob.transformOffset.base = &aso->transformOffset;
		ob.clipRect.base = &aso->clipRect;
		ob.base = aso;
		break;
	}
	if (ob.base)
		ob.base->zIndex = ob.getZIndex();
}

void Average4k::Lua::LuaFile::AddObject(Average4k::Lua::Base::gameObject& ob)
{
	if (!ob.base)
		CreateObject(ob);
	if (ob.base)
	{
		Average4K::Instance->CurrentMenu->addObject(ob.base);
		ob.id = ob.base->id;
	}
}

void Average4k::Lua::LuaFile::RemoveObject(Average4k::Lua::Base::gameObject& ob)
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

void LoadChartTexture(LuaFile* _t, std::string path, int index, sol::table* texes)
{
	std::string v = path;
	Average4k::Lua::Base::texture tex = Average4k::Lua::Base::texture("");
	tex.tempData = AvgEngine::External::stbi_h::stbi_load_file_data(v.c_str(), &tex.w, &tex.h);
	tex.path = v;
	chart_mutex.lock();
	(*texes)[index] = tex;
	chart_mutex.unlock();
}

void TweenEnd()
{
	if (endFunction == "")
		return;
	if (luaPtr != NULL)
	{
		sol::protected_function f = (*luaPtr)[endFunction];

		if (!f.valid())
			return;

		sol::function_result x = f();
		if (!x.valid()) {
			sol::error errorstring = x;
			AvgEngine::Logging::writeLog("[Lua] [Error] Lua Error while running " + endFunction + "!\n" + std::string(errorstring.what()));
		}
	}
	endFunction = "";
}

void LuaFile::Load()
{
	using namespace Average4k::Lua;

	luaPtr = lua.get();

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

	lua->set_function("loadArrowTexture", [&](texture& tex) {
		AvgEngine::OpenGL::Texture* t = Average4K::skin->GetArrowTexture(tex.path, true);
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

	lua->set_function("deleteTexture", [&](texture& tex) {
		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i]->id == tex.id)
			{
				delete textures[i];
				textures.erase(textures.begin() + i);
				break;
			}
		}
	});

	lua->set_function("loadChartTexturesThreaded", [&](std::vector<std::string> t, int threads) {
		boost::asio::thread_pool pool(threads);
		sol::table texes = lua->create_table();
		for(int i = 0; i < t.size(); i++)
		{
			try
			{
				boost::asio::post(pool, boost::bind(&LoadChartTexture, this, t[i], i + 1, &texes));
			}
			catch (...)
			{
				AvgEngine::Logging::writeLog("[Lua] [Threaded] [Error] Failed to load chart asset " + t[i]);
			}
		}

		pool.join();

		for (int i = 0; i < texes.size(); i++)
		{
			texture& tex = texes[i + 1];
			AvgEngine::OpenGL::Texture* t = AvgEngine::OpenGL::Texture::loadTextureFromData(tex.tempData, tex.w, tex.h);
			t->dontDelete = true;
			textures.push_back(t);
			tex.id = t->id;
		}

		return texes;
	});

	lua->set_function("getMonitorResolution", [&]() {
		int* res = AvgEngine::Render::DisplayHelper::getMonitorResolution();
		auto table = lua->create_table();
		table.add(res[0]);
		table.add(res[1]);
		return table;
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
		"transform", sol::property(&gameObject::getTransform, &gameObject::setTransform),
		"transformOffset", &gameObject::transformOffset,
		"clip", &gameObject::clipRect,
		"parent", &gameObject::parent,
		"children", &gameObject::children,
		"ratio", sol::property(&gameObject::getRatio, &gameObject::setRatio),
		"order", sol::property(&gameObject::getZIndex, &gameObject::setZIndex),
		"tag", sol::property(&gameObject::getTag, &gameObject::setTag),
		"getChildByTag", &gameObject::getChildByTag,
		"getRealRect", &gameObject::denormilize,
		"add", &gameObject::add,
		"remove", &gameObject::removeObject,
		"destroy", &gameObject::destroy,
		"removeAll", &gameObject::removeAll,
		"visible", &gameObject::getVisible
		);

	sol::usertype<sprite> sprite_type = lua->new_usertype<sprite>("sprite",
		sol::constructors<sprite(double, double, texture)>(),
		"texture", &sprite::tex,
		"center", sol::property(&sprite::getCenter, &sprite::setCenter),
		"recalculateRatio", &sprite::recalculateRatio,
		"clipAll", sol::property(&sprite::getClipAll, &sprite::setClipAll),
		sol::base_classes, sol::bases<gameObject>()
		);

	sol::usertype<rectangle> rectangle_type = lua->new_usertype<rectangle>("rectangle",
		sol::constructors<rectangle(double, double, double, double)>(),
		"outlineThickness", sol::property(&rectangle::getOutline, &rectangle::setOutline),
		"center", sol::property(&rectangle::getCentered, &rectangle::setCentered),
		sol::base_classes, sol::bases<gameObject>()
		);


	sol::usertype<textObject> text_type = lua->new_usertype<textObject>("text",
		sol::constructors<textObject(double, double, std::string, std::string)>(),
		"font", sol::property(&textObject::getFont, &textObject::setFont),
		"text", sol::property(&textObject::getText, &textObject::setText),
		"wrap", sol::property(&textObject::getWrapped, &textObject::setWrapped),
		"size", sol::property(&textObject::getSize, &textObject::setSize),
		"characterSpacing", sol::property(&textObject::getSpacing, &textObject::setSpacing),
		"outline", sol::property(&textObject::getOutline, &textObject::setOutline),
		"center", sol::property(&textObject::getCentered, &textObject::setCenter),
		sol::base_classes, sol::bases<gameObject>()
		);

	sol::usertype<animatedSprite> animatedSprite_type = lua->new_usertype<animatedSprite>("animatedSprite",
		sol::constructors<animatedSprite(float, float, texture)>(),
		"tex", &animatedSprite::tex,
		"center", sol::property(&animatedSprite::getCenter, &animatedSprite::setCenter),
		"fps", sol::property(&animatedSprite::getFPS, &animatedSprite::setFPS),
		"frame", sol::property(&animatedSprite::getFrame, &animatedSprite::setFrame),
		"setFrameSize", &animatedSprite::setFrameSize,
		sol::base_classes, sol::bases<gameObject>()
		);


	using namespace Average4k::Lua::Base;

	sol::global_table t = lua->globals();

	t["online"] = lua->create_table_with("version", Average4K::Instance->Version, "connected",Multiplayer::loggedIn,"username",Multiplayer::username, "avatarData", Multiplayer::currentUserAvatar);

	loadSettings();

	t["skin"] = lua->create_table_with("upscale", Average4K::skin->upscale);

	t["display"] = lua->create_table_with("width", AvgEngine::Render::Display::width, "height", AvgEngine::Render::Display::height);
	
	lua->set_function("create", [&](gameObject& ob) {
		if (ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to create " + std::to_string(ob.id) + ", it already exists!");
			return;
		}
		objects.push_back(ob);
		CreateObject(ob);
	});

	lua->set_function("getKeyName", [&](int key) {
		return AvgEngine::Utils::StringTools::convertKeyCodeToKeyName(key);
	});

	lua->set_function("getKeyCode", [&](std::string keyName) {
		return AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(keyName);
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

	lua->set_function("tween", [&](Average4k::Lua::Base::gameObject& ob, Average4k::Lua::Base::rect endRect, double length, std::string easing, std::string ef) {
		if (!ob.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failure to start tween! Object does not have a base. (did you forget to create it?)");
			return;
		}
	if (ef != "")
		endFunction = ef;
		Average4K::Instance->CurrentMenu->tween.CreateTween(&ob.base->transform,
			AvgEngine::Render::Rect(endRect.x, endRect.y,
				endRect.w, endRect.h, 
				endRect.r, endRect.g, endRect.b, endRect.a, 
				endRect.scale, 
				endRect.deg), 
			length, AvgEngine::Easing::Easing::getEasingFunction(easing), 
			TweenEnd);
	});

	lua->set_function("aabb_object", [&](Average4k::Lua::Base::gameObject& ob, Average4k::Lua::Base::gameObject& ob2) {
		if (!ob.base || !ob2.base)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failure to check AABB! Object(s) do not have (a) base(s). (did you forget to create it?)");
			return false;
		}
		return AvgEngine::Utils::Collision::AABB(ob.base->transform.x, ob.base->transform.y, ob2.base->transform.x, ob2.base->transform.y, ob2.base->transform.w, ob2.base->transform.h, ob.base->transform.w, ob.base->transform.h, ob.base->transformRatio);
	});

	lua->set_function("aabb_rect", [&](rect r1, rect r2, bool ratio) {
		return AvgEngine::Utils::Collision::AABB(r1.x, r1.y, r2.x, r2.y, r2.w, r2.h, r1.w, r1.h, ratio);
	});

	lua->set_function("vec2torect", [&](std::string s) {
		std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(s, ",");
		if (split.size() != 2)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Failed to convert vec2 to rect! Invalid string '" + s + "'!");
			return rect(0, 0);
		}
		return rect(std::stod(split[0]), std::stod(split[1]));
	});

	t["gamepad_a"] = GLFW_GAMEPAD_BUTTON_A;
	t["gamepad_b"] = GLFW_GAMEPAD_BUTTON_B;
	t["gamepad_x"] = GLFW_GAMEPAD_BUTTON_X;
	t["gamepad_y"] = GLFW_GAMEPAD_BUTTON_Y;
	t["gamepad_lb"] = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
	t["gamepad_rb"] = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
	t["gamepad_back"] = GLFW_GAMEPAD_BUTTON_BACK;
	t["gamepad_start"] = GLFW_GAMEPAD_BUTTON_START;
	t["gamepad_ls"] = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
	t["gamepad_rs"] = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
	t["gamepad_dpad_up"] = GLFW_GAMEPAD_BUTTON_DPAD_UP;
	t["gamepad_dpad_right"] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
	t["gamepad_dpad_down"] = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
	t["gamepad_dpad_left"] = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

}