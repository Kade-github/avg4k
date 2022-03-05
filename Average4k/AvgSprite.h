#pragma once
#include "includes.h"
#include "Object.h"
#include "GL.h"
#include "Game.h"

class AvgSprite : public Object
{
public:
	Texture* tex;

	int borderSize = 4;
	//Color borderColor;
	bool border = false;
	bool round = false;
	bool flip = false;

	bool drawCall = true;

	int colorR = 255, colorG = 255, colorB = 255;

	Rect clipRect;

	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}

	AvgSprite(int _x, int _y, std::string path) : Object(x, y) {
		x = _x;
		y = _y;
		tex = Texture::createWithImage(path);
		if (!tex)
			std::cout << "failed to get texture!" << std::endl;
		w = tex->width;
		h = tex->height;
		alpha = 1;
	};

	AvgSprite(int _x, int _y, Texture* data) : Object(x, y) {
		x = _x;
		y = _y;
		tex = data;
		if (!tex)
			std::cout << "failed to get texture!" << std::endl;
		w = tex->width;
		h = tex->height;
		alpha = 1;
	};

	virtual ~AvgSprite()
	{
		for (Object* obj : children)
		{
			delete obj;
		}
		beforeDeath();
		die();
	}

	void changeOutTexture(std::string path)
	{
		delete tex;
		tex = Texture::createWithImage(path);
		w = tex->width;
		h = tex->height;
	}

	virtual void draw() {
		if (!drawCall)
			return;
		Rect dstRect;
		Rect srcRect;


		float mpx = (w * (1 - scale)) / 2;
		float mpy = (h * (1 - scale)) / 2;
		dstRect.x = x + mpx;
		dstRect.y = y + mpy;
		if (!staticView)
		{
			dstRect.x -= Game::mainView.x;
			dstRect.y -= Game::mainView.y;
		}
		dstRect.w = w * scale;
		dstRect.h = h * scale;
		dstRect.r = colorR;
		dstRect.g = colorG;
		dstRect.b = colorB;
		dstRect.a = alpha;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		if (flip)
			srcRect.h = -1;
		else
			srcRect.h = 1;

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader, angle);

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);
	}

	virtual void beforeDeath() {
		if (!tex->dontDelete)
			delete tex;
	}
};


