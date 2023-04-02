#pragma once
#include "includes.h"
#include "Menu.h"
#include "Console.h"
#include "EventManager.h"
#include "Text.h"
#include "FntParser.h"

namespace AvgEngine
{
	class Game
	{
	public:
		virtual ~Game() = default;

		static Game* Instance;

		bool alpha = false;

		int fps = 0;

		float fpsCap = 144;

		std::string controllerName = "";

		Debug::Console console{};

		std::mutex eventMutex;

		std::vector<Events::Event> queuedEvents{};

		Events::EventManager eManager;

		GLFWwindow* Window;

		std::string Title;
		std::string Version;

		Base::Menu* CurrentMenu = NULL;

		Base::Text* fpsText = NULL;

		Base::Text* alphaText = NULL;

		Game(std::string _title, std::string ver, int w = 1920, int h = 1080)
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

			eManager = {};
			console.registerEvents(eManager);

			// create font cache
			Fnt::Fnt::fonts = new std::vector<Fnt::Fnt*>();

		}

		virtual void Resize(int w, int h)
		{
			if (CurrentMenu)
			{
				CurrentMenu->camera.resize(w, h);
				Render::Display::defaultShader->setProject(CurrentMenu->camera.projection);
			}
			Render::Display::Resize(Window, w, h);
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

		virtual void update()
		{
			HandleGamepad();

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
			CurrentMenu->draw();
		}

		virtual void QueueEvent(Events::Event e)
		{
			if (eventMutex.try_lock())
			{
				queuedEvents.push_back(e);
				eventMutex.unlock();
			}
		}

		virtual void Event(const Events::Event& e)
		{
			for(Events::Listener listen : eManager.Listeners)
				if (listen.type == e.type && (console.open ? listen.console : true))
					listen.toCall(e);
		}

		virtual void SwitchMenu(Base::Menu* menu)
		{
			if (CurrentMenu)
			{
				for(Base::GameObject* obj : CurrentMenu->GameObjects)
				{
					if (!obj->dontDelete)
						delete obj;
				}
				CurrentMenu->GameObjects.clear();
			}
			CurrentMenu = menu;
			CurrentMenu->eManager = &eManager;
			eManager.Clear();
			CurrentMenu->load();
		}
	};
}
