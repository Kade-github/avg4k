/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef GAME_H
#define GAME_H

#pragma once
#include <mutex>
#include <AvgEngine/Base/Menu.h>
#include <AvgEngine/Debug/Console.h>
#include <AvgEngine/EventManager.h>
#include <AvgEngine/Base/Text.h>

namespace AvgEngine
{
	class Game
	{
	public:
		virtual ~Game() = default;

		static Game* Instance;

		bool alpha = false;

		int fps = 0;

		float fpsCap = 240;

		std::string controllerName = "";

		Debug::Console console{};

		std::mutex eventMutex;

		std::vector<Events::Event> queuedEvents{};

		Events::EventManager eManager;

		GLFWwindow* Window;

		std::string Title;
		std::string Version;

		Base::Menu* CurrentMenu = NULL;
		Base::Menu* NextMenu = NULL;
		Base::Text* fpsText = NULL;

		Base::Text* alphaText = NULL;

		Game(std::string _title, std::string ver, int w = 1920, int h = 1080)
		{
			Title = _title;
			Version = ver;
			Window = glfwCreateWindow(w, h, _title.c_str(), NULL, NULL);
			Render::DisplayHelper::getMonitorResolution(); // set a store state for the monitor resolution (since on fullscreen it returns the fullscreen res)
			if (!Window)
			{
				Logging::writeLog("[AvgEngine] [Error] Failed to create GLFW window (game will most definitely crash)");
				return;
			}
			Instance = this;
			Logging::writeLog("[AvgEngine] Game created, title: " + Title + ". Version: " + Version);
			Render::Display::width = w;
			Render::Display::height = h;

			eManager = {};
			console.registerEvents(eManager);

			// create font cache
			Fnt::Fnt::fonts = new std::vector<Fnt::Fnt*>();

		}

		virtual void Resize(int w, int h)
		{
			Render::Display::Resize(Window, w, h);
			if (CurrentMenu)
			{
				CurrentMenu->camera.resize(Render::Display::width, Render::Display::height);
				Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
			}
			Event(Events::Event(Events::EventType::Event_Resize));
		}

		/**
		 * \brief A helper function to get the current mouse pos
		 * \param x A reference to a variable to output the x coordinate
		 * \param y A reference to a variable to output the y coordinate
		 */
		virtual void GetMousePos(double* x, double* y)
		{
			glfwGetCursorPos(Window, x, y);
		}

		virtual void HandleGamepad()
		{
			GLFWgamepadstate state;

			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				if (controllerName.size() == 0)
				{
					controllerName = glfwGetGamepadName(GLFW_JOYSTICK_1);
					AvgEngine::Logging::writeLog("[Gamepad] Controller conncted with name " + controllerName + " under slot 1.");
				}

				for (int i = 0; i < 15; i++)
					if (state.buttons[i])
						QueueEvent(Events::Event(Events::EventType::Event_GamepadPress, i));
			}
			else
				controllerName = "";
		}

		virtual void Switch()
		{
			if (CurrentMenu != NULL)
			{
				for (Base::GameObject* obj : CurrentMenu->GameObjects)
				{
					if (!obj->dontDelete)
						delete obj;
				}
				CurrentMenu->GameObjects.clear();
			}
			AvgEngine::Base::Menu* lastMenu = CurrentMenu;
			CurrentMenu = NextMenu;
			CurrentMenu->eManager = &eManager;
			eManager.Clear();
			if (lastMenu != NULL)
			{
				lastMenu->tween.Clear();
				delete lastMenu;
			}
			CurrentMenu->load();
			Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
		}

		virtual void update()
		{
			HandleGamepad();

			if (queuedEvents.size() != 0)
			{
				std::vector<int> listtoDelete = {};
				for (Events::Event& e : queuedEvents)
				{
					Event(e);
					if (e.type == Events::EventType::Event_SwitchMenu)
					{
						if (eventMutex.try_lock())
						{
							queuedEvents.clear();
							eventMutex.unlock();
						}
						Switch();
						return;
					}
					listtoDelete.push_back(e.id);
				}
				if (eventMutex.try_lock())
				{
					int index = 0;
					for (int toDelete : listtoDelete)
					{
						for (Events::Event& e : queuedEvents)
						{
							if (e.id == toDelete)
							{
								queuedEvents.erase(queuedEvents.begin() + index);
								break;
							}
						}
					}
					eventMutex.unlock();
				}
			}

			if (CurrentMenu != NULL)
				CurrentMenu->draw();
		}

		virtual void QueueEvent(Events::Event e)
		{
			static int lastId = 0;
			if (eventMutex.try_lock())
			{
				e.id += lastId;
				queuedEvents.push_back(e);
				eventMutex.unlock();
			}
			lastId++;
		}

		virtual void Event(const Events::Event& e)
		{
			for (Events::Listener listen : eManager.Listeners)
				if (listen.type == e.type && (console.open ? listen.console : true))
					listen.toCall(e);
		}

		virtual void SwitchMenu(Base::Menu* menu)
		{
			NextMenu = menu;
			Events::Event e;
			e.type = Events::EventType::Event_SwitchMenu;
			QueueEvent(e);
		}
	};
}

#endif // !AVG_GAME_H