#include "Gameplay.h"

noteskin_asset* Gameplay::noteskin;

float Gameplay::rate = 1;

void CALLBACK bruh(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
}

void Gameplay::initControls()
{
	// default for now

	controls.clear();

	gameplayControl left;
	left.code = SDLK_d;
	left.lane = 0;
	gameplayControl down;
	down.code = SDLK_f;
	down.lane = 1;
	gameplayControl up;
	up.code = SDLK_j;
	up.lane = 2;
	gameplayControl right;
	right.code = SDLK_k;
	right.lane = 3;

	controls.push_back(left);
	controls.push_back(down);
	controls.push_back(up);
	controls.push_back(right);
}

const float min(const float a, const float b)
{
	return (b < a) ? b : a;
}

void Gameplay::updateAccuracy(double hitWorth)
{
	notesPlayed++;
	notesHit += hitWorth;
	accuracy = ((notesHit / notesPlayed) * 100);
	if (accuracy < 0)
		accuracy = 0;

	std::string format = std::to_string((double)((int)(accuracy * 100)) / 100);
	format.erase(format.find_last_not_of('0') + 1, std::string::npos);

	if (floor(accuracy) == accuracy)
		format.erase(format.find_last_not_of('.') + 1, std::string::npos);

	Accuracy->setText(format + "%\n\nMarvelous: " + std::to_string(Marvelous) + "\nPerfect : " + std::to_string(Perfect) + "\nGreat : " + std::to_string(Great) + "\nEh : " + std::to_string(Eh) + "\nYikes: " + std::to_string(Yikes) + "\nCombo Breaks: " + std::to_string(Misses));
}

void Gameplay::removeNote(NoteObject* object)
{
	spawnedNotes.erase(
		std::remove_if(spawnedNotes.begin(), spawnedNotes.end(), [&](NoteObject* const nn) {
			return nn->beat == object->beat && nn->lane == object->lane;
			}),
		spawnedNotes.end());

	object->destroy();
}

void Gameplay::miss(NoteObject* object)
{
	Misses++;
	updateAccuracy(-0.4);
	combo = 0;
	Judgement->setText("MISS");
	(*Judgement).color.r = 255;
	(*Judgement).color.g = 0;
	(*Judgement).color.b = 0;

	Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
	Judgement->setY((Game::gameHeight / 2));


	Combo->setText(" ");
	Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
	Combo->setY((Game::gameHeight / 2) + 40);
}

Gameplay::Gameplay()
{
	initControls();

	downscroll = Game::save->GetBool("downscroll");

	Judge::initJudge();

	Judge::judgeNote(174);

	std::string path = MainMenu::currentChart->meta.folder + "/" + MainMenu::currentChart->meta.audio;

	std::cout << "playing " << path << std::endl;
	
	channel = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_DECODE);

	tempostream = BASS_FX_TempoCreate(channel, BASS_FX_FREESOURCE);

	BASS_ChannelSetAttribute(tempostream, BASS_ATTRIB_TEMPO, 0);

	noteskin = Noteskin::getNoteskin();
	notesToPlay = *(*MainMenu::currentChart->meta.difficulties)[MainMenu::selectedDiffIndex].notes;

	positionAndBeats = new Text(0, 20, "Time: 0 | Beat: 0 | Offset: 0", 60, 40);
	positionAndBeats->create();

	Judgement = new Text(Game::gameWidth / 2, Game::gameHeight / 2, " ", 100, 100);
	Judgement->create();

	Combo = new Text(Game::gameWidth / 2, Game::gameHeight / 2 + 40, " ", 100, 100);
	Combo->create();

	Accuracy = new Text(0, Game::gameHeight / 2, "N/A\n\nMarvelous: " + std::to_string(Marvelous) + "\nPerfect: " + std::to_string(Perfect) + "\nGreat: " + std::to_string(Great) + "\nEh: " + std::to_string(Eh) + "\nYikes: " + std::to_string(Yikes) + "\nCombo Breaks: " + std::to_string(Misses), 100, 40);
	Accuracy->create();

	for (int i = 0; i < 4; i++)
	{
		Receptor r;
		SDL_FRect rect;
		if (downscroll)
			rect.y = (Game::gameHeight / 2) + 250;
		else
			rect.y = (Game::gameHeight / 2) - 300;
		rect.x = (Game::gameWidth / 2) - 146 + ((76) * i);
		rect.w = 64;
		rect.h = 64;
		r.rect = rect;

		receptors.push_back(r);
	}

	positionInSong = -500;
}



