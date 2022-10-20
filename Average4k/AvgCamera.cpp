#include "AvgCamera.h"
#include "Game.h"

void AvgCamera::leDraw(Object* obj)
{
	glViewport(0, 0, Game::gameWidth, Game::gameHeight);
	GL::genShader->setProject(glm::ortho(0.0f, (float)Game::gameWidth, (float)Game::gameHeight, 0.0f, -1.0f, 1.0f));
	Rendering::rendW = Game::gameWidth;
	Rendering::rendH = Game::gameHeight;
	if (obj->children.size() < 900000)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, fb);

		obj->draw();
		if (obj->handleDraw) // because we binded to another
		{
			Rendering::drawBatch();
			Rendering::setBlend();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Game::gameWidth, Game::gameHeight);
			GL::genShader->setProject(glm::ortho(0.0f, (float)Game::gameWidth, (float)Game::gameHeight, 0.0f, -1.0f, 1.0f));
			Rendering::rendW = 1280;
			Rendering::rendH = 720;

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

				gdstRect.w = gr->w * gr->scale;
				gdstRect.h = gr->h * gr->scale;
			}
			gdstRect.r = 255;
			gdstRect.g = 255;
			gdstRect.b = 255;
			gdstRect.a = 1;

			gsrcRect.x = 0;
			gsrcRect.y = 1;
			gsrcRect.w = 1;
			gsrcRect.h = -1;

			if (gr->fuckingNo)
			{
				gdstRect.x = -9000;
				gdstRect.y = -9000;
			}

			if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
				Rendering::SetClipRect(&gr->clipRect);
			else
				Rendering::SetClipRect(NULL);

			if (gr->customShader)
				Rendering::PushQuad(&gdstRect, &gsrcRect, gr->ctb, gr->customShader, gr->angle);
			else
				Rendering::PushQuad(&gdstRect, &gsrcRect, gr->ctb, GL::genShader, gr->angle);

			if (gr->clipRect.w > 0 || gr->clipRect.h > 0)
				Rendering::SetClipRect(NULL);
		}
		Rendering::setBlend();
		if (obj->children.size() != 0 && !obj->handleDraw)
			obj->drawChildren();
	}
}

void AvgCamera::draw()
{
	Rendering::drawBatch();
	//glBindFramebuffer(GL_FRAMEBUFFER, fb);

	Rendering::setBlend();

	for (Object* obj : children)
	{
		if (obj->drawLast)
			continue;
		if (obj == nullptr)
			continue;
		if (obj->w < 0 || obj->h < 0 || obj->id < 0)
			continue;
		if ((obj->x + obj->w < 0 || obj->y + obj->h < 0) || (obj->x > 1280 || obj->y > 720))
			continue;
		if (obj->alpha < 0.05)
			continue;
		if (obj->customDraw)
		{
			obj->draw();
			continue;
		}
		leDraw(obj);
	}

	for (Object* obj : children)
	{
		if (!obj->drawLast)
			continue;
		if (obj == nullptr)
			continue;
		if (obj->w < 0 || obj->h < 0 || obj->id < 0)
			continue;
		if ((obj->x + obj->w < 0 || obj->y + obj->h < 0) || (obj->x > 1280 || obj->y > 720))
			continue;
		if (obj->alpha < 0.05)
			continue;
		if (obj->customDraw)
		{
			obj->draw();
			continue;
		}
		leDraw(obj);
	}


	Rendering::drawBatch();

	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ifb);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);*/
	glViewport(0, 0, 1280, 720);
	GL::genShader->setProject(glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f));
	Rendering::rendW = 1280;
	Rendering::rendH = 720;
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

	//if (!glGetError())
		//std::cout << "opgl error: " << glGetError() << std::endl;

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