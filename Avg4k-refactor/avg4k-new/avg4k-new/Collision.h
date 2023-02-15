#pragma once
#include "Display.h"
#include "includes.h"
namespace AvgEngine::Utils
{
	class Collision
	{
	public:
		/**
		 * \brief A helper function to calculate if a rectangle is inside another rectangle.
		 * \param x1 X coordinate 1
		 * \param y1 Y coordinate 1
		 * \param x2 X coordinate 2
		 * \param y2 Y coordinate 2
		 * \param w2 Width 2
		 * \param h2 Height 2
		 * \param ratio If the x2/y2 coordinates are a ratio based on the display width and height
		 * \return A bool if the coordinates intersect
		 */
		static bool AABB(
			float x1, float y1, 
			float x2, float y2, 
			float w2, float h2, bool ratio = false)
		{
			float _x2 = x2;
			float _y2 = y2;
			if (ratio)
			{
				_x2 = _x2 * static_cast<float>(Render::Display::width);
				_y2 = _y2 * static_cast<float>(Render::Display::height);
			}

			return x1 > _x2 && 
				y1 > _y2 && 
				x1 < _x2 + w2 && 
				y1< _y2 + h2;
		}
	};
}