#include "Gameplay.h"
#include "SongSelect.h"
#include "MultiplayerLobby.h"


noteskin_asset* Gameplay::noteskin;

std::map<std::string, SDL_Texture*> avatars;

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
	left.lane = 4;
	gameplayControl down;
	down.code = SDLK_f;
	down.lane = 5;
	gameplayControl up;
	up.code = SDLK_j;
	up.lane = 6;
	gameplayControl right;
	right.code = SDLK_k;
	right.lane = 7;

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

	//Accuracy->setText(format + "%\n\nMarvelous: " + std::to_string(Marvelous) + "\nPerfect : " + std::to_string(Perfect) + "\nGreat : " + std::to_string(Great) + "\nEh : " + std::to_string(Eh) + "\nYikes: " + std::to_string(Yikes) + "\nCombo Breaks: " + std::to_string(Misses));
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
	if (MultiplayerLobby::inLobby)
	{
		noteId++;
		CPacketNoteHit hit;
		hit.NoteID = noteId;
		hit.NoteTiming = Judge::hitWindows[4];
		hit.Order = 0;
		hit.PacketType = eCPacketNoteHit;

		Multiplayer::sendMessage<CPacketNoteHit>(hit);
	}

	Misses++;
	updateAccuracy(-0.4);
	combo = 0;
	(*Judgement).color.r = 255;
	(*Judgement).color.g = 0;
	(*Judgement).color.b = 0;
	Judgement->setText("MISS");

	Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
	Judgement->setY((Game::gameHeight / 2));


	Combo->setText(" ");
	Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
	Combo->setY((Game::gameHeight / 2) + 40);
}

void Gameplay::onPacket(PacketType pt, char* data, int32_t length)
{
	if (!MultiplayerLobby::inLobby)
		return;

	SPacketUpdateLeaderboard pack;
	SPacketFinalizeChart ch;

	msgpack::unpacked result;

	msgpack::object obj;

	switch (pt)
	{
	case eSPacketUpdateLeaderboard:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(pack);

		for (leaderboardSpot p : leaderboard)
			p.t->destroy();

		leaderboard.clear();

		for (int i = 0; i < pack.orderedScores.size(); i++)
		{
			PlayerScore score = pack.orderedScores[i];
			leaderboardSpot spot;
			spot.score = score;
			std::string format = std::to_string((double)((int)(spot.score.meanTiming * 100)) / 100);
			format.erase(format.find_last_not_of('0') + 1, std::string::npos);

			spot.t = new Text(4, (Game::gameHeight / 2) + 4, spot.score.Username + " - (" + std::to_string(spot.score.score) + "/" + format + "ms)", 24);
			spot.t->y = spot.t->y + (spot.t->surfH * i);
			leaderboard.push_back(spot);
			spot.t->create();
		}

		break;
	case eSPacketFinalizeChart:

		for (leaderboardSpot p : leaderboard)
			p.t->destroy();

		leaderboard.clear();

		SongSelect::currentChart->destroy();
		cleanUp();
		std::cout << "go back" << std::endl;

		Game::currentMenu = new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, false);
		break;
	}
}

