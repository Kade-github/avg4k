#pragma once
#include "GameObject.h"
namespace AvgEngine::Base
{
	class Rectangle : public GameObject
	{
	public:
		int outlinedThickness = 0;
		bool center = false;
		bool sizeToContents = false;


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

		void drawChildren(bool zIndexx)
		{
			Render::Rect prevTrans = transform;
			if (transformRatio && parent) // reverse the ratio
			{
				transform.x = parent->x + (parent->w * (transform.x)) + transformOffset.x;
				transform.y = parent->y + (parent->h * (transform.y)) + transformOffset.y;
				transform.w = (parent->w * (transform.w)) + transformOffset.w;
				transform.h = (parent->h * (transform.h)) + transformOffset.h;
				transform.w = transform.w * transform.scale;
				transform.h = transform.h * transform.scale;

				if (center)
				{
					transform.x -= transform.w / 2;
					transform.y -= transform.h / 2;
				}
			}
			else if (parent)
			{
				transform.x += parent->x + transformOffset.x;
				transform.y += parent->y + transformOffset.y;
				transform.w += transformOffset.w;
				transform.h += transformOffset.h;
			}
			if (zIndexx)
			{
				GameObject::drawTopZIndex();
				transform = prevTrans;
				return;
			}
			GameObject::draw();
			transform = prevTrans;
		}

		void draw() override
		{

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
			iTransform.w *= transform.scale;
			iTransform.h *= transform.scale;

			if (cr.w == 0 && cr.h == 0 && parentClip)
				cr = *parentClip;

			if (r.x + r.w < 0 || r.x > camera->w || r.y + r.h < 0 || r.y > camera->h)
				return;
			
			if (transform.a <= 0)
				return;

			drawChildren(false);

			if (outlinedThickness == 0)
				Primitives::DrawRectangle(camera, zIndex, r, cr);
			else
				Primitives::DrawOutlinedRectangle(camera, zIndex, outlinedThickness, r, cr);
			drawChildren(true);
		}
	};
}