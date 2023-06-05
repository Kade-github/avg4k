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

		eManager->Subscribe(AvgEngine::Events::EventType::Event_MouseScroll, [&](AvgEngine::Events::Event e)
		{
			file->Function("MouseWheel", std::to_string(e.data));
		});

		eManager->Subscribe(AvgEngine::Events::EventType::Event_GamepadPress, [&](AvgEngine::Events::Event e)
		{
			file->Function("GamepadPress", std::to_string(e.data));
		});

	}
}