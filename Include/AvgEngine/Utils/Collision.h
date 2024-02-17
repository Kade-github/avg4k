/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef COLLISION_H
#define COLLISION_H

#pragma once

#include <AvgEngine/Base/GameObject.h>

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
			float w2, float h2, float dW, float dH, bool ratio = false)
		{
			float _x2 = x2;
			float _y2 = y2;
			if (ratio)
			{
				_x2 = _x2 * dW;
				_y2 = _y2 * dH;
			}

			return x1 >= _x2 &&
				y1 >= _y2 && 
				x1 <= _x2 + w2 && 
				y1 <= _y2 + h2;
		}

		static bool SAT(float x1, float y1,
			float x2, float y2,
			float w2, float h2, float dW, float dH)
		{
			float tR = x1 + dW;
			float tL = x1;
			float tT = y1;
			float tB = y1 + dH;

			float oR = x2 + w2;
			float oL = x2;
			float oT = y2;
			float oB = y2 + h2;

			return tR < oL || tL > oR || tT > oB || tB < oT;

		}

		/// <summary>
		/// A helper function to calculate if an object is inside another object.
		/// </summary>
		/// <param name="obj1">The first object</param>
		/// <param name="obj2">The second object</param>
		/// <returns>If they collide</returns>
		static bool AABB(Base::GameObject* obj1, Base::GameObject* obj2)
		{
			return AABB(
				obj1->transform.x, obj1->transform.y,
				obj2->transform.x, obj2->transform.y,
				obj2->transform.w, obj2->transform.h,
				obj1->transform.w, obj1->transform.h);
		}

		/// <summary>
		/// A helper function to calculate if a point is inside an object.
		/// </summary>
		/// <param name="obj">The object</param>
		/// <param name="x">The point's x value</param>
		/// <param name="y">The point's y value</param>
		/// <param name="w">The point's width</param>
		/// <param name="h">The point's height</param>
		/// <returns>If they collide</returns>
		static bool AABB(Base::GameObject* obj, int x, int y, int w, int h)
		{
			return AABB(
				x, y,
				obj->transform.x, obj->transform.y,
				obj->transform.w, obj->transform.h,
				w,h);
		}

		static bool SAT(Base::GameObject* obj, int x, int y, int w, int h)
		{
			return SAT(
				x, y,
				obj->transform.x, obj->transform.y,
				obj->transform.w, obj->transform.h,
				w, h);
		}
 
		static bool AABB(int x, int y, int w, int h, Base::GameObject* obj)
		{
			return AABB(
				obj->transform.x, obj->transform.y,
				x,y,
				w,h,
				obj->transform.w, obj->transform.h);
		}

		static bool SAT(int x, int y, int w, int h, Base::GameObject* obj)
		{
			return SAT(
				obj->transform.x, obj->transform.y,
				x, y,
				w, h,
				obj->transform.w, obj->transform.h);
		}

	};
}

#endif // !COLLISION_H