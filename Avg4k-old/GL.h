#pragma once
#include "includes.h"
#include "Rendering.h"
#include "Texture.h"
#include "Shader.h"
struct Color {
	int r;
	int g;
	int b;
};

using namespace glm;
class GL
{
public:
	static Shader* genShader;
	static mat4 projection;

};