#include "ModManager.h"
#include "Gameplay.h"
#include "Game.h"
#include "Average4k.h"
#include "ArrowEffects.h"
#include "MainerMenu.h"
bool ModManager::doMods = false;
ModManager* ModManager::instance = NULL;

// usertype structs

struct configMod {
	bool downscroll;
	double noteSize;
	double scrollSpeed;
	int displayWidth;
	int displayHeight;
	std::string playField;
};

struct gameMod {

};

struct spriteMetric {
	float width;
	float height;
};

void consolePrint(std::string print)
{
	Game::instance->db_addLine("[LUA] " + print);
}

inline void my_panic(sol::optional<std::string> maybe_msg) {
	if (maybe_msg) {
		const std::string& msg = maybe_msg.value();
		consolePrint("Lua Erorr");
		consolePrint(msg);
		std::cout << "Lua Error " << msg << std::endl;
		Average4k::dumpOutstream();
	}
	Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
}


int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	if (maybe_exception) {
		const std::exception& ex = *maybe_exception;
		std::cout << ex.what() << std::endl;

		consolePrint("Lua Erorr");
		consolePrint(ex.what());
	}
	else {
		std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
		std::cout << std::endl;

		consolePrint("Lua Erorr");
		consolePrint(description.data());
	}
	Average4k::dumpOutstream();
	Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
	return 0;
}


std::map<std::string, std::function<float()>>* luaFunctions;

float ModManager::beat = 0;
float ModManager::time = 0;
float ModManager::bpm = 0;



ModManager::ModManager(std::string luaPath)
{
	killed = false;
	lua = std::make_unique<sol::state>();

	lua->open_libraries(sol::lib::base);

	lua->open_libraries(sol::lib::math);

	lua->set_panic(sol::c_call<decltype(&my_panic), &my_panic>);
	lua->set_exception_handler(&my_exception_handler);

	auto result = lua->safe_script_file(luaPath, &sol::script_pass_on_error);

	if (result.valid())
		std::cout << "Ran " << luaPath << " successfully!" << std::endl;
	else
	{
		sol::error error = result;
		std::cout << "Failed to run " << luaPath << ". " << error.what() << std::endl;
		Game::instance->db_addLine("Lua Error");
		Game::instance->db_addLine(error.what());
		Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
		Average4k::dumpOutstream();
		killed = true;
		return;
	}

	spriteCamera = new AvgGroup(0, 0, 1280, 720);

	assetPath = luaPath.substr(0, luaPath.find_last_of('/'));
	createFunctions();

	populateLuaMap();
}

void ModManager::populateLuaMap()
{
	luaMap["create"] = (*lua)["create"];
	luaMap["update"] = (*lua)["update"];
	luaMap["destroy"] = (*lua)["destroy"];
	luaMap["hit"] = (*lua)["hit"];
}

void ModManager::callEvent(std::string event, std::string args)
{
	if (killed)
		return;
	sol::protected_function f(luaMap[event]);

	if (!f.valid())
		return;


	sol::function_result x = f(args);

	if (!x.valid()) {
		std::string errorstring = x;
		std::cout << "Lua Erorr: \"" << errorstring << "\"";

		consolePrint("Lua Erorr");
		consolePrint(errorstring);
		Average4k::dumpOutstream();
		Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
		killed = true;
		return;
	}

	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
}

void ModManager::callEvent(std::string event, int args)
{
	if (killed)
		return;
	sol::protected_function f(luaMap[event]);

	if (!f.valid())
		return;

	sol::function_result x = f(args);

	if (!x.valid()) {
		std::string errorstring = x;
		std::cout << "Lua Erorr: \"" << errorstring << "\"";

		consolePrint("Lua Erorr");
		consolePrint(errorstring);
		Average4k::dumpOutstream();
		Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
		killed = true;
		return;
	}
	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
}

