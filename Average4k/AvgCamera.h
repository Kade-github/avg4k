#pragma once
#include "AvgGroup.h"
#include "includes.h"

class AvgCamera : public AvgGroup
{
public:
	AvgCamera(int _x, int _y, int _w, int _h) : AvgGroup(_x, _y, _w, _h)
	{
		handleDraw = true;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		w = _w;
		h = _h;
		x = _x;
		y = _y;

		ctb = new Texture(NULL, _w, _h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctb->id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void draw()
	{
		Rendering::drawBatch();
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 0);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (Object* obj : children)
		{
			if (obj == nullptr)
				continue;
			if (obj->w < 0 || obj->h < 0)
				continue;
			if (obj->children.size() < 9000)
			{
				obj->draw();
				if (obj->handleDraw) // because we binded to another
				{
					glBindFramebuffer(GL_FRAMEBUFFER, fb);
					AvgGroup* gr = (AvgGroup*)obj;

					Rect gdstRect;
					Rect gsrcRect;

					gdstRect.x = gr->x;
					gdstRect.y = gr->y;

					gdstRect.w = gr->w * gr->scale;
					gdstRect.h = gr->h * gr->scale;
					gdstRect.r = 255;
					gdstRect.g = 255;
					gdstRect.b = 255;
					gdstRect.a = 1;

					gsrcRect.x = 0;
					gsrcRect.y = 1;
					gsrcRect.w = 1;
					gsrcRect.h = -1;

					if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
						Rendering::SetClipRect(&gr->clipRect);

					Rendering::PushQuad(&gdstRect, &gsrcRect, gr->ctb, GL::genShader);

					if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
						Rendering::SetClipRect(NULL);
				}

				if (obj->children.size() != 0 && !obj->handleDraw)
					obj->drawChildren();
			}
		}


		Rendering::drawBatch();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Rect dstRect;
		Rect srcRect;

		float mpx = (w * (1 - scale)) / 2;
		float mpy = (h * (1 - scale)) / 2;

		dstRect.x = x + mpx;
		dstRect.y = y + mpy;

		dstRect.w = w * scale;
		dstRect.h = h * scale;
		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;
		dstRect.a = 1;

		// flip texture, cuz its stupid or something
		srcRect.x = 0;
		srcRect.y = 1;
		srcRect.w = 1;
		srcRect.h = -1;

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		Rendering::PushQuad(&dstRect, &srcRect, ctb, GL::genShader);

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);
	}

};