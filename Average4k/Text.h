#pragma once
#include "Object.h"
#include "includes.h"
#include "Game.h"
#include "Font.h"
#include "GL.h"
#include "Texture.h"

static const char* textFrag = R"(
#version 150 core
uniform sampler2D u_texture;
in vec2 f_uv;
in vec4 f_colour;

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

out vec4 o_colour;
void main()
{
	o_colour = texture(u_texture, f_uv) * f_colour;
	o_colour = vec4(hsv2rgb(vec3(gl_FragCoord.x * 5.0, 1.0, 1.0)), o_colour.a);
})";

class Text : public Object
{
public:
	TTF_Font* Arial;
	TTF_Font* outlineArial;
	Texture* message;
	Texture* outline;

	int borderSize = 1;
	float borderW, borderH;
	Color borderColor;
	float borderAlpha = 1;

	Shader* getShader()
	{
		static Shader* textShader = nullptr;
		if (!textShader)
		{
			textShader = new Shader();
			textShader->GL_CompileShader(NULL, textFrag);
		}
		return textShader;
	}

	std::string font;

	Text(int x, int y, std::string temp, int _size, std::string fontName) : Object(x, y) {
		Arial = Font::getFontByName(fontName,_size);
		TTF_SetFontHinting(Arial, TTF_HINTING_LIGHT);
		this->text = temp;
		this->w = w;
		this->h = h;
		this->rW = w;
		this->rH = h;
		size = _size;
		font = fontName;
		border = false;
		scale = 1;
		alpha = 1;

		color = { 255,255,255 };
		
		setText(temp);
	};

	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}

	virtual ~Text()
	{
		beforeDeath();
		die();
	}

	void setCharacterSpacing(float spacing);

	int size;

	void draw() override;
	void die() override;
	void centerX();
	void centerY();
	Color color;

	std::string text;
	int w;
	int h;

	int addW;
	int addH;

	bool border = true;

	int surfW;
	int surfH;

	int rW;
	int rH;

	double scale;
	void setFont(std::string name);
	void forceDraw();

	void setText(std::string _text);
};

