/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <AvgEngine/Base/GameObject.h>
#include <AvgEngine/Utils/Logging.h>

namespace AvgEngine::Base
{
	/**
	 * \brief A sprite object that draws a given texture
	 */
	class Sprite : public GameObject
	{
	public:
		OpenGL::Texture* texture = NULL;
		OpenGL::Shader* shader = NULL;

		Render::Rect src;

		Sprite(float x, float y, char* data, size_t size) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = OpenGL::Texture::loadTextureFromData(data,size);
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
			iTransform = transform;
		}

		Sprite(float x, float y, OpenGL::Texture* _texture) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = _texture;
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
			iTransform = transform;
		}

		Sprite(float x, float y, std::string filePath) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = OpenGL::Texture::createWithImage(filePath);
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
			iTransform = transform;
		}

		Sprite(float x, float y) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			iTransform = transform;
		}

		~Sprite()
		{
			if (!texture->dontDelete)
				delete texture;
		}

		void recalculateRatio()
		{
			if (transformRatio)
			{
				if (parentI)
				{
					transform.w = (static_cast<float>(texture->width) / parentI->w);
					transform.h = (static_cast<float>(texture->height) / parentI->h);
					iTransform.w = (transform.w * parentI->w);
					iTransform.h = (transform.h * parentI->h);
				}
				else
				{
					transform.w = (static_cast<float>(texture->width) / parent->w);
					transform.h = (static_cast<float>(texture->height) / parent->h);
					iTransform.w = (transform.w * parent->w);
					iTransform.h = (transform.h * parent->h);
				}
			}
			else
			{
				transform.w = texture->width;
				transform.h = texture->height;
				iTransform.w = transform.w;
				iTransform.h = transform.h;
			}
		}

		void setRatio(bool r) override
		{
			if (!parent)
			{
				AvgEngine::Logging::writeLog("[Sprite] [Error] You cannot set the ratio when the parent is null!");
				return;
			}

			if (transform.w > 1)
			{
				recalculateRatio();
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

			drawChildren(false);

			Render::Rect r = transform;
			Render::Rect cr = clipRect;
			if (parent)
			{
				if (transformRatio)
				{
					r.x = parent->x + (parent->w * (r.x));
					r.y = parent->y + (parent->h * (r.y));

					if (transform.w > 1)
						transform.w = texture->width / parent->w;
					if (transform.h > 1)
						transform.h = texture->height / parent->h;

					r.w = transform.w;
					r.h = transform.h;

					r.w = (parent->w * (r.w));
					r.h = (parent->h * (r.h));

				}
				else
				{
					r.x += parent->x;
					r.y += parent->y;
	
				}
			}
	
			if (center)
			{
				r.x -= r.w / 2;
				r.y -= r.h / 2;
			}

			iTransform = r;

			if (center)
			{
				float mpx = (iTransform.w * (1 - iTransform.scale)) / 2;
				float mpy = (iTransform.h * (1 - iTransform.scale)) / 2;
				iTransform.x += mpx;
				iTransform.y += mpy;
			}

			iTransform.w *= transform.scale;
			iTransform.h *= transform.scale;

			r.x += transformOffset.x;
			r.y += transformOffset.y;
			r.w += transformOffset.w;
			r.h += transformOffset.h;

			if (transform.a > 0)
			{
				if (r.x + r.w < 0 || r.y + r.h < 0)
				{
					drawn = false;
					return;
				}


				Render::Rect srcCopy = src; // normalize the coords for opengl

				if (src.x > 1)
					srcCopy.x = src.x / texture->width;
				if (src.y > 1)
					srcCopy.y = src.y / texture->height;
				if (src.w > 1)
					srcCopy.w = src.w / texture->width;
				if (src.h > 1)
					srcCopy.h = src.h / texture->height;

				drawCall c = Camera::FormatDrawCall(zIndex, texture, shader, Render::DisplayHelper::RectToVertex(r, srcCopy, center), iTransform);
				c.tag = tag;
				if ((cr.w != 0 || cr.h != 0) && clipAll)
					c.clip = cr;
				if (cr.w == 0 && cr.h == 0 && parentClip)
					c.clip = *parentClip;

				if (camera->addDrawCall(c))
					drawn = true;
				else
					drawn = false;
			}

			drawChildren(true);
		}
	};
}

#endif