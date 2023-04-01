#pragma once
#include "GameObject.h"
namespace AvgEngine::Base
{
	class Rectangle : public GameObject
	{
	public:
		int outlinedThickness = 0;
		Rectangle(float _x, float _y, float _w, float _h) : GameObject(_x, _y)
		{
			transform.w = _w;
			transform.h = _h;
		}

		void draw() override
		{
			if (outlinedThickness == 0)
				Primitives::DrawRectangle(camera, zIndex, transform);
			else
				Primitives::DrawOutlinedRectangle(camera, zIndex, outlinedThickness, transform);
		}
	};
}