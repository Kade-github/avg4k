#include "Gameplay.h"
#include "SongSelect.h"
#include "MultiplayerLobby.h"
#include "CPacketHostEndChart.h"
#include <chrono>
#include "TweenManager.h"


std::mutex weirdPog;

noteskin_asset* Gameplay::noteskin;

std::map<std::string, AvgSprite*> avatars;

float Gameplay::rate = 1;

void CALLBACK bruh(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
}

void Gameplay::initControls()
{
	controls.clear();

	gameplayControl left;
	left.code = Game::save->GetDouble("Left Key");
	left.lane = 0;
	gameplayControl down;
	down.code = Game::save->GetDouble("Down Key");
	down.lane = 1;
	gameplayControl up;
	up.code = Game::save->GetDouble("Up Key");
	up.lane = 2;
	gameplayControl right;
	right.code = Game::save->GetDouble("Right Key");
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
	MUTATE_START
	notesPlayed++;
	notesHit += hitWorth;
	accuracy = ((notesHit / notesPlayed) * 100);
	if (accuracy < 0)
		accuracy = 0;

	std::string format = std::to_string((double)((int)(accuracy * 100)) / 100);
	format.erase(format.find_last_not_of('0') + 1, std::string::npos);

	if (floor(accuracy) == accuracy)
		format.erase(format.find_last_not_of('.') + 1, std::string::npos);

	Accuracy->setText("Accuracy: " + format + "%");
	Mrv->setText("Marvelous: " + std::to_string(Marvelous));
	Prf->setText("Perfect: " + std::to_string(Perfect));
	God->setText("Great: " + std::to_string(Great));
	Ehh->setText("Eh: " + std::to_string(Eh));
	Yke->setText("Yikes: " + std::to_string(Yikes));
	MUTATE_END
}

void Gameplay::removeNote(NoteObject* object)
{
	spawnedNotes.erase(
		std::remove_if(spawnedNotes.begin(), spawnedNotes.end(), [&](NoteObject* const nn) {
			return nn->beat == object->beat && nn->lane == object->lane;
			}),
		spawnedNotes.end());

	colGroups[object->lane]->children.erase(colGroups[object->lane]->children.begin());
}

void Gameplay::miss(NoteObject* object)
{
	if (MultiplayerLobby::inLobby)
	{
		MUTATE_START
		noteId++;
		CPacketNoteHit hit;
		hit.NoteID = noteId;
		hit.NoteTiming = Judge::hitWindows[4];
		hit.Order = 0;
		hit.PacketType = eCPacketNoteHit;

		Multiplayer::sendMessage<CPacketNoteHit>(hit);
		MUTATE_END
	}

	Misses++;
	updateAccuracy(-0.4);
	combo = 0;
	Judgement->color.r = 255;
	Judgement->color.g = 0;
	Judgement->color.b = 0;
	Judgement->setText("MISS");

	Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
	Judgement->setY((Game::gameHeight / 2));


	Combo->setText("  ");
	Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
	Combo->setY((Game::gameHeight / 2) + 40);	
}

