#pragma once
#include "GameObject.h"
namespace AvgEngine::Base
{
	class Rectangle : public GameObject
	{
	public:
		int outlinedThickness = 0;
		bool center = false;
		Rectangle(float _x, float _y, float _w, float _h) : GameObject(_x, _y)
		{
			transform.w = _w;
			transform.h = _h;
			iTransform.w = _w;
			iTransform.h = _h;
		}

		void setRatio(bool r) override
		{
			if (!parent)
			{
				AvgEngine::Logging::writeLog("[Rectangle] [Error] You cannot set the ratio when the parent is null!");
				return;
			}
			if (transform.w > 1)
			{
				if (r)
				{
					transform.w = (iTransform.w / parent->w);
					transform.h = (iTransform.h / parent->h);
				}
				else
				{
					transform.w = iTransform.w;
					transform.h = iTransform.h;
				}
			}
			GameObject::setRatio(r);
		}

		void draw() override
		{
			if (transform.a <= 0)
				return;

			Render::Rect r = transform;
			Render::Rect cr = clipRect;
			if (parent)
			{
				if (transformRatio)
				{
					r.x = parent->x + (parent->w * (r.x)) + transformOffset.x;
					r.y = parent->y + (parent->h * (r.y)) + transformOffset.y;

					r.w = (parent->w * (r.w)) + transformOffset.w;
					r.h = (parent->h * (r.h)) + transformOffset.h;

					if (cr.w != 0 || cr.h != 0)
					{
						cr.x = parent->x + (parent->w * (cr.x)) + transformOffset.x;
						cr.y = parent->y + (parent->h * (cr.y)) + transformOffset.y;
						cr.w = (parent->w * (cr.w)) + transformOffset.w;
						cr.h = (parent->h * (cr.h)) + transformOffset.h;
					}
					else
					{

					}
				}
				else
				{
					r.x += parent->x + transformOffset.x;
					r.y += parent->y + transformOffset.y;
				}
			}

			if (center)
			{
				r.x -= r.w / 2;
				r.y -= r.h / 2;
			}

			iTransform = r;

			if (outlinedThickness == 0)
				Primitives::DrawRectangle(camera, zIndex, r);
			else
				Primitives::DrawOutlinedRectangle(camera, zIndex, outlinedThickness, r);
		}
	};
}