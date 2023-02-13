#pragma once
#include "FntParser.h"
#include "includes.h"
#include "GameObject.h"
namespace AvgEngine::Base
{
	class Text : public GameObject
	{
	public:
		Fnt::Fnt* fnt;

		bool wrap = false;

		float size = 0;
		float outlineThickness = 0;

		std::string text = "";

		Text(int x, int y, std::string folder, std::string font, std::string _text, float _size) : GameObject(x,y)
		{
			SetFont(folder, font);
			SetSize(_size);
			SetText(_text);
		}

		~Text()
		{
			if (fnt)
				delete fnt;
			
		}

		void SetFont(std::string folder, std::string font)
		{
			if (fnt)
				delete fnt;
			fnt = Fnt::Fnt::GetFont(folder, font);
			#ifdef _DEBUG
			Logging::writeLog("[Fnt] [Debug] Set " + font + " to path " + folder + " successfully.");
			#endif
		}

		void SetSize(float _size)
		{
			size = _size;
		}

		void SetText(std::string _text)
		{
			text = _text;
		}

		void draw() override
		{
			Render::Rect dst = transform;
			if (transformRatio)
			{
				dst.x = parent->x * dst.x;
				dst.y = parent->y * dst.y;
			}

			drawCall call;
			call.texture = fnt->texture;
			call.shad = NULL;
			call.zIndex = zIndex;
			call.vertices = {};
			int totalW = 0;
			int highestH = 0;
			float scale = size / fnt->ogSize;
			for(char ch : text)
			{
				const Fnt::FntChar c = fnt->GetChar(ch);
				if (c.id == -1)
					continue;
				float advance = c.xAdvance * scale;
				if (ch == 32)
				{
					dst.x += advance;
					totalW += advance;
					continue;
				}
				Render::Rect src = c.src;
				dst.w = c.dst.w * scale;
				dst.h = c.dst.h * scale;


				if (highestH < dst.h)
					highestH = dst.h;

				if (outlineThickness != 0)
				{
					Render::Rect outline = dst;
					float outlineScale = (outlineThickness / 1);
					float nX = (dst.w * (1 - outlineScale)) / 2;
					outline.x += nX;
					outline.y += (dst.h * (1 - outlineScale)) / 2;
					outline.w *= outlineScale;
					outline.h *= outlineScale;
					outline.r = 0;
					outline.g = 0;
					outline.b = 0;
					std::vector<Render::Vertex> newVert = Render::DisplayHelper::RectToVertex(outline, src);

					call.vertices.insert(call.vertices.end(), newVert.begin(), newVert.end());
				}
				std::vector<Render::Vertex> newVert = Render::DisplayHelper::RectToVertex(dst, src);

				call.vertices.insert(call.vertices.end(), newVert.begin(), newVert.end());

				dst.x += advance;
				if (wrap && dst.x + dst.w > parent->w && wrap)
				{
					dst.x = transform.x;
					dst.y += highestH;
				}
				totalW += advance;
			}
			transform.w = totalW;
			transform.h = highestH;

			camera->addDrawCall(call);

			GameObject::draw();
		}
	};
}