void Gameplay::onPacket(PacketType pt, char* data, int32_t length)
{
	MUTATE_START

	if (!MultiplayerLobby::inLobby)
		return;

	SPacketUpdateLeaderboard pack;
	SPacketFinalizeChart ch;

	msgpack::unpacked result;

	msgpack::object obj;
	std::map<std::string, AvgSprite*> sprites;
	std::map<std::string, AvgSprite*>::iterator it;
	leaderboardSpot cspot;
	std::vector<std::string> ids = {};

	std::vector<leaderboardSpot> copy = leaderboard;

	int ranking = 0;

	switch (pt)
	{
	case eSPacketUpdateLeaderboard:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(pack);

		for (PlayerScore score : pack.orderedScores)
		{
			bool found = false;
			for (leaderboardSpot& spot : leaderboard)
			{
				if (spot.score.SteamID64 == score.SteamID64)
				{
					std::string username = "";
					if (score.Username.size() > 6)
						username = score.Username.substr(0, 6) + "...";
					else
						username = score.Username;				
					spot.score = score; // copy it over
					found = true;
					spot.t->y = ((Game::gameHeight / 2) + (46 * ranking)) + 4;
					spot.t->setText(username + ": " + std::to_string(spot.score.score));
					spot.avgRect->w = 300;
					spot.avgRect->y = ((Game::gameHeight / 2) + (46 * ranking));
				}
			}
			if (!found)
			{
				int y = (Game::gameHeight / 2) + (46 * ranking);
				cspot.avgRect = new AvgRect(0, y, 1, 46);
				add(cspot.avgRect);
				std::string username = "";
				if (score.Username.size() > 6)
					username = score.Username.substr(0, 6) + "...";
				else
					username = score.Username;
				cspot.score = score;
				cspot.avgRect->alpha = 0.3;
				cspot.t = new Text(50, y + 4, username + ": " + std::to_string(cspot.score.score), 24, "NotoSans-Regular");
				cspot.avgRect->w = 300;
				leaderboard.push_back(cspot);
				add(cspot.t);
			}
			ranking++;
		}


		for (it = avatars.begin(); it != avatars.end(); it++)
		{
			it->second->x = -1000;
		}

		for (leaderboardSpot& spot : leaderboard)
		{
			avatars[spot.score.SteamID64]->y = spot.avgRect->y;
			avatars[spot.score.SteamID64]->x = 0;
			add(avatars[spot.score.SteamID64]);
		}
		break;
	case eSPacketFinalizeChart:

		leaderboard.clear();

		SongSelect::currentChart->destroy();
		cleanUp();
		std::cout << "go back" << std::endl;

		MultiplayerLobby* lob = new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, false);

		Game::instance->transitionToMenu(lob);
		break;
	}
	MUTATE_END
}

Gameplay::Gameplay()
{
}

