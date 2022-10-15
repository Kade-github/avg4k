#pragma once
#include "includes.h"
#include "Texture.h"
#include "Shader.h"

struct GL_Vertex
{
	float x = 0, y = 0;
	float u = 0, v = 0;
	float r = 1, g = 1, b = 1, a = 1;
};

struct Rect
{
	float x, y;
	float w, h;
	float r = 255, g = 255, b = 255;
	float a = 1;
};

class Rendering
{
public:

	static float rendW, rendH;

	static int drawCalls;

	static std::vector<GL_Vertex> batch_buffer;
	static GLuint batch_vao;
	static GLuint batch_vbo;
	static Shader* batch_shader;
	static Texture* batch_texture;
	static Texture* white;

	static float iBpm;
	static float iBeat;

	static void Render_GLInit(Shader* shad);

	static void drawBatch();

	static void setBlend();
	static void setBlendSep();
	static void setPremBlend();

	static void SetClipRect(Rect* clipRect);

	static void PushRectangle(Rect* rect, int stroke);

	static void PushQuad(std::vector<GL_Vertex> vertices, Texture* tex, Shader* shad);
	static void PushQuad(std::vector<GL_Vertex> vertices, Texture* tex, Shader* shad, float x, float y, float w, float h, float deg);

	static void PushQuad(Rect* dstRect, Rect* srcRect, Texture* tex, Shader* shad, float deg);
	static void PushQuad(Rect* dstRect, Rect* srcRect, Texture* tex, Shader* shad);

	static std::vector<GL_Vertex> convertSquareToTriangle(Rect r)
	{
		std::vector<GL_Vertex> verts;
		GL_Vertex tl;
		tl.x = r.x;
		tl.y = r.y;

		GL_Vertex tr;
		tr.x = r.x + r.w;
		tr.y = r.y;
		tr.u = 1;

		GL_Vertex bl;
		bl.x = r.x;
		bl.y = r.y + r.h;
		bl.v = 1;

		GL_Vertex br;
		br.x = r.x + r.w;
		br.y = r.y + r.h;
		br.u = 1;
		br.v = 1;

		verts.push_back(tl);
		verts.push_back(bl);
		verts.push_back(tr);
		verts.push_back(tr);
		verts.push_back(bl);
		verts.push_back(br);
		return verts;
	}

	static std::vector<GL_Vertex> convertSquareToTriangle(float x, float y, float x2, float y2)
	{
		std::vector<GL_Vertex> verts;
		GL_Vertex xx;
		xx.x = x;
		xx.y = y;

		GL_Vertex xx2;
		xx2.x = x2;
		xx2.y = y2;

		verts.push_back(xx);
		verts.push_back(xx2);

		return verts;
	}
};