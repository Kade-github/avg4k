/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <Glad/glad.h>

#include <AvgEngine/Render/OpenGL/Shader.h>
using namespace AvgEngine::OpenGL;

#ifndef SHADER_CPP
#define SHADER_CPP

int Shader::GetUniformLocation(std::string loc)
{
	//Use previously defined location
	auto key_loc = uniform_map.find(loc);
	if (key_loc != uniform_map.end())
		return key_loc->second;

	//Find and remember location
	GLint uniform_loc = glGetUniformLocation(program, loc.c_str());
	if (uniform_loc == -1)
	{
		return -1;
	}
	uniform_map[loc] = uniform_loc;
	return uniform_loc;
}

void Shader::GL_Use()
{
	glUseProgram(program);
}

Shader::~Shader()
{
	glDeleteProgram(this->program);
	glDeleteShader(this->vertex);
	glDeleteShader(this->fragment);
}

void Shader::GL_CompileShader(const char* src_vert, const char* src_frag)
{

	glDeleteProgram(this->program);
	glDeleteShader(this->vertex);
	glDeleteShader(this->fragment);
	// defaults
	if (src_vert == NULL)
		src_vert = generic_shader_vert;
	if (src_frag == NULL)
		src_frag = generic_shader_frag;

	// add precompiled headers

	std::string srcVertStr = std::string(src_vert);
	std::string srcFragStr = std::string(src_frag);
	std::string precompiled = "#version 150 core\nuniform float iTime;\n";
	std::string vertTogether = precompiled + srcVertStr;
	std::string fragTogether = precompiled + srcFragStr;
	const char* srcVert = vertTogether.c_str();
	const char* srcFrag = fragTogether.c_str();

	//Create shader
	GLint shader_status;
	program = glCreateProgram();

	//Compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &srcVert, NULL);
	glCompileShader(this->vertex);

	glGetShaderiv(this->vertex, GL_COMPILE_STATUS, &shader_status);


	//Compile fragment shader
	this->fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragment, 1, &srcFrag, NULL);
	glCompileShader(this->fragment);

	glGetShaderiv(this->fragment, GL_COMPILE_STATUS, &shader_status);

	//Attach and link
	glAttachShader(this->program, this->vertex);
	glAttachShader(this->program, this->fragment);

	glBindAttribLocation(this->program, 0, "v_position");
	glBindAttribLocation(this->program, 1, "v_uv");
	glBindAttribLocation(this->program, 2, "v_colour");

	glLinkProgram(this->program);

	glGetProgramiv(this->program, GL_LINK_STATUS, &shader_status);

}

void AvgEngine::OpenGL::Shader::uniform1i(int loc_id, int i0)
{
	GL_Use();
	glUniform1i(loc_id, i0);
}

void AvgEngine::OpenGL::Shader::uniform1f(int loc_id, float f0)
{
	GL_Use();
	glUniform1f(loc_id, f0);
}

void AvgEngine::OpenGL::Shader::uniform2i(int loc_id, int i0, int i1)
{
	GL_Use();
	glUniform2i(loc_id, i0, i1);
}

void AvgEngine::OpenGL::Shader::uniform2f(int loc_id, float f0, float f1)
{
	GL_Use();
	glUniform2f(loc_id, f0, f1);
}

void AvgEngine::OpenGL::Shader::uniform3i(int loc_id, int i0, int i1, int i2)
{
	GL_Use();
	glUniform3i(loc_id, i0, i1, i2);
}

void AvgEngine::OpenGL::Shader::uniform3f(int loc_id, float f0, float f1, float f2)
{
	GL_Use();
	glUniform3f(loc_id, f0, f1, f2);
}

void AvgEngine::OpenGL::Shader::uniform4i(int loc_id, int i0, int i1, int i2, int i3)
{
	GL_Use();
	glUniform4i(loc_id, i0, i1, i2, i3);
}

void AvgEngine::OpenGL::Shader::uniform4f(int loc_id, float f0, float f1, float f2, float f3)
{
	GL_Use();
	glUniform4f(loc_id, f0, f1, f2, f3);
}

void AvgEngine::OpenGL::Shader::setProject(glm::mat4 proj)
{
	GL_Use();
	glUniformMatrix4fv(glGetUniformLocation(this->program, "u_projection"), 1, GL_FALSE, &proj[0][0]);
}

#endif // !SHADER_CPP