void ModManager::callEvent(std::string event, float args)
{
	if (killed)
		return;
	sol::protected_function f(luaMap[event]);

	if (!f.valid())
		return;

	sol::function_result x = f(args);

	if (!x.valid()) {
		std::string errorstring = x;
		std::cout << "Lua Erorr: \"" << errorstring << "\"";

		consolePrint("Lua Erorr");
		consolePrint(errorstring);
		Average4k::dumpOutstream();
		Game::instance->asyncShowErrorWindow("Lua Error!", "Check console (F11)", true);
		killed = true;
		return;
	}
	std::sort(appliedMods.begin(), appliedMods.end(), AppliedMod());
}

void ModManager::initLuaFunctions()
{
	if (luaFunctions)
		delete luaFunctions;
	luaFunctions = new std::map<std::string, std::function<float()>>();

	(*luaFunctions)["getBeat"] = [&] {
		// returns the current beat
		return beat;
	};

	(*luaFunctions)["getTime"] = [&] {
		// returns the current time
		return time;
	};

	(*luaFunctions)["getBPM"] = [&] {
		// returns the current bpm
		return time;
	};
}

void ModManager::kill()
{
	killed = true;
	for (const auto& [key, value] : sprites) {
		if (!value.isPlayField && !value.notModCreated)
			spriteCamera->removeObj(value.spr);
	}
	sprites.clear();
}




void ModManager::runMods()
{
	if (killed)
		return;
	for (AppliedMod& m : appliedMods)
	{
		if (m.tweenLen == 0)
		{
			m.tweenLen = 0.1;
			m.instant = true;
		}
		if (m.tweenStart == 0)
			m.tweenStart = beat;

		if ((beat >= m.tweenStart) && ((beat < m.tweenStart + m.tweenLen)))
		{
			if (!m.started)
			{
				m.started = true;
				setModStart(m);
			}

			if (m.mod != "showPath")
			{
				float dur = (beat - m.tweenStart);
				if (dur == 0)
					dur = m.tweenLen;

				float perc = dur / m.tweenLen;

				if (perc > 1)
					perc = 1;

				if (m.tweenCurve == nullptr)
				{
					consolePrint(m.mod + " has a shit tween func. For beat " + std::to_string(m.tweenStart));
					continue;
				}

				float tween = m.tweenCurve(perc);

				if (m.instant)
				{
					tween = 1;
					m.done = true;
					m.tweenLen = 0;
				}

				setModProperties(m, tween);
			}
			else
			{
				setModProperties(m, 0);
			}
		}

	}

	for (FunctionMod& m : funcMod)
	{
		if (m.beat == 0)
			m.beat = beat;
		if (beat >= m.beat && !m.hit)
		{
			m.hit = true;
			m.toCall();
		}
	}

	for (const auto& [key, value] : sprites) {
		if (!value.notModCreated)
		{
			float anchorX = value.spr->defX;
			float anchorY = value.spr->defY;

			if (value.anchor != "")
			{
				SpriteMod anchor = ModManager::instance->sprites[value.anchor];
				anchorX = anchor.spr->defX + anchor.movex;
				anchorY = anchor.spr->defY + anchor.movey;
			}

			float x = anchorX + value.movex;
			float y = anchorY + value.movey;
			float rot = value.spr->defRot + value.confusion;

			value.spr->x = x + value.offsetX;
			value.spr->y = y + value.offsetY;
			value.spr->angle = rot;
			value.spr->alpha = 1 - value.stealth;
			value.spr->scale = 0.5 / value.mini;

			if (value.spr->animationFinished && value.finish != "" && value.spr->sparrow)
			{
				(*instance->lua)[value.finish](value.spr->sparrow->currentAnim);
			}
		}
		else
		{
			float x = value.offsetX + value.movex;
			float y = value.offsetY + value.movey;
			float rot = value.confusion;

			value.def->x = x + value.offsetX;
			value.def->y = y + value.offsetY;
			value.def->angle = rot;
			value.def->alpha = 1 - value.stealth;
			value.def->scale = 0.5 / value.mini;
		}
	}
}

