#pragma once
#ifndef AVG_SPRITE_H
#define AVG_SPRITE_H

#include "includes.h"
#include "GameObject.h"


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

		bool center = false;

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

		~Sprite()
		{
			if (!texture->dontDelete)
				delete texture;
		}

		void draw() override
		{
			if (transform.a <= 0)
				return;

			Render::Rect prevTrans = transform;
			if (transformRatio && parent) // reverse the ratio
			{
				transform.x = (parent->w * transform.x);
				transform.y = (parent->h * transform.y);
 				transform.w = (static_cast<float>(texture->width) * transform.scale);
				transform.h = (static_cast<float>(texture->height) * transform.scale);
			}
			GameObject::draw();

			transform = prevTrans;

			Render::Rect r = transform;
			if (transformRatio && parent)
			{
				r.x = parent->x + (parent->w * r.x);
				r.y = parent->y + (parent->h * r.y);

				if (r.w > 1)
				{
					r.w = static_cast<float>(texture->width) / parent->w;
					r.h = static_cast<float>(texture->height) / parent->h;
				}

				r.w = parent->w * r.w;
				r.h = parent->h * r.h;
			}
			else
			{
				r.x += parent->x;
				r.y += parent->y;
			}

			r.w = r.w * transform.scale;
			r.h = r.h * transform.scale;

			if (center)
			{
				r.x -= r.w / 2;
				r.y -= r.h / 2;
			}

			drawCall c = Camera::FormatDrawCall(zIndex, texture, shader, Render::DisplayHelper::RectToVertex(r, src, center));
			camera->addDrawCall(c);
		}
	};
}

#endif