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

		Sprite(int x, int y, OpenGL::Texture* _texture) : GameObject(x, y)
		{
			texture = _texture;
			transform.w = texture->width;
			transform.h = texture->height;
		}

		Sprite(int x, int y, std::string filePath) : GameObject(x, y)
		{
			
		}
	};
}