Gameplay::Gameplay()
{
	initControls();

	avatars.clear();

	downscroll = Game::save->GetBool("downscroll");

	gf = new SparrowAtlas("assets/holiday/GF_assets.png", "assets/holiday/GF_assets.xml", 24, "GF Dancing Beat");
	gf->x = (Game::gameWidth / 2) - 300;
	gf->y = 135;
	gf->loop = true;

	bf = new SparrowAtlas("assets/holiday/BOYFRIEND.png", "assets/holiday/BOYFRIEND.xml",24,"BF idle dance");
	bf->x = (Game::gameWidth / 2) + 185;
	bf->y = 400;
	bf->loop = true;

	bf->scale = 0.8;

	gf->scale = 0.8;

	//bf = new SparrowAtlas("assets/holiday/BOYFRIEND.png", "assets/holiday/BOYFRIEND.xml", 24, "BF idle dance");



	Judge::initJudge();

	Judge::judgeNote(174);

	if (MultiplayerLobby::inLobby)
		for(int i = 0; i < MultiplayerLobby::CurrentLobby.PlayerList.size(); i++)
		{
			player pp = MultiplayerLobby::CurrentLobby.PlayerList[i];
			const char* pog = pp.AvatarURL.c_str();
			SDL_Texture* s = Steam::getAvatar(pog);
			avatars[pp.SteamID64] = s;
		}

	std::string bg = SongSelect::currentChart->meta.folder + "/" + SongSelect::currentChart->meta.background;

	background = IMG_LoadTexture(Game::renderer,"assets/holiday/bg.png");

	float bassRate = (rate * 100) - 100;

	std::string path = SongSelect::currentChart->meta.folder + "/" + SongSelect::currentChart->meta.audio;

	std::cout << "playing " << path << std::endl;

	

	channel = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_DECODE);

	clap = BASS_StreamCreateFile(false, "assets/sounds/hitSound.mp3", 0, 0, NULL);

	tempostream = BASS_FX_TempoCreate(channel, BASS_FX_FREESOURCE);

	BASS_ChannelSetAttribute(tempostream, BASS_ATTRIB_TEMPO, bassRate);

	QWORD word = BASS_ChannelGetLength(tempostream, BASS_POS_BYTE);

	songLength = BASS_ChannelBytes2Seconds(tempostream, word);

	int diff = SongSelect::selectedDiffIndex;


	noteskin = Noteskin::getNoteskin();
	notesToPlay = SongSelect::currentChart->meta.difficulties[diff].notes;


	Judgement = new Text(Game::gameWidth / 2, Game::gameHeight / 2, " ", 24);
	Judgement->create();

	Combo = new Text(Game::gameWidth / 2, Game::gameHeight / 2 + 40, " ", 24);
	Combo->create();

	//Accuracy = new Text(230, (Game::gameHeight / 2) - 300, "N/A\n\nMarvelous: " + std::to_string(Marvelous) + "\nPerfect: " + std::to_string(Perfect) + "\nGreat: " + std::to_string(Great) + "\nEh: " + std::to_string(Eh) + "\nYikes: " + std::to_string(Yikes) + "\nCombo Breaks: " + std::to_string(Misses), 24);
	//Accuracy->create();

	for (int i = 0; i < 4; i++)
	{
		ReceptorObject* r;
		if (downscroll)
			r = new ReceptorObject(
				((Game::gameWidth / 2) - 146 + ((76) * i)) - 300,(Game::gameHeight / 2) + 250, i);
		else
			r = new ReceptorObject(
				((Game::gameWidth / 2) - 146 + ((76) * i)) - 300, (Game::gameHeight / 2) - 300, i);

		receptors.push_back(r);
	}

	for (int i = 4; i < 8; i++)
	{
		ReceptorObject* r;
		if (downscroll)
			r = new ReceptorObject(
				((Game::gameWidth / 2) - 146 + ((76) * (i % 4))) + 245, (Game::gameHeight / 2) + 250, i);
		else
			r = new ReceptorObject(
				((Game::gameWidth / 2) - 146 + ((76) * (i % 4))) + 245, (Game::gameHeight / 2) - 300, i);

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
			BASS_ChannelSetAttribute(tempostream, BASS_ATTRIB_VOL, 0.6);
			BASS_ChannelPlay(tempostream, false);
			play = true;
		}

		positionInSong = BASS_ChannelBytes2Seconds(tempostream, BASS_ChannelGetPosition(tempostream, BASS_POS_BYTE)) * 1000;
	}
	else
		positionInSong += Game::deltaTime;

	SDL_FRect bruh;
	bruh.x = 0;
	bruh.y = 0;
	bruh.h = 720;
	bruh.w = 1280;


	if (BASS_ErrorGetCode() != 0)
		Combo->setText(std::to_string(BASS_ErrorGetCode()) + "_bassError");


	SDL_FRect laneUnderway;

	laneUnderway.x = receptors[0]->x - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (receptors[3]->x - laneUnderway.x) + 68;
	laneUnderway.h = 1280;

	if (background)
	{
		SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);
		SDL_RenderCopyF(Game::renderer, background, NULL, &bruh);
	}

	if (gf->finished && SDL_GetTicks() - gf->startTime > 500)
	{
		gf->loop = true;
		gf->play("GF Dancing Beat", 24,0,0);
	}

	if (bf->finished)
	{
		bf->loop = true;
		bf->play("BF idle dance", 24,0,0);
	}

	gf->draw();

	bf->draw();

	/*SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 150);
	SDL_RenderFillRectF(Game::renderer, &laneUnderway);
	SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);*/


	if (!SongSelect::currentChart)
		return;

	curSeg = SongSelect::currentChart->getSegmentFromTime(positionInSong);
	beat = SongSelect::currentChart->getBeatFromTimeOffset(positionInSong, curSeg);

	// underlay for accuracy

	SDL_FRect overlayForAccuracy;

	overlayForAccuracy.x = Accuracy->x - 4;
	overlayForAccuracy.y = (Game::gameHeight / 2) - 302;
	overlayForAccuracy.w = Accuracy->surfH;
	overlayForAccuracy.h = Accuracy->surfH;


	/*SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 128);
	SDL_RenderFillRectF(Game::renderer, &overlayForAccuracy);
	SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);*/

	// multiplayer shit

	// leaderboard

	if (MultiplayerLobby::inLobby)
	{
		for (int i = 0; i < leaderboard.size(); i++)
		{
			leaderboardSpot& spot = leaderboard[i];
			spot.rect.w = 100;
			spot.rect.h = 85;
			spot.rect.x = 0;
			spot.rect.y = (Game::gameHeight / 2) + 2;

			SDL_FRect avatar;
			avatar.x = 0;
			avatar.w = 46;
			avatar.h = 46;

			spot.rect.w = 345;
			if (spot.t->surfW > 345)
				spot.rect.w = spot.t->surfW;
			spot.rect.h = 46;
			spot.rect.y = spot.rect.y + (48 * i);
			avatar.y = spot.rect.y;
			spot.t->y = spot.rect.y + 10;
			spot.t->x = 50;

			SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 128);
			SDL_RenderFillRectF(Game::renderer, &spot.rect);
			SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);

			SDL_RenderCopyF(Game::renderer, avatars[spot.score.SteamID64], NULL, &avatar);

			SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

			SDL_RenderDrawRectF(Game::renderer, &avatar);

			SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
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

			bpmSegment noteSeg = SongSelect::currentChart->getSegmentFromBeat(object->beat);

			object->time = SongSelect::currentChart->getTimeFromBeatOffset(object->beat, noteSeg) * 1000;
			rect.y = Game::gameHeight + 400;
			rect.x = receptors[object->lane]->x;
			rect.w = 64;
			rect.h = 64;
			object->rect = rect;

			note tail;

			bpmSegment bruh = SongSelect::currentChart->getSegmentFromBeat(object->beat);

			float wh = SongSelect::currentChart->getTimeFromBeat(beat, bruh);

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
					bpmSegment heldSeg = SongSelect::currentChart->getSegmentFromBeat(nn.beat);

					object->endTime = SongSelect::currentChart->getTimeFromBeatOffset(nn.beat, noteSeg) * 1000;
					tail = nn;
					break;
				}
			}

			notesToPlay.erase(notesToPlay.begin());

			if (object->type == Note_Head)
			{
				float noteOffset = (0.45 * Game::save->GetDouble("scrollspeed")) + Game::save->GetDouble("offset");

				for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
				{
					bpmSegment holdSeg = SongSelect::currentChart->getSegmentFromTime(i);

					double beat = SongSelect::currentChart->getBeatFromTime(i, holdSeg);

					if (beat <= object->beat + 0.04)
						continue;

					float whHold = SongSelect::currentChart->getTimeFromBeat(beat, holdSeg);

					float diff = whHold - (object->time / 1000);

					if (SongSelect::currentChart->meta.chartType == 1)
						diff = whHold - object->time;

					noteOffset = (bps * diff) * 64;

					if (object->heldTilings.size() != 0)
					{
						diff = whHold - (object->heldTilings.back().time / 1000);

						if (SongSelect::currentChart->meta.chartType == 1)
							diff = whHold - object->heldTilings.back().time;
					}

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
						rect.h = 65;
						tile.rect = rect;
						tile.beat = beat;
						tile.time = i;
						object->heldTilings.push_back(tile);
					}
				}
			}
			std::sort(object->heldTilings.begin(), object->heldTilings.end());
			spawnedNotes.push_back(object);
		}
	}
	else
	{
		if (!ended && spawnedNotes.size() == 0 && positionInSong > (songLength * 1000) - 450)
		{
			ended = true;
			if (!MultiplayerLobby::inLobby)
			{
				SongSelect::currentChart->destroy();
				cleanUp();
				Game::currentMenu = new MainMenu();
			}
			else
			{
				CPacketSongFinished song;
				song.Order = 0;
				song.PacketType = eCPacketSongFinished;

				Multiplayer::sendMessage<CPacketSongFinished>(song);

				Combo->setText("Waiting for others to finish (" + std::to_string(combo) + ")");
				Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
				Combo->setY((Game::gameHeight / 2) + 40);
			}
		}
	}
	// ok lets draw all of them now


	for (int i = 0; i < spawnedNotes.size(); i++)
	{
		NoteObject* note = spawnedNotes[i];

		if (!note->destroyed)
		{
			float wh = SongSelect::currentChart->getTimeFromBeat(note->beat, SongSelect::currentChart->getSegmentFromBeat(note->beat)) * 1000;

			if ((wh + Game::save->GetDouble("offset")) - positionInSong < 2 && note->active && note->lane < 4)
			{
				if (Game::save->GetBool("hitsounds"))
					BASS_ChannelPlay(clap, true);
				receptors[note->lane]->lightUpTimer = 100;

				float diff = (wh - positionInSong) - Game::save->GetDouble("offset");

				gf->loop = false;
				switch (note->lane)
				{
				case 0:
					gf->play("GF left note", 24, 0, 0);
					break;
				case 1:
					gf->play("GF Down Note", 24, 0, -8);
					break;
				case 2:
					gf->play("GF Up Note", 24, 0, 14);
					break;
				case 3:
					gf->play("GF Right Note", 24, 0, -17);
					break;
				}

				note->active = false;

				/*std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				Judgement->setText("Botplay (" + format + "ms)");
				(*Judgement).color.r = 0;
				(*Judgement).color.g = 255;
				(*Judgement).color.b = 255;
				Marvelous++;
				updateAccuracy(1);

				combo++;

				Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
				Judgement->setY((Game::gameHeight / 2));


				Combo->setText(std::to_string(combo));
				Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
				Combo->setY((Game::gameHeight / 2) + 40);*/

			}

			// BEFORE DRAW, CHECK HELD NOTES!!

			if (note->type == Note_Head)
			{
				if (keys[note->lane] || note->lane < 4) // holding that lane!
				{
					for (int i = 0; i < note->heldTilings.size(); i++)
					{
						holdTile& tile = note->heldTilings[i];
						float wh = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat)) * 1000;
						if ((wh + Game::save->GetDouble("offset")) - positionInSong <= Judge::hitWindows[1] && !tile.fucked)
						{
							tile.active = false;
							if (note->lane < 4)
							{
								receptors[note->lane]->lightUpTimer = 100;
								switch (note->lane)
								{
								case 0:
									gf->play("GF left note", 24, 0, 0);
									break;
								case 1:
									gf->play("GF Down Note", 24, 0, -8);
									break;
								case 2:
									gf->play("GF Up Note", 24, 0, 14);
									break;
								case 3:
									gf->play("GF Right Note", 24, 0, -17);
									break;
								}
							}
						}
						
					}
				}
			}

			bool fuckOver = false;

			for (int i = 0; i < note->heldTilings.size(); i++)
			{
				holdTile& tile = note->heldTilings[i];

				bool condition = false;

				if (downscroll)
					condition = tile.rect.y > receptors[note->lane]->y - 15;
				else
					condition = tile.rect.y < receptors[note->lane]->y + 15;

				if (!tile.active && condition)
				{
					note->heldTilings.erase(
							std::remove_if(note->heldTilings.begin(), note->heldTilings.end(), [&](holdTile& const nn) {
								return nn.beat == tile.beat;
								}),
							note->heldTilings.end());
				}
				auto whHold = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat)) * 1000;
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

			if (note->lane >= 0)
			{
				SDL_FRect receptorRect;
				receptorRect.w = 64;
				receptorRect.h = 64;
				receptorRect.x = receptors[note->lane]->x;
				receptorRect.y = receptors[note->lane]->y;
				note->draw(positionInSong, beat, receptorRect);
			}
			
			if (wh - positionInSong <= -Judge::hitWindows[4] && note->active)
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

	for (int i = 0; i < receptors.size(); i++)
	{
		if (keys[i])
			receptors[i]->light();

		receptors[i]->draw();
	}

}
void Gameplay::cleanUp()
{
	gf->destroy();
	bf->destroy();
	delete bf;
	delete gf;
	Judgement->destroy();
	Combo->destroy();
	//Accuracy->destroy();
	positionAndBeats->destroy();

	for (int i = 0; i < spawnedNotes.size(); i++)
	{
		spawnedNotes[i]->destroy();
	}

	for (int i = 0; i < receptors.size(); i++)
	{
		delete receptors[i];
	}

	spawnedNotes.clear();
	notesToPlay.clear();

	for (std::map<std::string, SDL_Texture*>::iterator iter = avatars.begin(); iter != avatars.end(); ++iter)
	{
		std::string k = iter->first;
		SDL_DestroyTexture(avatars[k]);
	}

	BASS_ChannelFree(channel);
	BASS_ChannelStop(tempostream);
	BASS_ChannelFree(tempostream);
	BASS_ChannelFree(clap);
	if (background)
		SDL_DestroyTexture(background);
}

