#pragma once
#include "includes.h"
#include "Texture.h"
#include "Shader.h"

typedef struct
{
	float x, y;
	float u, v;
	float r, g, b, a;
} GL_Vertex;

struct Rect
{
	float x, y;
	float w, h;
	float r, g, b;
	float a;
};

class Rendering
{
public:
	static std::vector<GL_Vertex> batch_buffer;
	static GLuint batch_vao;
	static GLuint batch_vbo;
	static Shader* batch_shader;
	static Texture* batch_texture;
	static Texture* white;



	static void Render_GLInit(Shader* shad);

	static void drawBatch();

	static void setBlend();
	static void setBlendSep();
	static void setPremBlend();

	static void SetClipRect(Rect* clipRect);

	static void PushQuad(Rect* dstRect, Rect* srcRect, Texture* tex, Shader* shad, float deg);
	static void PushQuad(Rect* dstRect, Rect* srcRect, Texture* tex, Shader* shad);
};