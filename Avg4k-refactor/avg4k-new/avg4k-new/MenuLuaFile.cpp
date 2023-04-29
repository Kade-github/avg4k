#include "MenuLuaFile.h"
#include "Average4K.h"
#include "LuaRectangle.h"
#include "LuaPack.h"
#include "LuaChart.h"
#include "LoadingPacksMenu.h"
#include "LuaAnimatedSprite.h"
#include "SpriteAnimated.h"
void Average4k::Lua::MenuLuaFile::SetPacks(sol::global_table t)
{
	using namespace Average4k::Lua::Menu;
	float startTime = glfwGetTime();

	// Lets set the packs
	t["packs"] = lua->create_table();

	// loop over all the packs in loadingpacksmenu
	for (int i = 0; i < LoadingPacksMenu::packs.size(); i++)
	{
		auto pl = LoadingPacksMenu::packs[i];

		// create a table for the pack
		pack p = pack();
		p.banner = pl.bannerPath;
		p.name = pl.name;
		p.showName = pl.showName;
		p.path = pl.path;
		// Loop over the files 
		for (int j = 0; j < pl.files.size(); j++)
		{
			Average4k::Chart::ChartFile cf = pl.files[j];
			chart c = chart();
			// Set all the metadata types
			c.songArtist = cf.chartMetadata.Song_Artist;
			c.songTitle = cf.chartMetadata.Song_Title;
			c.songSubtitle = cf.chartMetadata.Song_Subtitle;
			c.previewStart = cf.chartMetadata.previewStart;
			c.songBackground = cf.chartMetadata.Song_Background;
			c.songBanner = cf.chartMetadata.Song_Banner;
			c.songCredit = cf.chartMetadata.Song_Credit;
			c.songOffset = cf.chartMetadata.Song_Offset;
			c.songFile = cf.chartMetadata.Song_File;
			c.chartType = cf.chartMetadata.Chart_Type;
			c.path = cf.path;
			c.folder = cf.folder;
			c.isSteam = cf.chartMetadata.isSteam;

			for (Average4k::Chart::Difficulty d : cf.chartMetadata.Difficulties)
			{
				difficulty df;
				df.charter = d.Charter;
				df.difficultyRating = d.DifficultyRating;
				df.name = d.Name;
				for (Average4k::Chart::Note n : d.Notes)
				{
					note nt;
					nt.beat = n.Beat;
					nt.lane = n.Lane;
					nt.type = n.Type;
					df.notes.push_back(nt);
				}
				c.difficulties.push_back(df);
			}

			for (Average4k::Chart::StopPoint s : cf.chartMetadata.Stops)
			{
				stopPoint sp;
				sp.endBeat = s.EndBeat;
				sp.endTime = s.EndTimestamp;
				sp.length = s.Length;
				sp.startBeat = s.StartBeat;
				sp.startTime = s.StartTimestamp;
				c.stopPoints.push_back(sp);
			}

			for (Average4k::Chart::TimingPoint t : cf.chartMetadata.TimingPoints)
			{
				timingPoint tp;
				tp.bpm = t.Bpm;
				tp.endBeat = t.EndBeat;
				tp.endTime = t.EndTimestamp;
				tp.length = t.LengthTimestamp;
				tp.startBeat = t.StartBeat;
				tp.startTime = t.StartTimestamp;
				c.timingPoints.push_back(tp);
			}

			p.files.push_back(c);
		}

		// Set the pack in the lua table
		t["packs"][i + 1] = p;
	}

	float endTime = glfwGetTime();
	AvgEngine::Logging::writeLog("[MenuLuaFile] Set packs in " + std::to_string(endTime - startTime) + " seconds");
}

void Average4k::Lua::MenuLuaFile::CreateObject(Average4k::Lua::Base::gameObject& ob)
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
		ob.id = te->id;
		ob.transform.base = &te->transform;
		ob.transformOffset.base = &te->transformOffset;
		ob.clipRect.base = &te->clipRect;
		ob.base = te;
		break;
	case 3: // rectangle
		re = new AvgEngine::Base::Rectangle(ob.transform.x,ob.transform.y,ob.transform.w,ob.transform.h);
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
}

