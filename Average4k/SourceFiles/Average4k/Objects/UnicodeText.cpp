#include "UnicodeText.h"

void Average4k::Objects::UnicodeText::draw()
{
	using namespace AvgEngine::Base;
	using namespace AvgEngine::Render;
	using namespace AvgEngine::Fnt;

	if (!fnt)
		return;
	Rect dst = transform;
	Rect cr = clipRect;
	if (parent)
	{
		if (transformRatio)
		{

			float xRatio = (parent->w * (transform.x)) + transformOffset.x;
			float yRatio = (parent->h * (transform.y)) + transformOffset.y;
			dst.x = parent->x + xRatio;
			dst.y = parent->y + yRatio;
		}
		else
		{
			dst.x += parent->x;
			dst.y += parent->y;
		}
	}

	if (cr.w == 0 && cr.h == 0 && parentClip)
		cr = *parentClip;

	if (transform.a == 0 || text == L"" || text.size() == 0 || dst.x + dst.w < 0 || dst.x > parent->w || dst.y + dst.h < 0 || dst.y > parent->h)
	{
		drawn = false;
		return;
	}
	drawn = true;

	Rect start = dst;

	drawCall call;
	call.texture = fnt->texture;
	call.shad = NULL;
	call.zIndex = zIndex;
	call.vertices = {};
	int totalW = 0;
	int highestH = 0;
	float scale = size / fnt->ogSize;
	std::vector<Line> outlines;
	std::vector<Line> lines;
	Line currentLine;
	Line currentOutline;
	int d = 0;
	for (int i = 0; i < text.size(); i++)
	{
		wchar_t ch = text[i];
		const AvgEngine::Fnt::FntChar c = fnt->GetWChar(ch);
		if (c.id == -1)
			continue;
		float fileAdvance = static_cast<float>(c.xAdvance);
		if (i + 1 < text.size() && fnt->hasKernings)
		{
			// check kerning
			wchar_t next = text[i + 1];
			for (int j = 0; j < c.kernings.size(); j++)
			{
				if (c.kernings[j].with == next)
				{
					fileAdvance -= c.kernings[j].amount;
					break;
				}
			}
		}
		float advance = ((fileAdvance * scale) + characterSpacing) * transform.scale;

		if (wrap && dst.x + dst.w > parent->x + parent->w)
		{
			dst.x = start.x;
			dst.y += highestH;
			d += highestH;
			highestH = 0;
			currentLine.w = totalW;
			if (outlineThickness != 0)
			{
				currentOutline.w = totalW;
				outlines.push_back(currentOutline);
			}
			lines.push_back(currentLine);
			currentLine = {};
			currentOutline = {};
			if (totalW > transform.w)
				transform.w = totalW;
			totalW = 0;
		}

		if (ch == 32)
		{
			CharacterLine l;
			l.space = true;
			l.advance = advance;
			totalW += advance;
			dst.x += advance;
			if (outlineThickness != 0)
				currentOutline.characters.push_back(l);
			currentLine.characters.push_back(l);
			continue;
		}
		Rect src = c.src;
		dst.w = c.dst.w * scale;
		dst.h = c.dst.h * scale;


		if (highestH < dst.h)
			highestH = dst.h;

		CharacterLine l;
		l.dst = dst;
		l.src = src;
		l.advance = advance;

		if (outlineThickness != 0)
		{
			Rect outline = dst;

			outline.r = 0;
			outline.g = 0;
			outline.b = 0;
			CharacterLine ll;
			ll.dst = outline;
			ll.outline = true;
			ll.src = src;
			ll.advance = advance;
			currentOutline.characters.push_back(ll);
		}
		currentLine.characters.push_back(l);
		dst.x += advance;
		totalW += advance;
	}
	currentLine.w = totalW;
	if (outlineThickness != 0)
	{
		currentOutline.w = totalW;
		outlines.push_back(currentOutline);
	}
	lines.push_back(currentLine);
	currentLine = {};


	if (d == 0)
		d = highestH;

	float highestW = 0;

	Rect r = start;

	if (centerLines)
		r.x -= (transform.w / 2);

	iTransform = r;

	for (Line& line : outlines)
	{
		int newStartX = start.x;
		if (centerLines)
			newStartX = start.x - (line.w / 2);

		int currentAdvance = 0;

		for (CharacterLine& c : line.characters)
		{
			int newStartY = c.dst.y;
			if (centerLines)
				newStartY = c.dst.y - (d / 2);
			c.dst.x = newStartX + currentAdvance;
			c.dst.y = newStartY;
			float outlineScale = (outlineThickness / 1);
			float mpx = (c.dst.w * (1 - outlineScale)) / 2;
			float mpy = (c.dst.h * (1 - outlineScale)) / 2;
			c.dst.x += mpx;
			c.dst.y += mpy;
			c.dst.w *= outlineScale;
			c.dst.h *= outlineScale;
			if (!c.space)
			{
				drawCall ca = Camera::FormatDrawCall(zIndex, fnt->texture, NULL, DisplayHelper::RectToVertex(c.dst, c.src), iTransform);
				ca.clip = cr;
				ca.zIndex = zIndex;
				camera->addDrawCall(ca);
			}
			currentAdvance += c.advance;
		}
	}

	for (Line& line : lines)
	{
		if (line.w > highestW)
			highestW = line.w;
		int newStartX = start.x;
		if (centerLines)
			newStartX = start.x - (line.w / 2);

		int currentAdvance = 0;

		for (CharacterLine& c : line.characters)
		{
			int newStartY = c.dst.y;
			if (centerLines)
				newStartY = c.dst.y - (d / 2);
			c.dst.x = newStartX + currentAdvance;
			c.dst.y = newStartY;
			if (c.outline)
			{
				float outlineScale = (outlineThickness / 1);
				float nX = (c.dst.w * (1 - outlineScale));
				c.dst.x += nX / 2;
				c.dst.y += (c.dst.h * (1 - outlineScale)) / 2;
				c.dst.w *= outlineScale;
				c.dst.h *= outlineScale;
			}
			if (!c.space)
			{
				drawCall ca = Camera::FormatDrawCall(zIndex, fnt->texture, NULL, DisplayHelper::RectToVertex(c.dst, c.src), iTransform);
				ca.clip = cr;
				ca.zIndex = zIndex;
				camera->addDrawCall(ca);
			}
			if (!c.outline)
				currentAdvance += c.advance;
		}
	}

	transform.w = highestW;
	transform.h = d;
	iTransform.w = highestW;
	iTransform.h = d;
	if (transformRatio)
	{
		transform.w = highestW / parent->w;
		transform.h = d / parent->h;
	}
	call.clip = cr;
	call.zIndex = zIndex;
	camera->addDrawCall(call);

	GameObject::draw();
}