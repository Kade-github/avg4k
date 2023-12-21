#include "Shader.h"
using namespace AvgEngine::OpenGL;

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
	std::string precompiled = "#version 150 core\nuniform float iTime;\nuniform float iBpm;\nuniform float iBeat;\n";
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
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetShaderInfoLog(this->vertex, sizeof(buffer), NULL, buffer);
		Logging::writeLog("[Shader] [Error] Failure to create shader. Error: " + std::string(buffer));
	}

	//Compile fragment shader
	this->fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragment, 1, &srcFrag, NULL);
	glCompileShader(this->fragment);

	glGetShaderiv(this->fragment, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetShaderInfoLog(this->fragment, sizeof(buffer), NULL, buffer);
		Logging::writeLog("[Shader] [Error] Failure to create shader. Error: " + std::string(buffer));
	}

	//Attach and link
	glAttachShader(this->program, this->vertex);
	glAttachShader(this->program, this->fragment);

	glBindAttribLocation(this->program, 0, "v_position");
	glBindAttribLocation(this->program, 1, "v_uv");
	glBindAttribLocation(this->program, 2, "v_colour");

	glLinkProgram(this->program);

	glGetProgramiv(this->program, GL_LINK_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetProgramInfoLog(this->program, sizeof(buffer), NULL, buffer);
		Logging::writeLog("[Shader] [Error] Failure to create shader. Error: " + std::string(buffer));
	}
}