#pragma once
#ifndef AVG_CAMERA_H
#define AVG_CAMERA_H

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
		Camera(int _w, int _h)
		{
			resize(_w, _h);
		}

		glm::mat4 projection{};

		/**
		 * \brief Resize the projection matrix to the set width and height
		 * \param w The width to resize to
		 * \param h The height to resize to
		 */
		void resize(float _w, float _h)
		{
			w = _w;
			h = _h;
			projection = glm::ortho(0.0f, _w, _h, 0.0f, -1.0f, 1.0f);
		}

		/**
		 * \brief A helper function to format draw calls
		 * \param zIndex The zIndex of the draw call
		 * \param texture The texture of the draw call
		 * \param shader The shader of the draw call
		 * \param vertices The vertices of the draw call
		 * \return The formatted draw call struct
		 */
		static drawCall FormatDrawCall(int zIndex, OpenGL::Texture* texture, OpenGL::Shader* shader, std::vector<Render::Vertex> vertices)
		{
			drawCall call;
			call.texture = texture;
			call.zIndex = zIndex;
			call.vertices = vertices;
			return call;
		}

		/**
		 * \brief Add a draw call (or if it already exists, add on to it)
		 * \param call The draw call struct to add
		 */
		void addDrawCall(drawCall& call)
		{
			if (call.texture == NULL)
				call.texture = OpenGL::Texture::returnWhiteTexture();
			if (call.shad == NULL)
				call.shad = Render::Display::defaultShader;
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

			for(drawCall& call : drawCalls)
			{
				// Clear the buffer so we know nothing is there
				Render::Display::ClearBuffer();
				// Add our call's vertices
				Render::Display::AddVertex(call.vertices);
				// Draw all of the vertices
				Render::Display::DrawBuffer(call.texture, call.shad);
			}
			
			// Clear all of the draw calls so we don't draw things twice
			drawCalls.clear();
		}
	};

	class Primitives
	{
	public:
		/**
		 * \brief An !expensive! function to draw an outlined rectangle to a camera.
		 * \param camera The camera to draw to
		 * \param zIndex The zIndex of the call
		 * \param thickness The thickness of the line
		 * \param rect The transform of the rectangle
		 */
		static void DrawOutlinedRectangle(Camera* camera, int zIndex, float thickness, Render::Rect rect)
		{
			std::vector<Render::Vertex> vertices;

			Render::Rect topLine = { rect.x, rect.y, rect.w, thickness, rect.r, rect.g, rect.b, rect.a };
			Render::Rect bottomLine = { rect.x, rect.y + rect.h, rect.w, thickness, rect.r, rect.g, rect.b, rect.a };
			Render::Rect leftLine = { rect.x, rect.y, thickness, rect.h, rect.r, rect.g, rect.b, rect.a };
			Render::Rect rightLine = { rect.x + rect.w, rect.y, thickness, rect.h, rect.r,rect.g,rect.b, rect.a };

			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(topLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(bottomLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(leftLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(rightLine, { 0,0,1,1 }))
				vertices.push_back(v);

			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, vertices);
			camera->addDrawCall(c);
		}

		/**
		 * \brief A function to draw a rectangle to a camera.
		 * \param camera The camera to draw to
		 * \param zIndex The zIndex of the call
		 * \param rect The transform of the rectangle
		 */
		static void DrawRectangle(Camera* camera, int zIndex, Render::Rect rect)
		{
			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, Render::DisplayHelper::RectToVertex(rect, { 0,0,1,1 }));
			camera->addDrawCall(c);
		}
	};
}

#endif