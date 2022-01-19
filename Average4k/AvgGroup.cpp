#include "AvgGroup.h"

void AvgGroup::draw() 
	{
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
			obj->draw();
			if (obj->children.size() != 0 && !obj->handleDraw)
				obj->drawChildren();
		}


		Rendering::drawBatch();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
