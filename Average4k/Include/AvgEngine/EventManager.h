/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#pragma once
#include <iostream>
#include <functional>
#include <algorithm>
#include <string>

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
		Event_ReloadFont = 6,
		Event_GamepadPress = 7,
		Event_GamepadAxis = 8,
		Event_Resize = 9,
		Event_MouseScroll = 10,
		Event_SwitchMenu = 11,
		Event_WindowResize = 12,
		Event_CharacterInput = 13,
		Event_Null = -1
	};

	struct Vec
	{
		float x, y;

		std::string toString()
		{
			return std::to_string(x) + "," + std::to_string(y);
		}
	};

	struct Event
	{
		EventType type = EventType::Event_Null;
		int data = 0;
		int id = 0;
		Vec vector = {};
		std::string sData = "";
	};

	struct Listener
	{
		EventType type = EventType::Event_Null;
		std::function<void(Event)> toCall{};
		bool console = false;
		bool clear = true;
		int eId = 0;
		bool operator==(const Listener& other) {
			return eId == other.eId;
		}

	};

	class EventManager
	{
	public:
		int lastId = 0;
		std::vector<Listener> Listeners{};

		int Subscribe(EventType t, std::function<void(Event)> f, bool autoClear = true, bool ignoreConsole = false)
		{
			int o = lastId;
			Listeners.push_back({ t, f , ignoreConsole, autoClear, lastId});
			lastId++;
			return o;
		}

		void RemoveById(int id)
		{
			for (Listener& l : Listeners)
			{
				if (l.eId == id)
				{
					Listeners.erase(std::ranges::remove_if(Listeners,
						[&](const Listener x) { return x == l; }).begin(), Listeners.end());
					return;
				}
			}
		}

		void Clear()
		{
			std::vector<Listener> copy = Listeners;
			for(int i = 0; i < copy.size(); i++)
			{
				Listener& l = copy[i];
				if (l.clear)
					Listeners.erase(std::ranges::remove_if(Listeners,
						[&](const Listener x) { return x == l; }).begin(), Listeners.end());
			}
		}
	};
}

#endif // !EVENTMANAGER_H