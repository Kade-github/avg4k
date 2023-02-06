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

		float size = 0;
		float outlineThickness = 0;

		std::string text = "";

		Text(int x, int y, std::string folder, std::string font, std::string _text, float _size) : GameObject(x,y)
		{
			fnt = Fnt::Fnt::GetFont(folder, font);
			SetSize(_size);
			SetText(_text);
		}

		~Text()
		{
			if (fnt)
				delete fnt;
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
			drawCall call;
			call.texture = fnt->texture;
			call.shad = NULL;
			call.zIndex = zIndex;
			call.vertices = {};
			Render::Rect dst = transform;
			int totalW = 0;
			int highestH = 0;
			float scale = size / fnt->ogSize;
			for(char ch : text)
			{
				Fnt::FntChar c = fnt->GetChar(ch);
				if (c.id == -1)
					continue;
				Render::Rect src = c.src;
				dst.w = c.dst.w * scale;
				dst.h = c.dst.h * scale;

				float advance = c.xAdvance * scale;

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
					for (Render::Vertex v : Render::DisplayHelper::RectToVertex(outline, src))
						call.vertices.push_back(v);
				}
				for (Render::Vertex v : Render::DisplayHelper::RectToVertex(dst, src))
					call.vertices.push_back(v);

				dst.x += advance;
				totalW += advance;
			}
			transform.w = totalW;
			transform.h = highestH;

			camera->addDrawCall(call);
		}
	};
}