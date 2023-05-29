#include "Receptor.h"
#include "Average4K.h"


void Average4k::Objects::Gameplay::Receptor::draw()
{
	fps = 0;
	src.x = frame * frameWidth;
	src.y = 0;
	src.w = frameWidth;
	src.h = frameHeight;

	if (frame == 1)
		transform.scale = 0.85f;
	else
		transform.scale = 1.0f;
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);
	// create notes if they're in the drawbeats
	if (notes.size() > 0)
	{
		Chart::Note n = notes[0];
		if (n.Type == Chart::NoteType_Tail)
		{
			if (Children.size() != 0)
			{
				Note* last = static_cast<Note*>(Children.back());
				last->lengthInBeats = n.Beat - last->beat;
			}
			notes.erase(notes.begin());
		}
		else
		{
			float beatDiff = n.Beat - beat;
			if (beatDiff < k->options.drawbeats && beatDiff > -k->options.drawbeats)
			{

				Note* note = new Note(0, 0, arrowSpritesheet);
				note->tag = "Note-" + std::to_string(n.Beat);
				note->transform.angle = transform.angle;
				note->time = k->options.currentFile->GetTimeFromBeat(n.Beat);
				note->beat = n.Beat;
				note->type = (Chart::NoteType)n.Type;
				note->lane = n.Lane;
				note->center = true;
				note->SetFrameSize(frameWidth, frameHeight);
				note->transform.w = transform.w;
				note->transform.h = transform.h;
				note->noteSize = noteSize;
				note->cmod = std::stof(k->settings->Get("Scrollspeed").value);
				addObject(note);

				notes.erase(notes.begin());
			}
		}
	}

	for (GameObject* ob : Children)
	{
		Note* n = static_cast<Note*>(ob);
		n->sTime = time;
		if (n->beat - beat < -k->options.drawbeats && n->judged)
		{
			removeObject(n);
			delete n;
		}
	}
	AvgEngine::Base::Sprite::draw();
}
