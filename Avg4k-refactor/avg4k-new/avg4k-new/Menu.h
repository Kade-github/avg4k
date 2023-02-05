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
		}
		int lastObjectId = 0;
		std::vector<GameObject> GameObjects;

		Events::EventManager* eManager = NULL;

		Camera camera;

		virtual void load()
		{
			
		}

		virtual void draw()
		{
			for(GameObject ob : GameObjects)
			{
				// Render objects' draw calls.
				if (ob.render)
					ob.draw();
			}

			// Now we render the camera
			camera.draw();
		}

		/**
		 * \brief Creates a copy of an object and puts the copy onto a vector with a given id
		 * \param object Object to copy onto the vector
		 */
		virtual void addObject(GameObject object)
		{
			object.id = lastObjectId;
			object.eManager = eManager;
			GameObjects.push_back(object);
			lastObjectId++;
		}

		/**
		 * \brief Removes an object
		 * \param object The object to remove
		 */
		virtual void removeObject(GameObject object)
		{
			GameObjects.erase(std::ranges::remove_if(GameObjects,
				[&](const GameObject x) { return x == object; }).begin(), GameObjects.end());
		}

		/**
		 * \brief Removes an object
		 * \param id The id of the object to remove
		 */
		virtual void removeObject(int id)
		{
			GameObjects.erase(std::ranges::remove_if(GameObjects,
				[&](const GameObject x) { return x.id == id; }).begin(), GameObjects.end());
		}

	};
}