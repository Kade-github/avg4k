#include "Game.h"




void AvgGroup::forceDraw()
{
	Rendering::setBlend();
	Rendering::drawBatch();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, w, h);
	GL::genShader->setProject(glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f));
	Rendering::rendW = w;
	Rendering::rendH = h;
	Rendering::setBlendSep();

	for (Object* obj : children)
	{
		if (obj == nullptr)
			continue;  
		if (obj->children.size() < 900000)
		{
			//glBindFramebuffer(GL_FRAMEBUFFER, fb);
			obj->draw();
			Rendering::setBlend();
		}
	}
	Rendering::setBlend();
	Rendering::drawBatch();
}

void AvgGroup::draw()
	{
		if (renderOnce)
			return;

		Rendering::drawBatch();
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0, 0, 0, 0);

		glViewport(0, 0, w, h);
		glm::mat4 pro = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);

		GL::genShader->setProject(pro);
		Game::instance->whiteShader->setProject(pro);
		Rendering::rendW = w;
		Rendering::rendH = h;

		Rendering::setBlendSep();

		for (Object* obj : children)
		{
			if (obj == nullptr)
				continue;
			if (obj->children.size() < 900000)
			{
				//glBindFramebuffer(GL_FRAMEBUFFER, fb);
				obj->draw();
				if (obj->handleDraw) // because we binded to another
				{
					Rendering::drawBatch();
					Rendering::setBlend();
					glBindFramebuffer(GL_FRAMEBUFFER, fb);
					glViewport(0, 0, w, h);
					GL::genShader->setProject(glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f));
					Rendering::rendW = w;
					Rendering::rendH = h;

					AvgGroup* gr = (AvgGroup*)obj;

					Rect gdstRect;
					Rect gsrcRect;

					float mpx = (gr->w * (1 - gr->scale)) / 2;
					float mpy = (gr->h * (1 - gr->scale)) / 2;

					gdstRect.x = gr->x + mpx;
					gdstRect.y = gr->y + mpy;

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

		Rendering::setBlend();
		Rendering::drawBatch();

	}
