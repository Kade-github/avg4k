#pragma once
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

		Sprite(int x, int y, float w, float h, unsigned char* data) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = OpenGL::Texture::loadTextureFromData(data, w, h);
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
		}

		Sprite(int x, int y, OpenGL::Texture* _texture) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = _texture;
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
		}

		Sprite(int x, int y, std::string filePath) : GameObject(x, y)
		{
			src = { 0,0,1,1 };
			texture = OpenGL::Texture::createWithImage(filePath);
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
		}

		~Sprite()
		{
			if (!texture->dontDelete)
				delete texture;
		}

		void draw() override
		{
			Render::Rect r = transform;
			if (transformRatio)
			{
				r.x = parent->w * r.x;
				r.y = parent->h * r.y;
			}
			drawCall c = Camera::FormatDrawCall(zIndex, texture, shader, Render::DisplayHelper::RectToVertex(r, src));
			camera->addDrawCall(c);

			GameObject::draw();
		}
	};
}