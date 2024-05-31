/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <AvgEngine/Base/Camera.h>
#include <algorithm>

void AvgEngine::Base::Camera::draw()
{
	// Viewport width and height
	glViewport(0, 0, w, h);

	std::sort(drawCalls.begin(), drawCalls.end(), drawCall());

	for (drawCall& call : drawCalls)
	{
		// Clear the buffer so we know nothing is there
		Render::Display::ClearBuffer();
		// Set our clip rect (if its not the default one)
		if (call.clip != Render::Rect())
		{
			Render::Display::Clip(&call.clip);
		}
		// Add our call's vertices
		Render::Display::AddVertex(call.vertices);
		// Draw all of the vertices
		Render::Display::DrawBuffer(call.texture, call.shad);
		// Reset the clip
		Render::Display::Clip(NULL);

	}

	// Clear all of the draw calls so we don't draw things twice
	drawCalls.clear();
}