#include "Playfield.h"
#include "ModManager.h"

void Playfield::update(float positionInSong, float beat)
{
	if (mod)
	{
		for (int i = 0; i < screenReceptors.size(); i++)
		{
			ReceptorObject* rec = screenReceptors[i];
			rec->positionInSong = positionInSong;
			rec->x = x;

			rec->modX = rec->x;
			rec->modY = rec->y;
		}

		for (NoteObject* obj : screenNotes)
		{
			if ((obj->beat > beat + arrowEff.drawBeats) || (obj->beat < beat - arrowEff.drawBeats))
			{
				obj->drawCall = false;
				continue;
			}
			else
				obj->drawCall = true;
			obj->rTime = positionInSong;
			obj->modCMOD = obj->cmod;
			obj->endCMOD = obj->modCMOD;
			if (positionInSong >= obj->time)
			{
				if (!obj->wasHit || (obj->endTime > positionInSong))
				{
					obj->wasHit = true;
					ModManager::instance->callEvent("hit", obj->lane);
				}
			}

		}

	}


}