void Gameplay::keyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			if (MultiplayerLobby::inLobby)
				return;
			SongSelect::currentChart->destroy();
			cleanUp();
			Game::instance->switchMenu(new MainMenu());
			return;
		case SDLK_F1:
			if (MultiplayerLobby::inLobby)
				return;
			botplay = !botplay;
			return;
		case SDLK_BACKQUOTE:
			if (MultiplayerLobby::inLobby)
				return;
			cleanUp();
			Game::instance->switchMenu(new Gameplay());
			return;
	}

	// hit notes

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

				const auto wh = SongSelect::currentChart->getTimeFromBeat(object->beat, SongSelect::currentChart->getSegmentFromBeat(object->beat)) * 1000;

				float diff = std::abs(wh - positionInSong);

				if (diff <= Judge::hitWindows[4] && diff < currentDiff && object->lane == control.lane)
				{
					closestObject = object;
					currentDiff = diff;
				}
			}

			if (!closestObject)
				continue;

			const auto wh = SongSelect::currentChart->getTimeFromBeat(closestObject->beat, SongSelect::currentChart->getSegmentFromBeat(closestObject->beat)) * 1000;

			float diff = (wh - positionInSong) - Game::save->GetDouble("offset");

			if (closestObject->active && diff <= Judge::hitWindows[4] && diff > -Judge::hitWindows[4])
			{
				if (Game::save->GetBool("hitsounds"))
					BASS_ChannelPlay(clap, true);
				
				bf->loop = false;
				switch (closestObject->lane % 4)
				{
				case 0:
					bf->play("BF NOTE LEFT", 24, 9, -7);
					break;
				case 1:
					bf->play("BF NOTE DOWN", 24, -16, -40);
					break;
				case 2:
					bf->play("BF NOTE UP", 24, -31, 23);
					break;
				case 3:
					bf->play("BF NOTE RIGHT", 24, -34, -5);
					break;
				}

				closestObject->active = false;
				if (closestObject->type != Note_Head)
					removeNote(closestObject);

				judgement judge = Judge::judgeNote(diff);

				std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				if (MultiplayerLobby::inLobby)
				{
					CPacketNoteHit hit;
					noteId++;
					hit.NoteID = noteId;
					hit.NoteTiming = diff;
					hit.Order = 0;
					hit.PacketType = eCPacketNoteHit;

					Multiplayer::sendMessage<CPacketNoteHit>(hit);
				}
				(*Judgement).color.r = 255;
				(*Judgement).color.g = 255;
				(*Judgement).color.b = 255;

				switch (judge)
				{
					case judgement::Judge_marvelous:
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 255;
						Judgement->setText("Marvelous (" + format + "ms)");
						Marvelous++;
						updateAccuracy(1);
						break;
					case judgement::Judge_perfect:
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Judgement->setText("Perfect (" + format + "ms)");
						Perfect++;
						updateAccuracy(0.88);
						break;
					case judgement::Judge_great:
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Judgement->setText("Great (" + format + "ms)");
						Great++;
						updateAccuracy(0.7);
						break;
					case judgement::Judge_good:
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Judgement->setText("Eh (" + format + "ms)");
						Eh++;
						updateAccuracy(0.35);
						break;
					case judgement::Judge_bad:
						combo = 0;
						(*Judgement).color.r = 128;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Judgement->setText("Yikes (" + format + "ms) ");
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
