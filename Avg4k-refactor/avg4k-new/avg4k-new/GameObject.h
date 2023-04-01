#pragma once
#include "Display.h"
#include "Camera.h"
#include "includes.h"
#include "TweenManager.h"
#include "EventManager.h"

namespace AvgEngine::Base
{
	/**
	 * \brief A base class for AvgEngine Game Objects
	 */
	class GameObject
	{
	private:
		int lastObjectId = 0;
	public:

		Events::EventManager* eManager = NULL;

		TweenManager* tween{};

		Camera* camera{};

		/**
		 * \brief If the transform should base itself on percentages of the display's width/height
		 */
		bool transformRatio = false;

		bool dontDelete = false;
		Render::Rect iTransform = Render::Rect();
		Render::Rect transform = Render::Rect();
		Render::Rect* parent = NULL;

		std::vector<GameObject*> Children;

		int id = 0;
		int zIndex = 0;

		bool render = true;

		GameObject(Render::Rect _transform)
		{
			transform = _transform;
		}

		GameObject(float x, float y)
		{
			transform = Render::Rect();
			transform.x = x;
			transform.y = y;
		}

		void SetColor(float r, float g, float b)
		{
			transform.r = r;
			transform.g = g;
			transform.b = b;
		}

		virtual ~GameObject()
		{
			if (!dontDelete)
			{
				for (GameObject* o : Children)
					delete o;
			}
		};

		GameObject() = default;

		virtual void draw()
		{
			for (GameObject* ob : Children)
			{
				// Render objects' draw calls.
				if (ob->render && ob->zIndex <= zIndex)
				{
					ob->camera = camera;
					ob->parent = &transform;
					ob->draw();
				}
			}
		};

		virtual void drawTopZIndex()
		{
			for (GameObject* ob : Children)
			{
				// Render objects' draw calls.
				if (ob->render && ob->zIndex > zIndex)
				{
					ob->camera = camera;
					ob->parent = &transform;
					ob->draw();
				}
			}
		};

		virtual void setRatio(bool ratio)
		{
			transformRatio = ratio;
		}

		bool operator==(const GameObject& other) {
			return id == other.id;
		}

		bool operator()(const GameObject& a, const GameObject& b) {
			return a.id < b.id;
		}

		virtual void Added()
		{
			
		}

		/**
		 * \brief Creates a copy of an object and puts the copy onto a vector with a given id
		 * \param object Object to copy onto the vector
		 */
		virtual void addObject(GameObject* object)
		{
			object->tween = tween;
			object->id = lastObjectId;
			object->eManager = eManager;
			object->camera = camera;
			object->parent = &transform;
			object->Added();
			Children.push_back(object);
			lastObjectId++;
		}

		/**
		 * \brief Removes an object
		 * \param object The object to remove
		 */
		virtual void removeObject(GameObject* object)
		{
			Children.erase(std::ranges::remove_if(Children,
				[&](const GameObject* x) { return *x == *object; }).begin(), Children.end());
		}

		/**
		 * \brief Removes an object
		 * \param id The id of the object to remove
		 */
		virtual void removeObject(int id)
		{
			Children.erase(std::ranges::remove_if(Children,
				[&](const GameObject* x) { return x->id == id; }).begin(), Children.end());
		}
	};
}