/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <iostream>
#include <AvgEngine/Render/Display.h>
#include <AvgEngine/Render/OpenGL/Shader.h>
#include <AvgEngine/Render/OpenGL/Texture.h>
#include <glm/gtc/matrix_transform.hpp>

namespace AvgEngine::Base
{
	struct drawCall
	{
		std::string tag = "object";
		int zIndex = 0;
		Render::Rect original;
		std::vector<Render::Vertex> vertices{};
		AvgEngine::OpenGL::Texture* texture = NULL;
		OpenGL::Shader* shad = NULL;
		AvgEngine::Render::Rect clip;

		bool operator==(const drawCall& other) {
			return (zIndex == other.zIndex) &&
				(texture->id == other.texture->id) &&
				(shad->program == other.shad->program) && (clip == other.clip);
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
		static drawCall FormatDrawCall(int zIndex, AvgEngine::OpenGL::Texture* texture, OpenGL::Shader* shader, std::vector<Render::Vertex> vertices, Render::Rect original)
		{
			drawCall call;
			call.texture = texture;
			call.zIndex = zIndex;
			call.vertices = vertices;
			call.clip = {};
			call.original = original;
			return call;
		}

		bool checkIfInsideClip(Render::Rect r, Render::Rect clip)
		{
			bool checkClipGreater = clip.x > r.x + r.w;
			bool checkClipLess = clip.x + clip.w < r.x;
			bool checkClipYGreater = clip.y > r.y + r.h;
			bool checkClipYLess = clip.y + clip.h < r.y;
			return !(checkClipGreater
				|| checkClipLess
				|| checkClipYGreater
				|| checkClipYLess);
		}

		/**
		 * \brief Add a draw call (or if it already exists, add on to it)
		 * \param call The draw call struct to add
		 */
		bool addDrawCall(drawCall& call)
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
			return true;
		}

		/**
		 * \brief Render all of the current draw calls
		 */
		virtual void draw();
	};

	class Primitives
	{
	public:
		static void DrawLine(Camera* camera, int zIndex, float thickness, float x, float y, float toX, float toY, float r = 255, float g = 255, float b = 255, float a = 255)
		{
			float _x = x;
			float _x2 = x + thickness;
			float _y = y;

			float _toX = toX;
			float _toX2 = toX + thickness;
			float _toY = toY;

			std::vector<Render::Vertex> vertices;

			Render::Vertex tL = Render::Vertex(_x, _y, 0, 0,r,g,b,a);
			Render::Vertex tR = Render::Vertex(_x2, _y, 1, 0, r, g, b, a);
			Render::Vertex bL = Render::Vertex(_toX, _toY, 0, 1, r, g, b, a);
			Render::Vertex bR = Render::Vertex(_toX2, _toY, 1, 1, r, g, b, a);


			vertices.push_back(tL);
			vertices.push_back(bL);
			vertices.push_back(tR);
			vertices.push_back(tR);
			vertices.push_back(bL);
			vertices.push_back(bR);

			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, vertices, AvgEngine::Render::Rect());
			camera->addDrawCall(c);
		}

		static void DrawFilledPoints(Camera* camera, int zIndex, float tlX, float tlY, float trX, float trY, float blX, float blY, float brX, float brY, float r = 255, float g = 255, float b = 255, float a = 1, AvgEngine::Render::Rect clip = {-1,-1,-1,-1})
		{
			std::vector<Render::Vertex> vertices;

			Render::Vertex tL = Render::Vertex(tlX, tlY, 0, 0, r / 255, g / 255, b / 255, a);
			Render::Vertex tR = Render::Vertex(trX, trY, 1, 0, r / 255, g / 255, b / 255, a);
			Render::Vertex bL = Render::Vertex(blX, blY, 0, 1, r / 255, g / 255, b / 255, a);
			Render::Vertex bR = Render::Vertex(brX, brY, 1, 1, r / 255, g / 255, b / 255, a);


			vertices.push_back(tL);
			vertices.push_back(bL);
			vertices.push_back(tR);
			vertices.push_back(tR);
			vertices.push_back(bL);
			vertices.push_back(bR);

			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, vertices, AvgEngine::Render::Rect());
			if (clip.w != -1)
				c.clip = clip;
			camera->addDrawCall(c);
		}


		/**
		 * \brief An !expensive! function to draw an outlined rectangle to a camera.
		 * \param camera The camera to draw to
		 * \param zIndex The zIndex of the call
		 * \param thickness The thickness of the line
		 * \param rect The transform of the rectangle
		 */
		static void DrawOutlinedRectangle(Camera* camera, int zIndex, float thickness, Render::Rect rect, Render::Rect clip = {})
		{
			std::vector<Render::Vertex> vertices;

			Render::Rect topLine = { rect.x, rect.y, rect.w, thickness, rect.r, rect.g, rect.b, rect.a };
			Render::Rect bottomLine = { rect.x, rect.y + rect.h, rect.w, thickness, rect.r, rect.g, rect.b, rect.a };
			Render::Rect leftLine = { rect.x, rect.y, thickness, rect.h, rect.r, rect.g, rect.b, rect.a };
			Render::Rect rightLine = { rect.x + rect.w, rect.y, thickness, rect.h + thickness, rect.r,rect.g,rect.b, rect.a };

			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(topLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(bottomLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(leftLine, { 0,0,1,1 }))
				vertices.push_back(v);
			for (Render::Vertex v : Render::DisplayHelper::RectToVertex(rightLine, { 0,0,1,1 }))
				vertices.push_back(v);

			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, vertices, topLine);
			if (clip != Render::Rect{})
				c.clip = clip;
			camera->addDrawCall(c);
		}

		/**
		 * \brief A function to draw a rectangle to a camera.
		 * \param camera The camera to draw to
		 * \param zIndex The zIndex of the call
		 * \param rect The transform of the rectangle
		 */
		static void DrawRectangle(Camera* camera, int zIndex, Render::Rect rect, Render::Rect clip = {})
		{
			drawCall c = Camera::FormatDrawCall(zIndex, NULL, NULL, Render::DisplayHelper::RectToVertex(rect, { 0,0,1,1 }), rect);
			if (clip != Render::Rect{})
				c.clip = clip;
			camera->addDrawCall(c);
		}
	};
}

#endif