void ModManager::setModStart(AppliedMod& m)
{
	if (m.spriteName != "-1")
	{
		if (m.mod == "movex")
			m.modStartAmount = sprites[m.spriteName].movex;
		if (m.mod == "movey")
			m.modStartAmount = sprites[m.spriteName].movey;
		if (m.mod == "stealth")
			m.modStartAmount = sprites[m.spriteName].stealth;
		if (m.mod == "confusion")
			m.modStartAmount = sprites[m.spriteName].confusion;
		if (m.mod == "defPosX")
			m.modStartAmount = sprites[m.spriteName].spr->defX;
		if (m.mod == "defPosY")
			m.modStartAmount = sprites[m.spriteName].spr->defY;
		if (m.mod == "shaderUniform")
			m.modStartAmount = 0;
		return;
	}
	if (m.mod == "drunk")
		m.modStartAmount = ArrowEffects::drunk;
	if (m.mod == "mini")
		m.modStartAmount = ArrowEffects::mini;
	if (m.mod == "tipsy")
		m.modStartAmount = ArrowEffects::tipsy;
	if (m.mod == "dizzy")
		m.modStartAmount = ArrowEffects::dizzy;
	if (m.mod == "wave")
		m.modStartAmount = ArrowEffects::wave;
	if (m.mod == "amovex")
		m.modStartAmount = ArrowEffects::amovex;
	if (m.mod == "amovey")
		m.modStartAmount = ArrowEffects::amovey;
	if (m.mod == "reverse")
		m.modStartAmount = ArrowEffects::reverse[m.col];
	if (m.mod == "movex")
		m.modStartAmount = ArrowEffects::movex[m.col];
	if (m.mod == "movey")
		m.modStartAmount = ArrowEffects::movey[m.col];
	if (m.mod == "confusion")
		m.modStartAmount = ArrowEffects::confusion[m.col];
	if (m.mod == "aconfusion")
		m.modStartAmount = ArrowEffects::aconfusion;
	if (m.mod == "stealthWhite")
		m.modStartAmount = ArrowEffects::stealthWhite[m.col];
	if (m.mod == "stealthOpacity")
		m.modStartAmount = ArrowEffects::stealthOpacity[m.col];
	if (m.mod == "stealthReceptorOpacity")
		m.modStartAmount = ArrowEffects::stealthReceptorOpacity[m.col];
	if (m.mod == "drunkCol")
		m.modStartAmount = ArrowEffects::drunkCol[m.col];
	if (m.mod == "tipsyCol")
		m.modStartAmount = ArrowEffects::tipsyCol[m.col];
	if (m.mod == "waveCol")
		m.modStartAmount = ArrowEffects::waveCol[m.col];
	if (m.mod == "miniCol")
		m.modStartAmount = ArrowEffects::miniCol[m.col];
	if (m.mod == "pathAlpha")
		m.modStartAmount = ArrowEffects::SplineAlpha;
	if (m.mod == "pathDensity")
		m.modStartAmount = ArrowEffects::SplineDensity;
	if (m.mod == "rotz")
		m.modStartAmount = ArrowEffects::rotz;
}

void ModManager::setModProperties(AppliedMod& m, float tween)
{
	if (m.spriteName != "-1")
	{
		if (m.mod == "movex")
			sprites[m.spriteName].movex = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "movey")
			sprites[m.spriteName].movey = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "stealth")
			sprites[m.spriteName].stealth = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "confusion")
			sprites[m.spriteName].confusion = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "defPosX")
			sprites[m.spriteName].spr->defX = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "defPosY")
			sprites[m.spriteName].spr->defY = std::lerp(m.modStartAmount, m.amount, tween);
		if (m.mod == "shaderUniform")
			shaders[m.shader]->SetUniform(m.param, std::lerp(m.modStartAmount, m.amount, tween));
		return;
	}
	if (m.mod == "drunk")
		ArrowEffects::drunk = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "rotz")
		ArrowEffects::rotz = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "mini")
		ArrowEffects::mini = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "tipsy")
		ArrowEffects::tipsy = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "dizzy")
		ArrowEffects::dizzy = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "wave")
		ArrowEffects::wave = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "confusion")
		ArrowEffects::confusion[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "reverse")
		ArrowEffects::reverse[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "movex")
		ArrowEffects::movex[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "movey")
		ArrowEffects::movey[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "amovex")
		ArrowEffects::amovex = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "amovey")
		ArrowEffects::amovey = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "aconfusion")
		ArrowEffects::aconfusion = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "stealthWhite")
		ArrowEffects::stealthWhite[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "stealthReceptorOpacity")
		ArrowEffects::stealthReceptorOpacity[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "stealthOpacity")
		ArrowEffects::stealthOpacity[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "drawBeats")
		ArrowEffects::drawBeats = m.amount;
	if (m.mod == "drunkCol")
		ArrowEffects::drunkCol[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "tipsyCol")
		ArrowEffects::tipsyCol[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "waveCol")
		ArrowEffects::waveCol[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "miniCol")
		ArrowEffects::miniCol[m.col] = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "showPath")
		ArrowEffects::ShowSplines = m.amount == 1 ? true : false;
	if (m.mod == "pathAlpha")
		ArrowEffects::SplineAlpha = std::lerp(m.modStartAmount, m.amount, tween);
	if (m.mod == "pathDensity")
		ArrowEffects::SplineDensity = std::lerp(m.modStartAmount, m.amount, tween);
}