void Gameplay::create() {

	MUTATE_START
	addCamera(Game::mainCamera);
	initControls();

	avatars.clear();

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
		Channel* c = SoundManager::getChannelByName("prevSong");
		c->stop();
		c->free();
		SoundManager::removeChannel("prevSong");
	}

	downscroll = Game::save->GetBool("downscroll");

	Judge::initJudge();

	Judge::judgeNote(174);



	std::string bg = SongSelect::currentChart->meta.folder + "/" + SongSelect::currentChart->meta.background;

	background = new AvgSprite(0, 0, bg);
	background->w = Game::gameWidth;
	background->h = Game::gameHeight;
	add(background);

	if (MultiplayerLobby::inLobby)
		for (int i = 0; i < MultiplayerLobby::CurrentLobby.PlayerList.size(); i++)
		{
			player pp = MultiplayerLobby::CurrentLobby.PlayerList[i];
			const char* pog = pp.AvatarURL.c_str();
			Texture* s = Steam::getAvatar(pog);
			avatars[pp.SteamID64] = new AvgSprite(0, 0, s);
			avatars[pp.SteamID64]->w = 46;
			avatars[pp.SteamID64]->h = 46;
			add(avatars[pp.SteamID64]);
		}


	float bassRate = (rate * 100) - 100;

	std::string path = SongSelect::currentChart->meta.folder + "/" + SongSelect::currentChart->meta.audio;

	std::cout << "playing " << path << std::endl;

	song = SoundManager::createChannel(path.c_str(), "gameplaySong");
	clap = SoundManager::createChannel("assets/sounds/hitSound.mp3", "clapFx");

	song->createFXStream();

	songLength = song->length;

	int diff = SongSelect::selectedDiffIndex;

	positionAndBeats = new Text(0, 40, "", 16, "NotoSans-Regular");
	positionAndBeats->create();
	add(positionAndBeats);


	noteskin = Noteskin::getNoteskin();
	notesToPlay = SongSelect::currentChart->meta.difficulties[diff].notes;

	Rect laneUnderway;

	laneUnderway.x = ((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * 3) - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size"));
	laneUnderway.h = 1280;

	AvgRect* lunder = new AvgRect(laneUnderway.x, laneUnderway.y, laneUnderway.w, laneUnderway.h);
	add(lunder);
	lunder->alpha = 0.5;


	Judgement = new Text(Game::gameWidth / 2, Game::gameHeight / 2, " ", 24, "NotoSans-Regular");
	Judgement->create();

	Combo = new Text(Game::gameWidth / 2, Game::gameHeight / 2 + 40, " ", 24, "NotoSans-Regular");
	Combo->create();

	AvgRect* lAcc = new AvgRect(171, (Game::gameHeight / 2) - 304, 250, 295);
	lAcc->alpha = 0.5;
	add(lAcc);

	Accuracy = new Text(175, (Game::gameHeight / 2) - 300, "Accuracy: N/A", 24, "NotoSans-Regular");
	Accuracy->create();
	add(Accuracy);

	Mrv = new Text(175, (Game::gameHeight / 2) - 250, "Marvelous: " + std::to_string(Marvelous), 24, "NotoSans-Regular");
	Mrv->create();
	add(Mrv);

	Prf = new Text(175, (Game::gameHeight / 2) - 200, "Perfect: " + std::to_string(Perfect), 24, "NotoSans-Regular");
	Prf->create();
	add(Prf);

	God = new Text(175, (Game::gameHeight / 2) - 150, "Great: " + std::to_string(Great), 24, "NotoSans-Regular");
	God->create();
	add(God);

	Ehh = new Text(175, (Game::gameHeight / 2) - 100, "Eh: " + std::to_string(Eh), 24, "NotoSans-Regular");
	Ehh->create();
	add(Ehh);

	Yke = new Text(175, (Game::gameHeight / 2) - 50, "Yikes: " + std::to_string(Yikes), 24, "NotoSans-Regular");
	Yke->create();
	add(Yke);



	for (int i = 0; i < 4; i++)
	{
		ReceptorObject* r;

		int index = i + 1;
		if (downscroll)
			r = new ReceptorObject(
				((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * i), (Game::gameHeight / 2) + 250, i);
		else
			r = new ReceptorObject(
				((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * i), (Game::gameHeight / 2) - 300, i);
		r->lightUpTimer = 0;
		r->create();
		receptors.push_back(r);
		add(r);
	}

	for (int i = 0; i < receptors.size(); i++)
	{
		// create a group
		ReceptorObject* r = receptors[i];
		AvgGroup* group = new AvgGroup(r->x, 0, r->w, Game::gameHeight);
		colGroups.push_back(group);
		add(group);
	}

	add(Judgement);
	add(Combo);

	songPosBar = new AvgRect(receptors[0]->x, 24, ((receptors[3]->x + (64 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength)), 24);
	add(songPosBar);
	songPosBar->c.r = 255;
	songPosBar->c.g = 255;
	songPosBar->c.b = 255;

	if (downscroll)
		songPosBar->y = (receptors[0]->y + (64 * Game::save->GetDouble("Note Size"))) + 12;

	songPosOutline = new AvgRect(songPosBar->x,songPosBar->y, (receptors[3]->x + (64 * Game::save->GetDouble("Note Size"))) - receptors[0]->x, 24);
	songPosOutline->border = true;
	//add(songPosOutline);
	songPosOutline->c.r = 255;
	songPosOutline->c.g = 255;
	songPosOutline->c.b = 255;
	created = true;

	positionInSong = -1500;
	MUTATE_END
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float lastBPM = 0;

void Gameplay::update(Events::updateEvent event)
{
	MUTATE_START
	if (positionInSong >= startTime)
	{
		if (!play)
		{
			song->setPos(0);
			song->setVolume(0.6);
			song->play();
			play = true;
			lastBPM = 0;
		}

		positionInSong = song->getPos();
	}
	else
		positionInSong += Game::deltaTime;

	SDL_FRect bruh;
	bruh.x = 0;
	bruh.y = 0;
	bruh.h = 720;
	bruh.w = 1280;

	Rect ll;
	ll.y = 0;
	ll.w = 64 * Game::save->GetDouble("Note Size");
	ll.h = Game::gameHeight;

	int indexG = 0;
	for (AvgGroup* group : colGroups) // reset clips
	{
		ll.x = receptors[indexG]->x;
		group->clipRect = ll;
		indexG++;
	}

	if (Judgement->scale > 1.0)
	{
		Combo->scale = lerp(1.25, 1, scaleStart / scaleTime);
		Judgement->scale = lerp(1.25, 1, scaleStart / scaleTime);
		scaleStart += Game::deltaTime;
	}

	if (BASS_ErrorGetCode() != 0)
		Combo->setText(std::to_string(BASS_ErrorGetCode()) + "_bassError");

	SDL_FRect laneUnderway;

	laneUnderway.x = receptors[0]->x - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (receptors[3]->x - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size"));
	laneUnderway.h = 1280;

	

	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 150);
	//SDL_RenderFillRectF(Game::renderer, &laneUnderway);
	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);


	if (!SongSelect::currentChart)
		return;

	curSeg = SongSelect::currentChart->getSegmentFromTime(positionInSong - Game::save->GetDouble("offset"));
	beat = SongSelect::currentChart->getBeatFromTimeOffset(positionInSong - Game::save->GetDouble("offset"), curSeg);
	if (lastBPM != curSeg.bpm && Game::gameplayEvents_DB)
	{
		Game::instance->db_addLine("bpm change to " + std::to_string(curSeg.bpm));
		song->bpm = curSeg.bpm;
		lastBPM = curSeg.bpm;
	}


	if (Game::save->GetBool("Annoying bopping"))
	{
		if ((int)beat % 1 == 0 && lastbeat != (int)beat && beat > 0)
		{
			lastbeat = (int)beat;
			drop = 20000 / curSeg.bpm;
			cam->scale = 1.05;
		}

		if (cam->scale > 1.0)
		{
			float var = Game::deltaTime;
			drop -= var * 0.6;
			cam->scale = lerp(1.0, 1.05, drop / (20000 / curSeg.bpm));
		}
	}

	songPosBar->w = ((receptors[3]->x + (64 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength));

	// underlay for accuracy

	SDL_FRect overlayForAccuracy;

	overlayForAccuracy.x = Accuracy->x - 4;
	overlayForAccuracy.y = (Game::gameHeight / 2) - 302;
	overlayForAccuracy.w = Accuracy->surfH;
	overlayForAccuracy.h = Accuracy->surfH;


	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 128);
	//SDL_RenderFillRectF(Game::renderer, &overlayForAccuracy);
	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);

	// multiplayer shit

	// leaderboard

	//SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

	//SDL_RenderFillRectF(Game::renderer, &posBar);

	//SDL_RenderDrawRectF(Game::renderer, &outline);

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
			object->wasHit = false;
			object->clapped = false;
			object->active = true;
			object->beat = n.beat;
			object->lane = n.lane;
			object->type = n.type;
			object->endTime = -1;
			object->endBeat = -1;

			bpmSegment noteSeg = SongSelect::currentChart->getSegmentFromBeat(object->beat);

			object->time = SongSelect::currentChart->getTimeFromBeatOffset(object->beat, noteSeg);
			rect.y = Game::gameHeight + 400;
			rect.x = 0;
			rect.w = 64 * Game::save->GetDouble("Note Size");
			rect.h = 64 * Game::save->GetDouble("Note Size");
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

					object->endTime = SongSelect::currentChart->getTimeFromBeatOffset(nn.beat, noteSeg);
					tail = nn;
					break;
				}
			}

			notesToPlay.erase(notesToPlay.begin());

			float time = SDL_GetTicks();

			if (object->type == Note_Head)
			{

				for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
				{
					bpmSegment holdSeg = SongSelect::currentChart->getSegmentFromTime(i);

					double beat = SongSelect::currentChart->getBeatFromTimeOffset(i, holdSeg);

					float whHold = SongSelect::currentChart->getTimeFromBeatOffset(beat, holdSeg);

					float diff = whHold - (object->time);

					float noteOffset = (bps * (diff / 1000)) * (64 * Game::save->GetDouble("Note Size"));

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
						otherOne = yDiff <= -(64 * Game::save->GetDouble("Note Size"));
					else
						otherOne = yDiff >= 64 * Game::save->GetDouble("Note Size");

					if (otherOne || object->heldTilings.size() == 0)
					{
						object->holdHeight += 64 * Game::save->GetDouble("Note Size");
						holdTile tile;
						SDL_FRect rect;
						tile.active = true;
						tile.fucked = false;
						rect.y = y;
						rect.x = 0;
						rect.w = 64 * Game::save->GetDouble("Note Size");
						rect.h = 68 * Game::save->GetDouble("Note Size");
						tile.rect = rect;
						tile.beat = beat;
						tile.time = i;
						object->heldTilings.push_back(tile);
					}
				}
			}
			std::sort(object->heldTilings.begin(), object->heldTilings.end());
			
			spawnedNotes.push_back(object);
			object->create();
			colGroups[object->lane]->add(object);
		}
	}
	else
	{
		if (!ended && spawnedNotes.size() == 0 && positionInSong > (songLength) - 1000)
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

	for (int i = 0; i < receptors.size(); i++)
	{
		if (keys[i] || holding[i])
		{
			receptors[i]->light();
			if (noteskin->shrink)
				receptors[i]->scale = 0.85;
		}
		else
		{
			if (receptors[i]->scale < 1.0 && noteskin->shrink)
			{
				receptors[i]->scale += Game::deltaTime * 0.04;
				if (receptors[i]->scale > 1.0)
					receptors[i]->scale = 1;
			}
		}
	}

	{
		for (int i = 0; i < spawnedNotes.size(); i++)
		{
			NoteObject* note = spawnedNotes[i];

			if (!note->destroyed)
			{
				float wh = SongSelect::currentChart->getTimeFromBeat(note->beat, SongSelect::currentChart->getSegmentFromBeat(note->beat));

				if (wh - positionInSong < 2)
				{
					if (Game::save->GetBool("hitsounds") && !note->clapped)
					{
						note->clapped = true;
						if (SoundManager::getChannelByName("clapFx") != NULL)
							clap->play();
					}
					if (botplay && note->active)
					{
						float diff = (wh - positionInSong);

						std::string format = std::to_string(diff - fmod(diff, 0.01));
						format.erase(format.find_last_not_of('0') + 1, std::string::npos);

						Judgement->setText("Botplay (" + format + "ms)");
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 255;
						Marvelous++;
						updateAccuracy(1);
						note->active = false;

						combo++;
						if (noteskin->bounce)
						{
							Judgement->scale = 1.15;
							Combo->scale = 1.15;
							scaleTime = 350;
							scaleStart = 0;
						}

						Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
						Judgement->setY((Game::gameHeight / 2));


						Combo->setText(std::to_string(combo));
						Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
						Combo->setY((Game::gameHeight / 2) + 40);
					}

				}

				// BEFORE DRAW, CHECK HELD NOTES!!

				if (note->type == Note_Head)
				{
					if (holding[note->lane] || botplay) // holding that lane!
					{
						for (int i = 0; i < note->heldTilings.size(); i++)
						{
							holdTile& tile = note->heldTilings[i];
							float wh = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
							float offset = wh;
							if (offset - positionInSong <= Judge::hitWindows[3] && !tile.fucked)
							{
								tile.active = false;
							}
						}
					}
				}

				bool fuckOver = false;

				for (int i = 0; i < note->heldTilings.size(); i++)
				{
					holdTile& tile = note->heldTilings[i];
					if (fuckOver)
						tile.fucked = true;
				}

				if (note->lane < 4 && note->lane >= 0)
				{
					Rect receptorRect;
					receptorRect.w = receptors[note->lane]->w;
					receptorRect.h = receptors[note->lane]->h;
					receptorRect.x = receptors[note->lane]->x;
					receptorRect.y = receptors[note->lane]->y;
					note->debug = debug;

					Rect l;
					l.x = receptorRect.x;
					l.y = (receptorRect.y + 40);
					l.w = Game::gameWidth;
					l.h = Game::gameHeight - (receptorRect.y + 32);
					if (downscroll)
					{
						l.y = 0;
						l.h = receptorRect.y + 48;
					}

					//SDL_SetRenderTarget(Game::renderer, Game::mainCamera->cameraTexture);

					if (keys[note->lane] || botplay)
					{
						colGroups[note->lane]->clipRect = l;
					}

				}

				if (wh - positionInSong <= -Judge::hitWindows[4] && note->active)
				{
					note->active = false;
					miss(note);
				}

				bool condition = true;


				if ((wh - positionInSong <= -200 && !note->active) && note->holdsActive == 0)
				{
					removeNote(note);
				}

				for (int i = 0; i < note->heldTilings.size(); i++)
				{
					holdTile& tile = note->heldTilings[i];

					float whHold = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
					float diff = whHold - positionInSong;

					if (diff < -Judge::hitWindows[2] && tile.active)
					{
						std::cout << note->lane << " fucked " << diff << " time: " << whHold << " song: " << positionInSong << std::endl;
						miss(note);
						removeNote(note);
						break;
					}
				}
			}
		}
	}
	MUTATE_END
}
void Gameplay::cleanUp()
{
	colGroups.clear();
	spawnedNotes.clear();
	notesToPlay.clear();

	//for (std::map<std::string, SDL_Texture*>::iterator iter = avatars.begin(); iter != avatars.end(); ++iter)
	//{
	//	std::string k = iter->first;
	//	SDL_DestroyTexture(avatars[k]);
	//}

	song->free();
	clap->free();

	SoundManager::removeChannel("gameplaySong");
	SoundManager::removeChannel("clapFx");

	
	//if (background)
	//	SDL_DestroyTexture(background);
}

