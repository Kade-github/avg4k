/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <AvgEngine/Render/Display.h>

#include <Glad/glad.h>

using namespace AvgEngine;

#ifndef DISPLAY_CPP
#define DISPLAY_CPP

int AvgEngine::Render::Display::height = 0;
int AvgEngine::Render::Display::width = 0;

std::vector<AvgEngine::Render::Vertex> AvgEngine::Render::Display::batch_buffer{};
GLuint AvgEngine::Render::Display::batch_vao{};
GLuint AvgEngine::Render::Display::batch_vbo{};

OpenGL::Shader* Render::Display::defaultShader = NULL;

void AvgEngine::Render::Display::Init()
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

void AvgEngine::Render::Display::DrawBuffer(std::shared_ptr<OpenGL::Texture> tex, OpenGL::Shader* shad)
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

#endif // !DISPLAY_CPP