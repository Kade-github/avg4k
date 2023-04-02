#include "LuaMenu.h"
#include "Average4K.h"
namespace Average4k::Lua
{
	void LuaMenu::load()
	{
		file->Launch();
		file->Start();

		Menu::load();

		eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e)
		{
			file->Function("KeyPress", std::to_string(e.data));
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyRelease, [&](AvgEngine::Events::Event e)
		{
			file->Function("KeyRelease", std::to_string(e.data));
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseDown, [&](AvgEngine::Events::Event e)
		{
			file->Function("MouseDown", e.vector.toString());
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseRelease, [&](AvgEngine::Events::Event e)
		{
			file->Function("MouseUp", e.vector.toString());
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_GamepadPress, [&](AvgEngine::Events::Event e)
		{
			file->Function("GamepadPress", std::to_string(e.data));
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_Resize, [&](AvgEngine::Events::Event e)
		{
			sol::global_table t = file->lua->globals();
			t["skin"]["upscale"] = Average4K::skin->upscale;
			file->Function("Resize", e.vector.toString());
		}, true, true);
	}
}