void Gameplay::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START

	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			
			if (MultiplayerLobby::inLobby) {
				CPacketHostEndChart end;
				end.Order = 0;
				end.PacketType = eCPacketHostEndChart;

				Multiplayer::sendMessage<CPacketHostEndChart>(end);
				return;
			}
			SongSelect::currentChart->destroy();
			cleanUp();
			Game::instance->transitionToMenu(new MainMenu());
		
			return;
		case SDLK_F1:
			if (MultiplayerLobby::inLobby)
				return;
			botplay = !botplay;
			return;
		case SDLK_F2:
			debug = !debug;
			break;
		case SDLK_BACKQUOTE:
			if (MultiplayerLobby::inLobby)
				return;
			cleanUp();
			Game::instance->transitionToMenu(new Gameplay());
			return;
	}

	// hit notes

	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == event.keysym.sym)
		{
			// note lol

			NoteObject* closestObject = nullptr;
			float currentDiff = Judge::hitWindows[4] + 1;


			for (int n = 0; n < spawnedNotes.size(); n++)
			{
				NoteObject* object = spawnedNotes[n];

				const auto wh = SongSelect::currentChart->getTimeFromBeat(object->beat, SongSelect::currentChart->getSegmentFromBeat(object->beat));

				float diff = std::abs(wh - positionInSong);

				if (diff <= Judge::hitWindows[4] && diff < currentDiff && object->lane == control.lane)
				{
					closestObject = object;
					currentDiff = diff;
				}
			}

			if (!keys[control.lane])
			{
				keys[control.lane] = true;
				if (closestObject)
					if (closestObject->holdsActive > 0)
						holding[control.lane] = true;

			}
			else
				continue;


			if (!closestObject)
				continue;

			float wh = SongSelect::currentChart->getTimeFromBeat(closestObject->beat, SongSelect::currentChart->getSegmentFromBeat(closestObject->beat));

			float diff = (wh - positionInSong);

			if (closestObject->active && diff <= Judge::hitWindows[4] && diff > -Judge::hitWindows[4])
			{
				closestObject->active = false;
				closestObject->wasHit = true;

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
						updateAccuracy(0.925);
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
					if (noteskin->bounce)
					{
						Judgement->scale = 1.15;
						Combo->scale = 1.15;
						scaleTime = 350;
						scaleStart = 0;
					}
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
	
	MUTATE_END
}

void Gameplay::keyUp(SDL_KeyboardEvent ev)
{
	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == ev.keysym.sym)
		{
			keys[control.lane] = false;
			holding[control.lane] = false;
		}
	}
}