void ModManager::runMods(AppliedMod m, float beat)
{
	if (killed)
		return;
	setModStart(m);

	if (m.tweenLen == 0)
	{
		m.tweenLen = 0.1;
		m.instant = true;
	}
	if (m.tweenStart == 0)
		m.tweenStart = beat;

	if (m.mod != "showPath")
	{
		float dur = (beat - m.tweenStart);

		float perc = dur / m.tweenLen;

		if (perc > 1)
			perc = 1;

		if (m.tweenCurve == nullptr)
		{
			consolePrint(m.mod + " has a shit tween func. For beat " + std::to_string(m.tweenStart));
			return;
		}

		float tween = m.tweenCurve(perc);

		setModProperties(m, tween);
	}
	else
	{
		setModProperties(m, 0);
	}
	if (m.spriteName != "-1")
	{
		if (!ModManager::instance->sprites[m.spriteName].spr)
		{
			consolePrint("Couldn't find sprite " + m.spriteName);
			return;
		}

		for (const auto& [key, value] : sprites) {
			if (!value.spr)
				continue;
			float anchorX = value.spr->defX;
			float anchorY = value.spr->defY;

			if (value.anchor != "")
			{
				SpriteMod anchor = ModManager::instance->sprites[value.anchor];
				anchorX = anchor.spr->defX + anchor.movex;
				anchorY = anchor.spr->defY + anchor.movey;
			}

			float x = anchorX + value.movex;
			float y = anchorY + value.movey;
			float rot = value.spr->defRot + value.confusion;

			value.spr->x = x + value.offsetX;
			value.spr->y = y + value.offsetY;

			value.spr->angle = rot;
			value.spr->alpha = 1 - value.stealth;

			if (value.spr->animationFinished && value.finish != "" && value.spr->sparrow)
			{
				(*instance->lua)[value.finish](value.spr->sparrow->currentAnim);
			}
		}
	}
}

float getWidth(std::string name)
{
	if (!ModManager::instance->sprites[name].spr)
	{
		consolePrint("Couldn't find sprite " + name);
		return 0;
	}

	return ModManager::instance->sprites[name].spr->w;
}

float getHeight(std::string name)
{
	if (!ModManager::instance->sprites[name].spr)
	{
		consolePrint("Couldn't find sprite " + name);
		return 0;
	}

	return ModManager::instance->sprites[name].spr->h;
}


