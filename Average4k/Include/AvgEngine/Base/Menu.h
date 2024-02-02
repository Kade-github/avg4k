/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once
#ifndef MENU_H
#define MENU_H

#include <AvgEngine/Base/Camera.h>
#include <AvgEngine/Render/Display.h>
#include <AvgEngine/EventManager.h>
#include <AvgEngine/Base/GameObject.h>

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
			object->parentI = &displayRect;
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
			for (GameObject* g : GameObjects)
				if (g->id == object->id)
					GameObjects.erase(std::ranges::remove(GameObjects, g).begin(), GameObjects.end());
		}

		/**
		 * \brief Removes every object, and deletes them from the stack.
		 */
		virtual void removeAll()
		{
			for (GameObject* g : GameObjects)
			{
				delete g;
			}

			GameObjects.clear();
		}

		/**
		 * \brief Removes an object
		 * \param id The id of the object to remove
		 */
		virtual void removeObject(int id)
		{
			for (GameObject* g : GameObjects)
				if (g->id == id)
					GameObjects.erase(std::ranges::remove(GameObjects, g).begin(), GameObjects.end());
		}


	};
}

#endif