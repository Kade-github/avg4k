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

			Render::Rect r = transform;
			if (transformRatio)
			{
				if (transform.w > 1)
					transform.w = (r.w / parent->w);
				if (transform.h > 1)
					transform.h = (r.h / parent->h);
				r = transform;

				r.x = parent->x + (parent->w * r.x);
				r.y = parent->y + (parent->h * r.y);
				r.w = parent->w * (r.w * transform.scale);
				r.h = parent->h * (r.h * transform.scale);
			}
			else
			{
				r.x += parent->x;
				r.y += parent->y;
			}

			if (center)
			{
				r.x -= (r.w * transform.scale) / 2;
				r.y -= (r.h * transform.scale) / 2;
			}

			drawCall c = Camera::FormatDrawCall(zIndex, texture, shader, Render::DisplayHelper::RectToVertex(r, src));
			camera->addDrawCall(c);

			GameObject::draw();
		}
	};
}

#endif