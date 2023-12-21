#include "Average4K.h"

void AvgEngine::Base::GameObject::draw()
{
	for (GameObject* ob : Children)
	{
		// Render object's draw calls.
		if (ob->render && zIndex + ob->zIndex <= zIndex)
		{
			Render::Rect* cr = &clipRect;
			if (parentClip != NULL && (cr->w == 0 && cr->h == 0))
				cr = parentClip;
			if (cr->w != 0 || cr->h != 0)
				ob->parentClip = cr;
			else
				ob->parentClip = NULL;
			ob->camera = camera;
			ob->parent = &transform;
			int oldZ = ob->zIndex;
			ob->zIndex += zIndex;
			ob->draw();
			ob->zIndex = oldZ;
		}
	}

}
