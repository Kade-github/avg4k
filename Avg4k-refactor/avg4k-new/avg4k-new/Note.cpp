#include "Note.h"
#include "Average4K.h"

void Average4k::Objects::Gameplay::Note::calculateJudge(float diff_abs)
{
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);

	judged = true;

	if (diff_abs < k->options.judgeWindow[0])
	{
		judge = Judgement_Perfect;
		return;
	}

	if (diff_abs < k->options.judgeWindow[1])
	{
		judge = Judgement_Great;
		return;
	}

	if (diff_abs < k->options.judgeWindow[2])
	{
		judge = Judgement_Good;
		return;
	}
	if (diff_abs < k->options.judgeWindow[3])
	{
		judge = Judgement_Bad;
		return;
	}

}

void Average4k::Objects::Gameplay::Note::draw()
{
	frame = 0;
	src.x = frame * frameWidth;
	src.y = 0;
	src.w = frameWidth;
	src.h = frameHeight;

	float diff = time - sTime;
	transform.y = calculateY(false, diff);
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);
	if (!judged && (diff * 1000) < -k->options.judgeWindow[3])
	{
		judged = true;
		judge = Judgement_Miss;
	}
	AvgEngine::Base::Sprite::draw();
}
