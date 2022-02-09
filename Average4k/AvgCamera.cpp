#include "AvgCamera.h"
#include "Game.h"

void AvgCamera::draw()
{
	Rendering::drawBatch();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

	Rendering::setBlend();

	for (Object* obj : children)
	{
		if (obj == nullptr)
			continue;
		if (obj->w < 0 || obj->h < 0)
			continue;
		if (obj->children.size() < 9000)
		{
			//glBindFramebuffer(GL_FRAMEBUFFER, fb);

			obj->draw();
			if (obj->handleDraw) // because we binded to another
			{
				Rendering::drawBatch();
				Rendering::setBlend();
				glBindFramebuffer(GL_FRAMEBUFFER, fb);

				AvgGroup* gr = (AvgGroup*)obj;

				Rect gdstRect;
				Rect gsrcRect;

				if (gr->center)
				{
					float mpx = (gr->w * (1 - gr->scale)) / 2;
					float mpy = (gr->h * (1 - gr->scale)) / 2;

					gdstRect.x = gr->x + mpx;
					gdstRect.y = gr->y + mpy;

					gdstRect.w = gr->w * gr->scale;
					gdstRect.h = gr->h * gr->scale;
				}
				else
				{

					gdstRect.x = gr->x;
					gdstRect.y = gr->y;

					gdstRect.w = gr->w;
					gdstRect.h = gr->h;
				}
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
				else
					Rendering::SetClipRect(NULL);

				Rendering::PushQuad(&gdstRect, &gsrcRect, gr->ctb, GL::genShader);

				if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
					Rendering::SetClipRect(NULL);
			}
			Rendering::setBlend();
			if (obj->children.size() != 0 && !obj->handleDraw)
				obj->drawChildren();
		}
	}


	Rendering::drawBatch();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Rendering::setBlend();


	Rect dstRect;
	Rect srcRect;

	float mpx = (w * (1 - scale)) / 2;
	float mpy = (h * (1 - scale)) / 2;

	dstRect.x = x + mpx;
	dstRect.y = y + mpy;


	if (shakeTime > 0)
	{
		shakeTime -= Game::deltaTime * 0.2;
		int mi = -40 * shakeInten;
		int ma = 40 * shakeInten;
		dstRect.x += mi + rand() % ((ma + mi) - mi);
		dstRect.y += mi + rand() % ((ma + mi) - mi);
	}

	dstRect.w = w * scale;
	dstRect.h = h * scale;

	dstRect.r = 255;
	dstRect.g = 255;
	dstRect.b = 255;
	dstRect.a = 1;

	// flip texture, cuz its stupid or something
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1;
	srcRect.h = -1;

	if (clipRect.w > 0 || clipRect.h > 0)
		Rendering::SetClipRect(&clipRect);


	Rendering::PushQuad(&dstRect, &srcRect, ctb, GL::genShader);

	if (!glGetError())
		std::cout << "error: " << glGetError() << std::endl;

	if (tint.r != -1)
	{
		dstRect.r = tint.r;
		dstRect.g = tint.g;
		dstRect.b = tint.b;
		dstRect.a = 0.1;
		Rendering::PushQuad(&dstRect, &srcRect, NULL, GL::genShader);
	}

	if (clipRect.w > 0 || clipRect.h > 0)
		Rendering::SetClipRect(NULL);
}