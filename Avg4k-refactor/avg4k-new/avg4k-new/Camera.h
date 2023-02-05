#pragma once
#include "includes.h"
#include "Display.h"
namespace AvgEngine::Base
{
	struct drawCall
	{
		int zIndex = 0;
		std::vector<Render::Vertex> vertices{};
		OpenGL::Texture* texture = NULL;
		OpenGL::Shader* shad = NULL;

		bool operator==(const drawCall& other) {
			return (zIndex == other.zIndex) &&
				(texture->id == other.texture->id) &&
				(shad->program == other.shad->program);
		}

		bool operator()(const drawCall& a, const drawCall& b) {
			return a.zIndex < b.zIndex;
		}

	};

	/**
	 * \brief A camera object that organizes and draws objects
	 */
	class Camera
	{
	public:
		virtual ~Camera() = default;
		std::vector<drawCall> drawCalls{};
		Camera() = default;
		int w, h;
		Camera(int w, int h)
		{
			resize(w, h);
		}

		glm::mat4 projection{};

		/**
		 * \brief Resize the projection matrix to the set width and height
		 * \param w The width to resize to
		 * \param h The height to resize to
		 */
		void resize(float w, float h)
		{
			projection = glm::ortho(0.0f, w, h, 0.0f, -1.0f, 1.0f);
		}

		/**
		 * \brief Add a draw call (or if it already exists, add on to it)
		 * \param call The draw call struct to add
		 */
		void addDrawCall(drawCall& call)
		{
			// See if we can find a draw call already with the same shader, texture, and zIndex
			auto it = std::find(drawCalls.begin(), drawCalls.end(), call);
			if (it != drawCalls.end()) {
				// We found it, so instead of creating a new draw call lets just append to this one
				auto index = std::distance(drawCalls.begin(), it);
				drawCall& modify = drawCalls[index];
				for (Render::Vertex v : call.vertices)
					modify.vertices.push_back(v);
			}
			else // We didn't find it, so we can just push the draw call directly onto the vector
				drawCalls.push_back(call);
		}

		/**
		 * \brief Render all of the current draw calls
		 */
		virtual void draw()
		{
			// Viewport width and height
			glViewport(0, 0, w, h);
			// Sort all of the draw calls based on their zIndex
			std::ranges::sort(drawCalls, drawCall());
			for(drawCall& call : drawCalls)
			{
				// Clear the buffer so we know nothing is there
				Render::Display::ClearBuffer();
				// Add our call's vertices
				Render::Display::AddVertex(call.vertices);
				// Set our projection matrix to the shader
				call.shad->setProject(projection);
				// Draw all of the vertices
				Render::Display::DrawBuffer(call.texture, call.shad);
			}
			// Clear all of the draw calls so we don't draw things twice
			drawCalls.clear();
		}
	};
}