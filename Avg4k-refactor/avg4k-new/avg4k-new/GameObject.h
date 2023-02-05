#pragma once
#include "Display.h"
#include "Camera.h"
#include "includes.h"
namespace AvgEngine::Base
{
	/**
	 * \brief A base class for AvgEngine Game Objects
	 */
	class GameObject
	{
	public:
		virtual ~GameObject() = default;

		Events::EventManager* eManager = NULL;

		Camera camera{};

		Render::Rect transform = Render::Rect();

		int id = 0;

		bool render = false;

		GameObject(Render::Rect _transform)
		{
			transform = _transform;
		}

		GameObject(int x, int y)
		{
			transform = Render::Rect();
			transform.x = x;
			transform.y = y;
		}

		GameObject() = default;

		virtual void draw() {};

		bool operator==(const GameObject& other) {
			return id == other.id;
		}

		bool operator()(const GameObject& a, const GameObject& b) {
			return a.id < b.id;
		}
	};
}