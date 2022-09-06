#pragma once
#include "includes.h"
#include "Object.h"
#include "GL.h"
#include "Game.h"
#include "AvgFrame.h"

class AvgSprite : public Object
{
private:
	float animStartTime = 0;
	float animTime = 0;
public:
	Texture* tex;

	AvgSparrow* sparrow;
	int frame;
	int fps;

	bool loop;
	bool animationFinished;

	int borderSize = 4;
	//Color borderColor;
	bool border = false;
	bool round = false;
	bool flip = false;
	bool flipX = false;
	bool deleteShader = true;
	Shader* customShader;
	std::string filePath;

	bool drawCall = true;

	int colorR = 255, colorG = 255, colorB = 255;

	Rect clipRect;

	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}

	AvgSprite(int _x, int _y, std::string path) : Object(x, y) {
		MUTATE_START
		x = _x;
		y = _y;
		tex = Texture::createWithImage(path);
		path = filePath;
		if (!tex)
			std::cout << "failed to get texture!" << std::endl;
		w = tex->width;
		h = tex->height;
		alpha = 1;
		MUTATE_END
	};

	AvgSprite(int _x, int _y, Texture* data) : Object(x, y) {
		MUTATE_START
		x = _x;
		y = _y;
		tex = data;
		if (!tex) 
			std::cout << "failed to get texture!" << std::endl;
		w = tex->width;
		h = tex->height;
		alpha = 1;
		MUTATE_END
	};

	virtual ~AvgSprite()
	{
		for (Object* obj : children)
		{
			delete obj;
		}
		if (customShader && deleteShader)
			delete customShader;
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

	void setSparrow(std::string xml)
	{
		sparrow = new AvgSparrow(xml, tex->width, tex->height);
		AvgFrame firstFrame = sparrow->animations.begin()->second.frames[0];
		w = firstFrame.frameRect.w;
		h = firstFrame.frameRect.h;
	}

	void playAnim(std::string name, int _fps, bool _loop)
	{
		animTime = 0;
		animationFinished = false;
		sparrow->playAnim(name);
		fps = _fps;
		loop = _loop;
	}

	virtual void draw() {
		if (!drawCall)
			return;

		Rect dstRect;
		Rect srcRect;
		float uX = x, uY = y;

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

		if (sparrow)
		{
			if (!animationFinished || loop)
			{
				animTime += Game::deltaTime;
				frame = (animTime * fps / 1000);
				int size = sparrow->animations[sparrow->currentAnim].frames.size();
				if (frame > size - 1)
				{
					if (!loop)
					{
						animationFinished = true;
						frame = size - 1;
					}
					else
					{
						animTime = 0;
					}
				}
				if (frame > size - 1)
					frame = 0;
			}
			AvgFrame fr = sparrow->getRectFromFrame(frame);
			srcRect = fr.srcRect;
			dstRect.x = uX + ((fr.frameRect.x * scale) + mpx);
			dstRect.y = uY + ((fr.frameRect.y * scale) + mpy);

			dstRect.w = (int)(fr.frameRect.w * scale);
			dstRect.h = (int)(fr.frameRect.h * scale);
			//Game::mainCamera->clipRect = dstRect;
		}

		srcRect.x = 0;
		srcRect.y = 0;
		if (flipX)
			srcRect.w = -1;
		else
			srcRect.w = 1;
		if (flip)
			srcRect.h = -1;
		else
			srcRect.h = 1;

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		if (!customShader)
			Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader, angle);
		else
		{
			Rendering::drawBatch();
			Rendering::PushQuad(&dstRect, &srcRect, tex, customShader, angle);
		}

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);

		if (customShader)
			Rendering::drawBatch();
	}

	virtual void beforeDeath() {
		if (!tex->dontDelete)
			delete tex;
	}
};


