#pragma once
#include "includes.h"
#include "Rendering.h"
#include "Texture.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
class GL
{
public:
	static Shader* genShader;
	static mat4 projection;
};