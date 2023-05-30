#include "Note.h"
#include "Average4K.h"
#include "GameplayMenu.h"

void Average4k::Objects::Gameplay::Note::calculateJudge(float diff_abs)
{
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);

	judged = true;

	if (diff_abs < k->options.judgeWindow[0])
	{
		judge = Judgement_Marvelous;
		return;
	}

	if (diff_abs < k->options.judgeWindow[1])
	{
		judge = Judgement_Perfect;
		return;
	}

	if (diff_abs < k->options.judgeWindow[2])
	{
		judge = Judgement_Great;
		return;
	}
	if (diff_abs < k->options.judgeWindow[3])
	{
		judge = Judgement_Good;
		return;
	}

	if (diff_abs < k->options.judgeWindow[4])
	{
		judge = Judgement_Bad;
		return;
	}

}

void Average4k::Objects::Gameplay::Note::hit()
{
	float diff = std::abs(time - sTime);
	calculateJudge(diff * 1000);
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);
	holding = true;
}

void Average4k::Objects::Gameplay::Note::draw()
{
	src.x = frame * frameWidth;
	src.y = 0;
	src.w = frameWidth;
	src.h = frameHeight;

	float diff = time - sTime;
	transform.y = calculateY(false, diff);
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);
	Average4k::Lua::GameplayMenu* m = static_cast<Average4k::Lua::GameplayMenu*>(k->CurrentMenu);
	// tap misses
	// make sure this stuff works with downscroll (later)
	if (!judged && ((diff * 1000) < -k->options.judgeWindow[4] || transform.y + parent->y < 0) && type != Chart::NoteType::NoteType_Head && (type != Chart::NoteType_Fake && type != Chart::NoteType_Mine))
	{
		judged = true;
		judge = Judgement_Miss;
		m->file->Function("ArrowJudged", std::to_string((int)judge));
		m->UpdateAccuracy(judge);
	}

	AvgEngine::Base::Sprite::draw();
	if (type != Chart::NoteType::NoteType_Head)
		return;

	// welcome to spaghetti town, population: this code

	float end = beat + lengthInBeats;
	float endTime = k->options.currentFile->GetTimeFromBeat(end);
	float diffPos = endTime - time;
	int amountToDraw = 1 + ((calculateY(false, diffPos) - (64 * noteSize)) / (64 * noteSize));

	AvgEngine::Render::Rect lastRect = {};
	lastRect.a = 0;

	for (int i = 0; i < amountToDraw; i++)
	{
		AvgEngine::Render::Rect r = transform;
		
		r.x = parent->x - (32 * noteSize);
	
		AvgEngine::Render::Rect rSrc = src;
		r.angle = 0;
		if (i == amountToDraw - 1) // Hold end
		{
			rSrc.x = frameWidth - 1;
			r.angle = 180;
		}
		else
			rSrc.x = 0;
		// calculate the beat of the hold, via the start beat of the head * a percentage of the length we are already through.
		float holdBeat = beat + ((end - beat) * (static_cast<float>(i) / amountToDraw));
		float holdTime = k->options.currentFile->GetTimeFromBeat(holdBeat);
		if (!holdJudged)
			r.a = 1;
		else
			r.a = 0;
		r.y = calculateY(false, holdTime - sTime) + (64 * noteSize);

		if (lastRect.a != 0)
			r.y = lastRect.y + lastRect.h;
		lastRect = r;

		// later we will make our own little vertice hell for mods, but for now, this will do just fine.
		// normalize the coords for opengl

		rSrc.x = rSrc.x / holdTexture->width;
		rSrc.y = rSrc.y / holdTexture->height;
		rSrc.w = rSrc.w / holdTexture->width;
		rSrc.h = rSrc.h / holdTexture->height;

		drawCall c = Camera::FormatDrawCall(zIndex - 1, holdTexture, shader, AvgEngine::Render::DisplayHelper::RectToVertex(r, rSrc, center));
		if (holding)
		{
			AvgEngine::Render::Rect rClip = r;
			rClip.y = parent->y;
			rClip.h = AvgEngine::Render::Display::height;
			c.clip = rClip;
		}
		camera->addDrawCall(c);
	}

	// hold misses
	if ((time - sTime) * 1000 < -k->options.judgeWindow[5] && sTime < endTime && !holdJudged)
	{
		if (holdTimer <= 0)
		{
			holdJudged = true;
			judge = Judgement_Miss;
			m->file->Function("ArrowJudged", std::to_string(judge));
			m->UpdateAccuracy(judge);
		}
		if (!holding)
			holdTimer -= 0.05f;
		else if (holdTimer < 1)
			holdTimer += 0.02f;
	}

	// set the judged flag for holds
	if (sTime > endTime)
		holdJudged = true;
}
