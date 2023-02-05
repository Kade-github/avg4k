#pragma once
#include "includes.h"
#include "Shader.h"
#include "Texture.h"

namespace AvgEngine::Render
{
	struct Vertex
	{
		float x = 0, y = 0;
		float u = 0, v = 0;
		float r = 1, g = 1, b = 1, a = 1;
	};

	struct Rect
	{
		float x = 0, y = 0;
		float w = 0, h = 0;
		float r = 255, g = 255, b = 255, a = 1;
	};

	class DisplayHelper
	{
	public:
		/**
		 * \brief A helper function to convert two Rect structs to a Vertex Array
		 * \param dst The destination struct
		 * \param src The source struct
		 * \return An array of Vertex's
		 */
		static std::vector<Vertex> RectToVertex(Rect dst, Rect src)
		{
			std::vector<Vertex> verts;
			Vertex tl;
			tl.x = dst.x;
			tl.y = dst.y;
			tl.u = src.x;
			tl.v = src.y;
			tl.r = dst.r / 255;
			tl.g = dst.g / 255;
			tl.b = dst.b / 255;
			tl.a = dst.a;

			Vertex bl;
			bl.x = dst.x;
			bl.y = dst.y + dst.h;
			bl.u = src.x;
			bl.v = src.y + src.h;
			bl.r = dst.r / 255;
			bl.g = dst.g / 255;
			bl.b = dst.b / 255;
			bl.a = dst.a;

			Vertex tr;
			tr.x = dst.x + dst.w;
			tr.y = dst.y;
			tr.u = src.x + src.w;
			tr.v = src.y;
			tr.r = dst.r / 255;
			tr.g = dst.g / 255;
			tr.b = dst.b / 255;
			tr.a = dst.a;

			Vertex br;
			br.x = dst.x + dst.w;
			br.y = dst.y + dst.h;
			br.u = src.x + src.w;
			br.v = src.y + src.h;
			br.r = dst.r / 255;
			br.g = dst.g / 255;
			br.b = dst.b / 255;
			br.a = dst.a;

			verts.push_back(tl);
			verts.push_back(bl);
			verts.push_back(tr);
			verts.push_back(tr);
			verts.push_back(bl);
			verts.push_back(br);
			return verts;
		}
	};

	class Display
	{
	public:

		static int width, height;

		static std::vector<Vertex> batch_buffer;
		static GLuint batch_vao;
		static GLuint batch_vbo;

		/**
		 * \brief A helper function to set the window's fullscreen property
		 * \param window The window to set
		 * \param type The type of fullscreen to set
		 */
		static void Fullscreen(GLFWwindow* window, int type)
		{
			switch (type)
			{
			case 0: // windowed
				
				break;
			case 1: // fullscreen

				break;
			case 2: // borderless
				
				break;
			default:
				Logging::writeLog("[Display] [Error] Failed to set fullscreen variable. Type not correct");
				break;
			}
		}

		/**
		 * \brief A helper function to resize the window
		 * \param window The window to resize
		 * \param newWidth The width to resize to
		 * \param newHeight The height to resize to 
		 */
		static void Resize(GLFWwindow* window, int newWidth, int newHeight)
		{
			width = newWidth;
			height = newHeight;
			
			Logging::writeLog("[Display] Resized to " + std::to_string(newWidth) + "x" + std::to_string(newHeight));
		}

		/**
		 * \brief Initializes OpenGL
		 */
		static void Init()
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			glGenVertexArrays(1, &batch_vao);
			glBindVertexArray(batch_vao);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &batch_vbo);
		}

		/**
		 * \brief Sets the current clip struct
		 * \param clip A reference to a clip struct
		 */
		static void Clip(Rect* clip)
		{
			if (clip)
			{
				glEnable(GL_SCISSOR_TEST);
				glScissor(clip->x, width - clip->h - clip->y, clip->w, clip->h);
			}
			else
				glDisable(GL_SCISSOR_TEST);
		}

		/**
		 * \brief Adds an array of vertices to the draw buffer.
		 * \param verts The vertices to add
		 */
		static void AddVertex(std::vector<Vertex> verts)
		{
			for (Vertex v : verts)
				batch_buffer.push_back(v);
		}

		/**
		 * \brief A helper function to convert two structs to their vertices
		 * \param dst The destination struct
		 * \param src The source struct
		 */
		static void AddVertex(Rect dst, Rect src)
		{
			AddVertex(DisplayHelper::RectToVertex(dst, src));
		}

		/**
		 * \brief Clear the current batch buffer
		 */
		static void ClearBuffer()
		{
			batch_buffer.clear();
		}

		/**
		 * \brief Draws the current buffer and clears it
		 * \param tex The texture to associate the buffer with
		 * \param shad The shader to associate the buffer with
		 */
		static void DrawBuffer(OpenGL::Texture* tex, OpenGL::Shader* shad)
		{
			if (batch_buffer.size() == 0)
				return;

			glBindVertexArray(batch_vao);
			glBindBuffer(GL_ARRAY_BUFFER, batch_vbo);

			shad->GL_Use();
			tex->Bind();

			//Set attribute pointers
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, x)));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, u)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, r)));

			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * batch_buffer.size(), batch_buffer.data(), GL_STATIC_DRAW);

			glDrawArrays(GL_TRIANGLES, 0, batch_buffer.size());

			batch_buffer.clear(); // clear it out
			glUseProgram(NULL);
		}

	};
}