float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void Gameplay::update(Events::updateEvent event)
{
	if (positionInSong >= startTime)
	{
		if (!play)
		{
			auto stuff = BASS_ChannelSeconds2Bytes(tempostream, startTime);
			std::cout << "offset time: " << stuff << " = " << startTime << std::endl;
			BASS_ChannelSetPosition(tempostream, stuff, BASS_POS_BYTE);
			BASS_ChannelPlay(tempostream, false);
			play = true;
		}

		positionInSong = BASS_ChannelBytes2Seconds(tempostream, BASS_ChannelGetPosition(tempostream, BASS_POS_BYTE)) * 1000;
	}
	else
		positionInSong += Game::deltaTime;

	curSeg = MainMenu::currentChart->getSegmentFromTime(positionInSong);
	beat = MainMenu::currentChart->getBeatFromTimeOffset(positionInSong, curSeg);
	positionAndBeats->setText("Time: " + std::to_string(positionInSong) + " | Beat: " + std::to_string(beat) + " | BPM: " + std::to_string(curSeg.bpm));

	if (BASS_ErrorGetCode() != 0)
	{
		positionAndBeats->setText("BASS ERROR CODE: " + std::to_string(BASS_ErrorGetCode()));
	}

	for (int i = 0; i < receptors.size(); i++)
	{
		switch (i)
		{
			case 0:
				SDL_RenderCopyExF(Game::renderer, noteskin->receptor, NULL, &receptors[i].rect, 90, NULL, SDL_FLIP_NONE);
				break;
			case 1:
				SDL_RenderCopyExF(Game::renderer, noteskin->receptor, NULL, &receptors[i].rect, 0, NULL, SDL_FLIP_NONE);
				break;
			case 2:
				SDL_RenderCopyExF(Game::renderer, noteskin->receptor, NULL, &receptors[i].rect, 180, NULL, SDL_FLIP_NONE);
				break;
			case 3:
				SDL_RenderCopyExF(Game::renderer, noteskin->receptor, NULL, &receptors[i].rect, -90, NULL, SDL_FLIP_NONE);
				break;
		}
	}

	// spawning shit

	if (notesToPlay.size() > 0)
	{
		note& n = notesToPlay[0];
		if (n.type == Note_Tail || n.type == Note_Mine)
			notesToPlay.erase(notesToPlay.begin());
		else if (n.beat < beat + 16) // if its in 16 beats
		{
			NoteObject* object = new NoteObject();
			SDL_FRect rect;
			object->active = true;
			object->beat = n.beat;
			object->lane = n.lane;
			object->type = n.type;
			object->endTime = -1;
			object->endBeat = -1;

			bpmSegment noteSeg = MainMenu::currentChart->getSegmentFromBeat(object->beat);

			object->time = MainMenu::currentChart->getTimeFromBeatOffset(object->beat, noteSeg) * 1000;
			rect.y = Game::gameHeight + 400;
			rect.x = receptors[object->lane].rect.x;
			rect.w = 64;
			rect.h = 64;
			object->rect = rect;

			note tail;

			bpmSegment bruh = MainMenu::currentChart->getSegmentFromBeat(object->beat);

			float wh = MainMenu::currentChart->getTimeFromBeat(beat, bruh);

			float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;


			if (object->type == Note_Head)
			{
				for (int i = 0; i < notesToPlay.size(); i++)
				{
					note& nn = notesToPlay[i];
					if (nn.type != Note_Tail)
						continue;
					if (nn.lane != object->lane)
						continue;
					object->endBeat = nn.beat;
					bpmSegment heldSeg = MainMenu::currentChart->getSegmentFromBeat(nn.beat);

					object->endTime = MainMenu::currentChart->getTimeFromBeatOffset(nn.beat, noteSeg) * 1000;
					tail = nn;
					break;
				}
			}

			notesToPlay.erase(notesToPlay.begin());

			if (object->type == Note_Head)
			{
				float noteOffset = (0.45 * Game::save->GetDouble("scrollspeed")) + MainMenu::offset;

				for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
				{
					bpmSegment holdSeg = MainMenu::currentChart->getSegmentFromTime(i);

					double beat = MainMenu::currentChart->getBeatFromTime(i, holdSeg);

					if (beat >= object->endBeat - 0.4)
						break;

					float whHold = MainMenu::currentChart->getTimeFromBeat(beat, holdSeg);

					float diff = whHold - (object->time / 1000);

					float noteOffset = (bps * diff) * 64;

					if (object->heldTilings.size() != 0)
						diff = whHold - (object->heldTilings.back().time / 1000);
					float y = 0;
					float yDiff = 0;
					if (object->heldTilings.size() != 0)
					{
						if (downscroll)
							y = object->rect.y - noteOffset;
						else
							y = object->rect.y + noteOffset;
						yDiff = y - object->heldTilings.back().rect.y;
					}
					else
					{
						if (downscroll)
							y = object->rect.y - noteOffset;
						else
							y = object->rect.y + noteOffset;
						yDiff = y - object->rect.y;
					}

					bool otherOne = false;

					if (downscroll)
						otherOne = yDiff <= -64;
					else
						otherOne = yDiff >= 64;

					if (otherOne || object->heldTilings.size() == 0)
					{
						holdTile tile;
						SDL_FRect rect;
						tile.active = true;
						tile.fucked = false;
						rect.y = y;
						rect.x = object->rect.x;
						rect.w = 64;
						rect.h = 64;
						tile.rect = rect;
						tile.beat = beat;
						tile.time = i;
						object->heldTilings.push_back(tile);
					}
				}
			}
			std::sort(object->heldTilings.begin(), object->heldTilings.end());
			std::cout << "pushed " << object->heldTilings.size() << " tiles" << std::endl;
			spawnedNotes.push_back(object);
		}
	}

	// ok lets draw all of them now


	for (int i = 0; i < spawnedNotes.size(); i++)
	{
		NoteObject* note = spawnedNotes[i];

		if (!note->destroyed)
		{
			float wh = MainMenu::currentChart->getTimeFromBeat(note->beat, MainMenu::currentChart->getSegmentFromBeat(note->beat)) * 1000;

			if (wh - positionInSong < 2 && note->active && botplay)
			{
				Judgement->setText("Marvelous (2ms)");
				(*Judgement).color.r = 0;
				(*Judgement).color.g = 255;
				(*Judgement).color.b = 255;
				Marvelous++;
				updateAccuracy(1);
				note->active = false;

				combo++;

				Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
				Judgement->setY((Game::gameHeight / 2));

				Combo->setText("Botplay");
				Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
				Combo->setY((Game::gameHeight / 2) + 40);

			}

			// BEFORE DRAW, CHECK HELD NOTES!!

			if (note->type == Note_Head)
			{
				if (keys[note->lane] || botplay) // holding that lane!
				{
					for (int i = 0; i < note->heldTilings.size(); i++)
					{
						holdTile& tile = note->heldTilings[i];
						float wh = MainMenu::currentChart->getTimeFromBeat(tile.beat, MainMenu::currentChart->getSegmentFromBeat(tile.beat)) * 1000;
						if (wh - positionInSong <= Judge::hitWindows[4] && !tile.fucked)
							tile.active = false;
					}
				}
			}

			bool fuckOver = false;

			for (int i = 0; i < note->heldTilings.size(); i++)
			{
				holdTile& tile = note->heldTilings[i];

				bool condition = false;

				if (downscroll)
					condition = tile.rect.y >= receptors[note->lane].rect.y - 32;
				else
					condition = tile.rect.y <= receptors[note->lane].rect.y + 32;

				if (!tile.active && condition)
				{
					note->heldTilings.erase(
						std::remove_if(note->heldTilings.begin(), note->heldTilings.end(), [&](holdTile& const nn) {
							return nn.beat == tile.beat;
							}),
						note->heldTilings.end());
				}
				auto whHold = MainMenu::currentChart->getTimeFromBeatOffset(tile.beat, MainMenu::currentChart->getSegmentFromBeat(tile.beat)) * 1000;
				float diff = whHold - positionInSong;

				if (diff < -Judge::hitWindows[4] && tile.active && !tile.fucked)
				{
					std::cout << note->lane << " fucked" << std::endl;
					miss(note);
					fuckOver = true;
					tile.fucked = true;
				}

				if (diff < -200 && tile.fucked)
				{
					note->heldTilings.erase(
						std::remove_if(note->heldTilings.begin(), note->heldTilings.end(), [&](holdTile& const nn) {
							return nn.beat == tile.beat;
							}),
						note->heldTilings.end());
				}

			}

			for (int i = 0; i < note->heldTilings.size(); i++)
			{
				holdTile& tile = note->heldTilings[i];
				if (fuckOver)
					tile.fucked = true;
			}

			if (note->lane < 4 && note->lane >= 0)
				note->draw(positionInSong, beat, receptors[note->lane].rect);
			
			if (wh - positionInSong  <= -Judge::hitWindows[4] && note->active)
			{
				note->active = false;
				miss(note);
			}

			if (wh - positionInSong <= -200 && !note->active && (note->heldTilings.size() == 0 || note->heldTilings[0].fucked))
			{
				removeNote(note);
			}
		}
	}

}


