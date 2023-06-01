#include "Receptor.h"
#include "Average4K.h"
#include "GameplayMenu.h"

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
				note->zIndex = 4;
				note->holdTexture = holdSpritesheet;
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
				note->xmod = 4;
				addObject(note);

				float beatRow = n.Beat * 48;

				switch (note->type)
				{
				case Chart::NoteType::NoteType_Tap:
					if (fmod(beatRow, (192 / 4)) == 0)
						note->frame = 4;
					else if (fmod(beatRow, (192 / 8)) == 0)
						note->frame = 6;
					else if (fmod(beatRow, (192 / 12)) == 0)
						note->frame = 0;
					else if (fmod(beatRow, (192 / 16)) == 0)
						note->frame = 1;
					else if (fmod(beatRow, (192 / 24)) == 0)
						note->frame = 2;
					else if (fmod(beatRow, (192 / 32)) == 0)
						note->frame = 3;
					else if (fmod(beatRow, (192 / 64)) == 0)
						note->frame = 5;
					else
						note->frame = 9;
					break;
				case Chart::NoteType_Fake:
					note->frame = 8;
					break;
				case Chart::NoteType_Mine:
					note->frame = 7;
					break;
				}

				notes.erase(notes.begin());
			}
		}
	}

	std::vector<GameObject*> toRemove;

	for (GameObject* ob : Children)
	{
		Note* n = static_cast<Note*>(ob);
		n->sTime = time;
		n->sBeat = beat;
		if (n->beat - beat < -k->options.drawbeats && n->judged && (n->type == Chart::NoteType_Head ? n->holdJudged : true))
		{
			toRemove.push_back(n);
			continue;
		}
	}

	for (GameObject* ob : toRemove)
	{
		removeObject(ob);
		delete ob;
	}

	AvgEngine::Base::Sprite::draw();
}

void Average4k::Objects::Gameplay::Receptor::hit()
{
	frame = 1;
	hitTimestamp = glfwGetTime();
	if (Children.size() == 0)
		return;
	Average4K* k = static_cast<Average4K*>(Average4K::Instance);
	Note* n = NULL;
	for (GameObject* o : Children)
	{
		Note* nn = static_cast<Note*>(o);
		if (nn->judged || nn->type == Chart::NoteType_Mine || nn->type == Chart::NoteType_Fake)
			continue;
		n = nn;
		break;
	}
	if (n == NULL)
		return;
	float diff = (n->time - time) * 1000;
	if (diff < k->options.judgeWindow[4])
	{
		n->hit();
		n->transform.a = 0;

		Average4k::Lua::GameplayMenu* m = static_cast<Average4k::Lua::GameplayMenu*>(k->CurrentMenu);
		m->file->Function("ArrowJudged", std::to_string(n->judge));

		m->UpdateAccuracy(n->judge);
	}
}

void Average4k::Objects::Gameplay::Receptor::release()
{
	frame = 0;
	Note* n = NULL;
	for (GameObject* o : Children)
	{
		Note* nn = static_cast<Note*>(o);
		if (nn->type != Chart::NoteType_Head && !nn->holdJudged)
			continue;
		n = nn;
		break;
	}
	if (n == NULL)
		return;
	n->release();
}
