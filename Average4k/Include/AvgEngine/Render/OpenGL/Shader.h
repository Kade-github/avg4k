/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef SHADER_H
#define SHADER_H

#pragma once

#include <Glad/glad.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <Glm/gtc/matrix_transform.hpp>

namespace AvgEngine::OpenGL
{
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

			/**
			 * \brief The OpenGL Shader Class
			 */
			class Shader {
			private:
				void uniform1i(int loc_id, int i0);

				void uniform1f(int loc_id, float f0);

				void uniform2i(int loc_id, int i0, int i1);

				void uniform2f(int loc_id, float f0, float f1);

				void uniform3i(int loc_id, int i0, int i1, int i2);

				void uniform3f(int loc_id, float f0, float f1, float f2);

				void uniform4i(int loc_id, int i0, int i1, int i2, int i3);

				void uniform4f(int loc_id, float f0, float f1, float f2, float f3);
			public:

				bool fromLua = false;
				GLuint program = 0;
				GLuint vertex = 0;
				GLuint fragment = 0;

				std::unordered_map<std::string, GLint> uniform_map;

				Shader() {

				}

				Shader(std::string vertFile, std::string fragFile, bool lua = false)
				{
					fromLua = lua;
					std::ifstream vertF(vertFile);
					std::ifstream fragF(fragFile);

					std::string vert;
					std::string frag;

					bool useDefualtVert = vertFile == "n";
					bool useDefualtFrag = fragFile == "n";

					if (!vertF.is_open() || !fragF.is_open())
					{
						if (!useDefualtVert && !useDefualtFrag)
						{
							return;
						}
					}

					std::string line;

					if (!useDefualtVert)
					{
						while (std::getline(vertF, line))
							vert += line;
						vertF.close();
					}
					if (!useDefualtFrag)
					{
						while (std::getline(fragF, line))
							frag += line;
						fragF.close();
					}


					GL_CompileShader(useDefualtVert ? generic_shader_vert : vert.c_str(), useDefualtFrag ? generic_shader_frag : frag.c_str());
				}

				~Shader();

				void GL_Use();

				GLint GetUniformLocation(std::string loc);

				#define SETUNIFORM_GET_LOC_ID() \
						GLint loc_id = GetUniformLocation(loc); \
						if (loc_id == -1) \
							return true;

				bool SetUniform(std::string loc, int i0)
				{
					SETUNIFORM_GET_LOC_ID()
						uniform1i(loc_id, i0);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform2i(loc_id, i0, i1);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1, int i2)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform3i(loc_id, i0, i1, i2);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1, int i2, int i3)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform4i(loc_id, i0, i1, i2, i3);
					return false;
				}

				bool SetUniform(std::string loc, float f0)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform1f(loc_id, f0);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform2f(loc_id, f0, f1);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1, float f2)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform3f(loc_id, f0, f1, f2);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1, float f2, float f3)
				{
					SETUNIFORM_GET_LOC_ID()
					uniform4f(loc_id, f0, f1, f2, f3);
					return false;
				}

				void GL_CompileShader(const char* src_vert, const char* src_frag);

				void setProject(glm::mat4 proj);
			};
}

#endif // !SHADER_H