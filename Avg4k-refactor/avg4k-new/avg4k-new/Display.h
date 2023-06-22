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
		float scale = 1;
		float angle = 0;

		Rect() = default;
		Rect(float _x, float _y, float _w, float _h)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}

		Rect(float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
		Rect(float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a, float _s, float _d)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			r = _r;
			g = _g;
			b = _b;
			a = _a;
			scale = _s;
			angle = _d;
		}

		Rect(float _x, float _y, Rect _r)
		{
			x = _x;
			y = _y;
			w = _r.w;
			h = _r.h;
			r = _r.r;
			g = _r.g;
			b = _r.b;
			a = _r.a;
		}

		bool operator==(const Rect& other) {
			// == with tolerance
			return (std::abs(x - other.x) < 0.001f) && (std::abs(y - other.y) < 0.001f) && (std::abs(w - other.w) < 0.001f) && (std::abs(h - other.h) < 0.001f) && (std::abs(r - other.r) < 0.001f) && (std::abs(g - other.g) < 0.001f) && (std::abs(b - other.b) < 0.001f) && (std::abs(a - other.a) < 0.001f);

		}

		bool operator!=(const Rect& other) {
			// != with tolerance
			return (std::abs(x - other.x) > 0.001f) || (std::abs(y - other.y) > 0.001f) || (std::abs(w - other.w) > 0.001f) || (std::abs(h - other.h) > 0.001f) || (std::abs(r - other.r) > 0.001f) || (std::abs(g - other.g) > 0.001f) || (std::abs(b - other.b) > 0.001f) || (std::abs(a - other.a) > 0.001f);

		}

		std::string toString() {
			return "XYWH {" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(w) + "," + std::to_string(h) + "} RGBA {" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + "}";
		}
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
		static std::vector<Vertex> RectToVertex(Rect dst, Rect src, bool center = false)
		{
			std::vector<Vertex> verts;

			Rect rD = dst;
			if (center)
			{
				float mpx = (rD.w * (1 - rD.scale)) / 2;
				float mpy = (rD.h * (1 - rD.scale)) / 2;
				rD.x += mpx;
				rD.y += mpy;
			}

			rD.w *= rD.scale;
			rD.h *= rD.scale;


			Vertex tl;
			tl.x = rD.x;
			tl.y = rD.y;
			tl.u = src.x;
			tl.v = src.y;
			tl.r = rD.r / 255;
			tl.g = rD.g / 255;
			tl.b = rD.b / 255;
			tl.a = rD.a;

			Vertex bl;
			bl.x = rD.x;
			bl.y = rD.y + rD.h;
			bl.u = src.x;
			bl.v = src.y + src.h;
			bl.r = rD.r / 255;
			bl.g = rD.g / 255;
			bl.b = rD.b / 255;
			bl.a = rD.a;

			Vertex tr;
			tr.x = rD.x + rD.w;
			tr.y = rD.y;
			tr.u = src.x + src.w;
			tr.v = src.y;
			tr.r = rD.r / 255;
			tr.g = rD.g / 255;
			tr.b = rD.b / 255;
			tr.a = rD.a;

			Vertex br;
			br.x = rD.x + rD.w;
			br.y = rD.y + rD.h;
			br.u = src.x + src.w;
			br.v = src.y + src.h;
			br.r = rD.r / 255;
			br.g = rD.g / 255;
			br.b = rD.b / 255;
			br.a = rD.a;

			if (rD.angle != 0)
			{
				float s = sin(rD.angle * (3.14159265 / 180));
				float c = cos(rD.angle * (3.14159265 / 180));
				float cx = rD.x + rD.w * 0.5f;
				float cy = rD.y + rD.h * 0.5f;

				Vertex* verts[] = { &tl, &bl, &tr, &br };
				for (Vertex* vert : verts)
				{
					float tx = vert->x - cx;
					float ty = vert->y - cy;
					float rx = tx * c - ty * s;
					float ry = tx * s + ty * c;
					vert->x = rx + cx;
					vert->y = ry + cy;
				}
			}

			verts.push_back(tl);
			verts.push_back(bl);
			verts.push_back(tr);
			verts.push_back(tr);
			verts.push_back(bl);
			verts.push_back(br);
			return verts;
		}

		static int* getMonitorResolution()
		{
			static int* stored = nullptr;

			if (stored != nullptr)
				return stored;
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			stored = new int[2] {mode->width, mode->height};
			return stored;
		}
	};


	class Display
	{
	public:

		static OpenGL::Shader* defaultShader;

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
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			int* max_res = DisplayHelper::getMonitorResolution();
			switch (type)
			{
			case 0: // windowed
				glfwSetWindowAttrib(window, GLFW_DECORATED, 1);
				glfwSetWindowAttrib(window, GLFW_FLOATING, 0);
				glfwSetWindowMonitor(window, NULL, (max_res[0] / 2) - width / 2, (max_res[1] / 2) - height / 2, width, height, GLFW_DONT_CARE);
				break;
			case 1: // fullscreen
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
				break;
			case 2: // borderless
				glfwSetWindowAttrib(window, GLFW_DECORATED, 0);
				glfwSetWindowAttrib(window, GLFW_FLOATING, 1);
				width = max_res[0];
				height = max_res[1];
				glfwSetWindowMonitor(window, NULL, 0, 0, max_res[0], max_res[1], GLFW_DONT_CARE);
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

			int* max_res = DisplayHelper::getMonitorResolution();

			width = newWidth;
			height = newHeight;

			if (width >= max_res[0])
				width = max_res[0];
			if (height >= max_res[1])
				height = max_res[1];

			glfwSetWindowMonitor(window, NULL, (max_res[0] / 2) - (width / 2), (max_res[1] / 2) - (height / 2), width, height, GLFW_DONT_CARE);

			Logging::writeLog("[Display] Resized to " + std::to_string(width) + "x" + std::to_string(height));
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

			defaultShader = new OpenGL::Shader();
			defaultShader->GL_CompileShader(NULL, NULL);
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
				glScissor(clip->x, height - clip->h - clip->y, clip->w, clip->h);
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
