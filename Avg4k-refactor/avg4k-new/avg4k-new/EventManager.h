#pragma once
#include "includes.h"
namespace AvgEngine::Events
{
	enum class EventType
	{
		Event_KeyPress = 0,
		Event_KeyRelease = 1,
		Event_MouseDown = 2,
		Event_MouseRelease = 3,
		Event_LoadMenu = 4,
		Event_UnloadMenu = 5,
		Event_Null = -1
	};

	struct Event
	{
		EventType type = EventType::Event_Null;
		int data;
	};

	struct Listener
	{
		EventType type = EventType::Event_Null;
		std::function<void(Event)> toCall{};
	};

	class EventManager
	{
	public:
		std::vector<Listener> Listeners{};

		void Subscribe(EventType t, std::function<void(Event)> f)
		{
			Listeners.push_back({ t, f });
		}

		void Clear()
		{
			Listeners.clear();
		}
	};
}