#pragma once
#include "includes.h"
#include "Sprite.h"

namespace Average4k::External::Spritesheet {
	class AnimatedSprite : public AvgEngine::Base::Sprite{
	public:
		int frameWidth = 0;
		int frameHeight = 0;

		int frames = 0;
		int frame = 0;

		int fps = 24;

		double frameTime = 0;
		double start = 0;

		void SetFrameSize(int _w, int _h)
		{
			frameWidth = _w;
			frameHeight = _h;
			frames = texture->width / frameWidth;

			transform.w = frameWidth;
			transform.h = frameHeight;

		}

		AnimatedSprite(float x, float y, std::string filePath) : AvgEngine::Base::Sprite(x, y, filePath) {}
		AnimatedSprite(float x, float y, AvgEngine::OpenGL::Texture* tex) : AvgEngine::Base::Sprite(x, y, tex) {}

		void draw() override
		{
			if (frameTime >= 1.0 / fps)
			{
				frameTime = 0;
				frame++;
				if (frame >= frames)
					frame = 0;
				src.x = frame * frameWidth;
				src.y = 0;
				src.w = frameWidth;
				src.h = frameHeight;
			}
			else
				frameTime += glfwGetTime() - start;

			AvgEngine::Base::Sprite::draw();
			start = glfwGetTime(); // very funny way of getting our own delta time teehee (instead of giving it to us via a call... lol)
		}
	};
}