void ModManager::createFunctions()
{
	// simple helper functions
	for (std::map<std::string, std::function<float()>>::iterator iter = (*luaFunctions).begin(); iter != (*luaFunctions).end(); ++iter)
	{
		lua->set_function(iter->first, iter->second);
	}

	// user types

	lua->new_usertype<configMod>("config", "downscroll", &configMod::downscroll, "noteSize", &configMod::noteSize, "scrollSpeed", &configMod::scrollSpeed, "displayWidth", &configMod::displayWidth, "displayHeight", &configMod::displayHeight, "playField", &configMod::playField);


	configMod cm;
	cm.downscroll = Game::save->GetBool("downscroll");
	cm.noteSize = 64 * Game::save->GetDouble("Note Size");
	cm.playField = "playField";
	cm.displayHeight = Game::gameHeight;
	cm.displayWidth = Game::gameWidth;
	cm.scrollSpeed = Game::instance->save->GetDouble("scrollspeed");

	(*lua)["config"] = cm;

	// special functions

	lua->set_function("consolePrint", consolePrint);


	lua->set_function("setScrollSpeed", [](float speed) {
		if (Game::instance->save->GetBool("Ignore mod scrollspeed"))
			return;
		ArrowEffects::scrollSpeed = speed;
	});

	lua->set_function("setNoteSize", [](float size) {
		ArrowEffects::noteSize = size;
	});

	lua->set_function("activateMod", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount) {
		AppliedMod aMod;
		aMod.mod = name;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.amount = amount;
		aMod.modStartAmount = -999;

		instance->appliedMods.push_back(aMod);
	});

	lua->set_function("activateSpriteMod", [](std::string sprite, std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount) {
		if (!instance->sprites[sprite].spr)
		{
			consolePrint("Couldn't find sprite " + sprite);
			return;
		}
		AppliedMod aMod;
		aMod.mod = name;
		aMod.spriteName = sprite;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.amount = amount;
		aMod.modStartAmount = -999;

		instance->appliedMods.push_back(aMod);
	});

	lua->set_function("activateModMap", [](std::string name, float tweenStart, float tweenLen, std::string easingFunc, float amount, int col) {
		AppliedMod aMod;
		aMod.mod = name;
		aMod.tweenStart = tweenStart;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.col = col;
		aMod.amount = amount;
		aMod.modStartAmount = -999;


		instance->appliedMods.push_back(aMod);
	});

	lua->set_function("getSpriteWidth", &getWidth);

	lua->set_function("applyShader", [](std::string shader, std::string sprite) {
		if (!instance->shaders[shader])
		{
			consolePrint(shader + " doesn't exist!");
			return;
		}
		if (!instance->sprites[sprite].spr)
		{
			consolePrint("Couldn't find sprite " + sprite);
			return;
		}

		instance->sprites[sprite].spr->customShader = instance->shaders[shader];

	});

	lua->set_function("activateModUniform", [](std::string shader, std::string param, float tweenStart, float tweenLen, std::string easingFunc, float value)
	{
		if (!instance->shaders[shader])
		{
			consolePrint(shader + " doesn't exist!");
			return;
		}
		AppliedMod aMod;
		aMod.mod = "shaderUniform";
		aMod.spriteName = shader;
		aMod.tweenStart = tweenStart;
		aMod.param = param;
		aMod.tweenLen = tweenLen;
		aMod.tweenCurve = Easing::getEasingFunction(easingFunc);
		aMod.amount = value;
		aMod.modStartAmount = -999;

		instance->appliedMods.push_back(aMod);
	});

	lua->set_function("getSpriteHeight", &getHeight);

	lua->set_function("setSpriteOffset", [](std::string sprite, float x, float y)
		{
			if (!instance->sprites[sprite].spr)
			{
				consolePrint("Couldn't find sprite " + sprite);
				return;
			}
			SpriteMod& m = instance->sprites[sprite];
			m.offsetX = x;
			m.offsetY = y;
		});

	lua->set_function("createShader", [](std::string name, std::string vert, std::string frag) {
		if (instance->shaders[name])
		{
			consolePrint(name + " already exists!");
			return;
		}
		std::string vertPath = instance->assetPath + "/" + vert + ".shad";
		if (vert == "NULL")
			vertPath = "n";
		std::string fragPath = instance->assetPath + "/" + frag + ".shad";
		if (frag == "NULL")
			fragPath = "n";
		instance->shaders[name] = new Shader(vertPath, fragPath);
		instance->shaders[name]->setProject(GL::projection);
	});

	lua->set_function("createSpline", [](int col, float x, float y, float beatsAway) {
		Spline s;
		s.x = x;
		s.y = y;
		s.beatAway = beatsAway;
		int i = 0;
		if (beatsAway <= 0)
			for (Spline& ss : ArrowEffects::splines[col])
			{
				if (ss.goToReceptor)
				{
					ArrowEffects::splines[col].erase(ArrowEffects::splines[col].begin() + i);
					break;
				}
				i++;
			}
		ArrowEffects::splines[col].push_back(s);
		std::sort(ArrowEffects::splines[col].begin(), ArrowEffects::splines[col].end(), Spline());
	});

	lua->set_function("clearSplines", [](int col) {
		if (col == -1)
		{
			ArrowEffects::splines.clear();
			for (int i = 0; i < 4; i++)
			{
				// x,y, goToReceptor, beatAway
				ArrowEffects::splines[i].push_back({ 0,0,true, false, -1 });
			}
		}
		else
		{
			ArrowEffects::splines[col].clear();
			ArrowEffects::splines[col].push_back({ 0,0,true, false, -1 });
		}
	});

	lua->set_function("dom", [](sol::function f, float beat) {
		FunctionMod m;
		m.toCall = f;
		m.beat = beat;

		instance->funcMod.push_back(m);
	});

	lua->set_function("setSpriteProperty", [](std::string sprite, std::string prop, std::string val) {
		if (!instance->sprites[sprite].spr)
		{
			consolePrint("Couldn't find sprite " + sprite);
			return;
		}
		SpriteMod& m = instance->sprites[sprite];

		if (prop == "sparrow")
		{
			m.spr->setSparrow(instance->assetPath + "/" + val + ".xml");
		}

		if (prop == "loop")
		{
			m.spr->loop = val == "true" ? true : false;
		}

		if (prop == "fps")
		{
			m.spr->fps = std::stoi(val);
		}

		if (prop == "rangeMin")
		{
			m.spr->minFrame = std::stoi(val);
		}

		if (prop == "rangeMax")
		{
			m.spr->maxFrame = std::stoi(val);
		}

		if (prop == "anim")
		{
			m.spr->playAnim(val, m.spr->fps, m.spr->loop);
			m.offsetX = 0;
			m.offsetY = 0;
		}

		if (prop == "animFinish")
		{
			m.finish = val;
		}

		if (prop == "anchor")
		{
			if (!instance->sprites[val].spr)
			{
				consolePrint("Couldn't find sprite " + val + " to anchor");
				return;
			}
			m.anchor = val;
		}
	});

	lua->set_function("createSprite", [](std::string p, std::string name, int x, int y) {
		if (instance->sprites[name].spr)
		{
			consolePrint("Sprite " + name + " already exists!");
			return;
		}
		std::string path = instance->assetPath + "/" + p + ".png";
		AvgSprite* spr = new AvgSprite(x, y, Texture::createWithImage(path));

		SpriteMod mod;
		mod.confusion = 0;
		mod.movex = 0;
		mod.movey = 0;
		mod.spr = spr;

		instance->sprites[name] = mod;
		instance->spriteCamera->add(spr);
	});

	lua->set_function("copyPlayField", [](std::string name) {
		if (instance->sprites[name].spr)
		{
			consolePrint("Sprite " + name + " already exists!");
			return;
		}
		AvgSprite* spr = new AvgSprite(0, 0, instance->sprites["playField"].spr->tex);
		spr->w = Game::gameWidth;
		spr->h = Game::gameHeight;
		spr->dontDelete = true;
		spr->flip = true;
		SpriteMod mod;
		mod.confusion = 0;
		mod.movex = 0;
		mod.movey = 0;
		mod.isPlayField = true;
		mod.spr = spr;

		instance->sprites[name] = mod;
		instance->cam->children.push_back(spr);
	});

	lua->set_function("setNoteskin", [](std::string noteskinName) {
		if (Game::instance->save->GetBool("Ignore mod noteskin"))
			return;
		std::string path = instance->assetPath + "/" + noteskinName;
		Game::noteskin = Noteskin::getNoteskin(path);
	});

}
