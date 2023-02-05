#pragma once
#include "includes.h"
#include "Menu.h"
#include "EventManager.h"
namespace AvgEngine
{
	class Game
	{
	public:

		static Game* Instance;

		std::mutex eventMutex;

		std::vector<Events::Event> queuedEvents{};

		Events::EventManager eManager{};

		GLFWwindow* Window;

		std::string Title;
		std::string Version;

		Base::Menu* CurrentMenu = NULL;

		Game(std::string _title, std::string ver, int w = 1280, int h = 720)
		{
			Title = _title;
			Version = ver;
			Window = glfwCreateWindow(w,h, _title.c_str(), NULL, NULL);
			if (!Window)
			{
				Logging::writeLog("[AvgEngine] [Error] Failed to create GLFW window (game will most definitely crash)");
				return;
			}
			Instance = this;
			Logging::writeLog("[AvgEngine] Game created, title: " + Title + ". Version: " + Version);
			Render::Display::width = w;
			Render::Display::height = h;
		}

		/**
		 * \brief A helper function to get the current mouse pos
		 * \param x A reference to a variable to output the x coordinate
		 * \param y A reference to a variable to output the y coordinate
		 */
		void GetMousePos(double* x, double* y)
		{
			glfwGetCursorPos(Window, x, y);
		}

		void update()
		{
			if (queuedEvents.size() != 0)
			{
				if (eventMutex.try_lock())
				{
					for (Events::Event& e : queuedEvents)
						Event(e);
					queuedEvents.clear();
					eventMutex.unlock();
				}
			}
		}

		void QueueEvent(Events::Event e)
		{
			if (eventMutex.try_lock())
			{
				queuedEvents.push_back(e);
				eventMutex.unlock();
			}
		}

		void Event(const Events::Event& e)
		{
			for(Events::Listener listen : eManager.Listeners)
				if (listen.type == e.type)
					listen.toCall(e);
		}

		void SwitchMenu(Base::Menu* menu)
		{
			if (CurrentMenu)
			{
				for(Base::GameObject* obj : CurrentMenu->GameObjects)
				{
					delete obj;
				}
				CurrentMenu->GameObjects.clear();
			}
			CurrentMenu = menu;
			CurrentMenu->eManager = &eManager;
			CurrentMenu->load();
		}
	};
}