void Gameplay::keyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			if (!MainMenu::instance)
				delete MainMenu::instance;
			MainMenu::currentChart->destroy();
			Judgement->die();
			Combo->die();
			Accuracy->die();
			positionAndBeats->die();
			for (int i = 0; i < spawnedNotes.size(); i++)
			{
				spawnedNotes[i]->destroy();
			}

			BASS_ChannelStop(tempostream);
			BASS_ChannelFree(tempostream);

			Game::currentMenu = new MainMenu();
			delete this;
			return;
		case SDLK_F1:
			botplay = !botplay;
			return;
		case SDLK_BACKQUOTE:
			Game::currentMenu = new Gameplay();
			Judgement->die();
			Combo->die();
			Accuracy->die();
			positionAndBeats->die();
			for (int i = 0; i < spawnedNotes.size(); i++)
			{
				spawnedNotes[i]->destroy();
			}

			BASS_ChannelStop(tempostream);
			BASS_ChannelFree(tempostream);
			delete this;
			return;
	}

	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == event.keysym.sym)
		{
			if (!keys[control.lane])
				keys[control.lane] = true;
			else
				return;

			std::cout << "pressing " << control.lane << std::endl;

			// note lol

			NoteObject* closestObject = nullptr;
			float currentDiff = Judge::hitWindows[4] + 1;

			for (int n = 0; n < spawnedNotes.size(); n++)
			{
				NoteObject* object = spawnedNotes[n];

				const auto wh = MainMenu::currentChart->getTimeFromBeat(object->beat, MainMenu::currentChart->getSegmentFromBeat(object->beat)) * 1000;

				float diff = std::abs(wh - positionInSong);

				if (diff <= Judge::hitWindows[4] && diff < currentDiff && object->lane == control.lane)
				{
					closestObject = object;
					currentDiff = diff;
				}
			}

			if (!closestObject)
				continue;

			const auto wh = MainMenu::currentChart->getTimeFromBeat(closestObject->beat, MainMenu::currentChart->getSegmentFromBeat(closestObject->beat)) * 1000;

			float diff = wh - positionInSong;

			if (closestObject->active && diff <= Judge::hitWindows[4] && diff > -Judge::hitWindows[4])
			{
				closestObject->active = false;
				if (closestObject->type != Note_Head)
					removeNote(closestObject);

				judgement judge = Judge::judgeNote(diff);

				std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				switch (judge)
				{
					case judgement::Judge_marvelous:
						Judgement->setText("Marvelous (" + format + "ms)");
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 255;
						Marvelous++;
						updateAccuracy(1);
						break;
					case judgement::Judge_perfect:
						Judgement->setText("Perfect (" + format + "ms)");
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Perfect++;
						updateAccuracy(0.88);
						break;
					case judgement::Judge_great:
						Judgement->setText("Great (" + format + "ms)");
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Great++;
						updateAccuracy(0.7);
						break;
					case judgement::Judge_good:
						Judgement->setText("Eh (" + format + "ms)");
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Eh++;
						updateAccuracy(0.35);
						break;
					case judgement::Judge_bad:
						combo = 0;
						Judgement->setText("Yikes (" + format + "ms) ");
						(*Judgement).color.r = 128;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Yikes++;
						updateAccuracy(0.1);
						break;
					}

					combo++;

					Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
					Judgement->setY((Game::gameHeight / 2));

					if (combo == 0)
						Combo->setText(" ");
					else
						Combo->setText(std::to_string(combo));
					Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
					Combo->setY((Game::gameHeight / 2) + 40);
					break;
				}
		}
	}
}

void Gameplay::keyUp(SDL_KeyboardEvent ev)
{
	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == ev.keysym.sym)
		{
			keys[control.lane] = false;
		}
	}
}
