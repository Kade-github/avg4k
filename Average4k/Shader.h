#pragma once
#include "includes.h"

static const char* generic_shader_vert = R"(
in vec2 v_position;
in vec2 v_uv;
in vec4 v_colour;
out vec2 f_uv;
out vec4 f_colour;
uniform mat4 u_projection;

void main()
{
	f_uv = v_uv;
	f_colour = v_colour;
	gl_Position = u_projection * vec4(v_position.xy, 0.0, 1.0);
})";
static const char* generic_shader_frag = R"(
uniform sampler2D u_texture;
uniform float white;
in vec2 f_uv;
in vec4 f_colour;

out vec4 o_colour;
void main()
{
	o_colour = texture(u_texture, f_uv) * f_colour;
	if (o_colour.a == 0.0)
		discard;
	if (white >= 0.9)
		o_colour = vec4(1,1,1,o_colour.a);
		
})";

class Shader {
public:
	GLuint program = 0;
	GLuint vertex = 0;
	GLuint fragment = 0;

	std::unordered_map<std::string, GLint> uniform_map;

	Shader() {

	}

	Shader(std::string vertFile, std::string fragFile)
	{
		std::ifstream vertF(vertFile);
		std::ifstream fragF(vertFile);

		if (!vertF.is_open() || !fragF.is_open())
		{
			std::cout << "Failed to open " << vertFile << " and " << fragFile << std::endl;
			return;
		}

		std::string line;

		std::string vert;
		std::string frag;

		while (getline(vertF, line))
			vert += line;
		while (getline(fragF, line))
			frag += line;

		vertF.close();
		fragF.close();

		GL_CompileShader(vert.c_str(), frag.c_str());
	}

	~Shader() {
		glDeleteProgram(this->program);
		glDeleteShader(this->vertex);
		glDeleteShader(this->fragment);
	}

	void GL_Use()
	{
		glUseProgram(program);
	}

	GLint GetUniformLocation(std::string loc)
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

	#define SETUNIFORM_GET_LOC_ID() \
			GLint loc_id = GetUniformLocation(loc); \
			if (loc_id == -1) \
				return true;

	bool SetUniform(std::string loc, int i0)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform1i(loc_id, i0);
		return false;
	}

	bool SetUniform(std::string loc, int i0, int i1)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform2i(loc_id, i0, i1);
		return false;
	}

	bool SetUniform(std::string loc, int i0, int i1, int i2)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform3i(loc_id, i0, i1, i2);
		return false;
	}

	bool SetUniform(std::string loc, int i0, int i1, int i2, int i3)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform4i(loc_id, i0, i1, i2, i3);
		return false;
	}

	bool SetUniform(std::string loc, float f0)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform1f(loc_id, f0);
		return false;
	}

	bool SetUniform(std::string loc, float f0, float f1)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform2f(loc_id, f0, f1);
		return false;
	}

	bool SetUniform(std::string loc, float f0, float f1, float f2)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform3f(loc_id, f0, f1, f2);
		return false;
	}

	bool SetUniform(std::string loc, float f0, float f1, float f2, float f3)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniform4f(loc_id, f0, f1, f2, f3);
		return false;
	}

	bool SetUniform(std::string loc, int count, float* ptr)
	{
		SETUNIFORM_GET_LOC_ID()
		glUniformMatrix4fv(loc_id, count, GL_FALSE, ptr);
		return false;
	}

	void GL_CompileShader(const char* src_vert, const char* src_frag)
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
			std::cout << buffer << std::endl;
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
			std::cout << buffer << std::endl;
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
			std::cout << buffer << std::endl;
		}
		std::cout << "Created shader " << program << std::endl;
	}

	void setProject(glm::mat4 proj)
	{
		GL_Use();
		glUniformMatrix4fv(glGetUniformLocation(this->program, "u_projection"), 1, GL_FALSE, &proj[0][0]);
	}
};