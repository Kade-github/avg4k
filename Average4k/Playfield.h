#pragma once
#include "Object.h"
#include "NoteObject.h"
#include "ReceptorObject.h"
#include "ArrowEffects.h"
#include "AvgGroup.h"
class Playfield
{
public:
	AvgGroup* gInstance = NULL;

	ArrowEffects arrowEff;
	std::vector<ReceptorObject*> screenReceptors;
	std::vector<NoteObject*> screenNotes;

	bool mod = false;

	float x, y;

	Playfield(int _x, int _y, AvgGroup* g)
	{
		x = _x;
		y = _y;
		gInstance = g;
		arrowEff.resetEffects();

		for (int i = 0; i < 4; i++)
		{
			ReceptorObject* r;

			int index = i + 1;
			r = new ReceptorObject(
				x + ((64 * Game::save->GetDouble("Note Size") + 12) * i), y, i);
			r->lightUpTimer = 0;
			r->create();
			screenReceptors.push_back(r);
		}
	}

	~Playfield()
	{
		for (ReceptorObject* o : screenReceptors)
			gInstance->removeObj(o);
		for (NoteObject* o : screenNotes)
			gInstance->removeObj(o);
		screenNotes.clear();
		screenReceptors.clear();
	}

	void addReceptors()
	{
		for (ReceptorObject* o : screenReceptors)
		{
			o->arrowEffects = &arrowEff;
			gInstance->add(o);
		}
	}

	void addNote(NoteObject* ob)
	{
		ob->arrowEffects = &arrowEff;
		screenNotes.push_back(ob);
		gInstance->add(ob);
	}

	void update(float positionInSong, float beat);

};