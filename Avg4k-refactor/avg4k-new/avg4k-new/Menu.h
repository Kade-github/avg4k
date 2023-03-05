#pragma once
#include "includes.h"
#include "Camera.h"
#include "Display.h"
#include "EventManager.h"
#include "GameObject.h"
namespace AvgEngine::Base
{
	/**
	 * \brief A menu object that contains a camera, and game objects in which to then draw
	 */
	class Menu
	{
	public:
		virtual ~Menu() = default;
		Menu()
		{
			camera = Camera(Render::Display::width, Render::Display::height);
			displayRect.w = Render::Display::width;
			displayRect.h = Render::Display::height;
		}
		int lastObjectId = 0;
		std::vector<GameObject*> GameObjects;

		Render::Rect displayRect;

		TweenManager tween{};

		Events::EventManager* eManager = NULL;

		Camera camera;

		virtual void load()
		{
			
		}

		virtual void draw()
		{
			displayRect.w = Render::Display::width;
			displayRect.h = Render::Display::height;
			// Update tweens
			tween.Update();

			for(GameObject* ob : GameObjects)
			{
				// Render objects' draw calls.
				if (ob->render)
					ob->draw();
			}
		}

		virtual void cameraDraw()
		{
			// Now we render the camera
			camera.draw();
		}

		/**
		 * \brief Creates a copy of an object and puts the copy onto a vector with a given id
		 * \param object Object to copy onto the vector
		 */
		virtual void addObject(GameObject* object)
		{
			object->tween = &tween;
			object->id = lastObjectId;
			object->eManager = eManager;
			object->camera = &camera;
			object->parent = &displayRect;
			object->Added();
			GameObjects.push_back(object);
			lastObjectId++;
		}


		/**
		 * \brief Get an object by id
		 * \returns object The object
		 */
		virtual GameObject* getObject(int id)
		{
			for (int i = 0; i < GameObjects.size(); i++)
				if (GameObjects[i]->id == id)
					return GameObjects[i];
			return NULL;
		}

		/**
		 * \brief Removes an object
		 * \param object The object to remove
		 */
		virtual void removeObject(GameObject* object)
		{
			GameObjects.erase(std::ranges::remove_if(GameObjects,
				[&](const GameObject* x) { return *x == *object; }).begin(), GameObjects.end());
		}

		/**
		 * \brief Removes an object
		 * \param id The id of the object to remove
		 */
		virtual void removeObject(int id)
		{
			GameObjects.erase(std::ranges::remove_if(GameObjects,
				[&](const GameObject* x) { return x->id == id; }).begin(), GameObjects.end());
		}

	};
}