void Average4k::Lua::MenuLuaFile::AddObject(Average4k::Lua::Base::gameObject& ob)
{
	if (!ob.base)
		CreateObject(ob);
	if (ob.base)
	{
		Average4K::Instance->CurrentMenu->addObject(ob.base);
		ob.id = ob.base->id;
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
		"removeAll", &gameObject::removeAll
		);

	sol::usertype<sprite> sprite_type = lua->new_usertype<sprite>("sprite",
		sol::constructors<sprite(double, double, texture)>(),
		"texture", &sprite::tex,
		"center", sol::property(&sprite::getCenter, &sprite::setCenter),
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
		"center", sol::property(&textObject::getCentered, &textObject::setCenter),
		"size", sol::property(&textObject::getSize, &textObject::setSize),
		"characterSpacing", sol::property(&textObject::getSpacing, &textObject::setSpacing),
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

	using namespace Average4k::Lua::Menu;

	sol::usertype<pack> pack_type = lua->new_usertype<pack>("pack",
		sol::constructors<pack()>(),
		"name", &pack::name,
		"banner", &pack::banner,
		"showName", &pack::showName,
		"files", &pack::files,
		"path", &pack::path
		);
	
	sol::usertype<note> note_type = lua->new_usertype<note>("note",
		sol::constructors<note()>(),
		"beat", &note::beat,
		"lane", &note::lane,
		"type", &note::type
		);

	sol::usertype<stopPoint> stopPoint_type = lua->new_usertype<stopPoint>("stopPoint",
		sol::constructors<stopPoint()>(),
		"startBeat", &stopPoint::startBeat,
		"startTime", &stopPoint::startTime,
		"length", &stopPoint::length,
		"endBeat", &stopPoint::endBeat,
		"endTime", &stopPoint::endTime
		);

	sol::usertype<timingPoint> timingPoint_type = lua->new_usertype<timingPoint>("timingPoint",
		sol::constructors<timingPoint()>(),
		"startBeat", &timingPoint::startBeat,
		"endBeat", &timingPoint::endBeat,
		"startTime", &timingPoint::startTime,
		"endTime", &timingPoint::endTime,
		"length", &timingPoint::length,
		"bpm", &timingPoint::bpm
		);

	sol::usertype<difficulty> difficulty_type = lua->new_usertype<difficulty>("difficulty",
		sol::constructors<difficulty()>(),
		"name", &difficulty::name,
		"charter", &difficulty::charter,
		"difficultyRating", &difficulty::difficultyRating,
		"notes", &difficulty::notes
		);

	sol::usertype<chart> chart_type = lua->new_usertype<chart>("chart",
		sol::constructors<chart()>(),
		"songArtist", &chart::songArtist,
		"songTitle", &chart::songTitle,
		"songSubtitle", &chart::songSubtitle,
		"songCredit", &chart::songCredit,
		"songBanner", &chart::songBanner,
		"songBackground", &chart::songBackground,
		"songFile", &chart::songFile,
		"songOffset", &chart::songOffset,
		"chartType", &chart::chartType,
		"previewStart", &chart::previewStart,
		"timingPoints", &chart::timingPoints,
		"stopPoints", &chart::stopPoints,
		"difficulties", &chart::difficulties,
		"path", &chart::path,
		"folder", &chart::folder
		);


	using namespace Average4k::Lua::Base;

	sol::global_table t = lua->globals();

	t["online"] = lua->create_table_with("version", Average4K::Instance->Version, "connected",Multiplayer::loggedIn,"username",Multiplayer::username, "avatarData", Multiplayer::currentUserAvatar);

	sol::table ta = lua->create_table_with("version", Average4K::settings->f.settingsVersion);
	for (Setting& s : Average4K::settings->f.settings)
		ta.add(&s.name, &s.value);

	t["settings"] = ta;

	t["skin"] = lua->create_table_with("upscale", Average4K::skin->upscale);

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

	lua->set_function("setResolution", [&](int w, int h) {
		if (w <= 0 || h <= 0)
		{
			AvgEngine::Logging::writeLog("[Lua] [Error] Cannot set the resolution to a negative value (or 0)!");
			return;
		}
		Average4K* c = static_cast<Average4K*>(Average4K::Instance);

		c->SetResolution(std::to_string(w) + "x" + std::to_string(h));
	});

	
	lua->set_function("reloadPacks", [&]() {
		// TODO: reload packs

	});

	SetPacks(t);

}
