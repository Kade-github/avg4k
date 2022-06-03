#include "Gameplay.h"
#include "SongSelect.h"
#include "MultiplayerLobby.h"
#include "CPacketHostEndChart.h"
#include <chrono>
#include "TweenManager.h"
#include "MainerMenu.h"


std::mutex weirdPog;


std::map<std::string, AvgSprite*> avatars;

float Gameplay::rate = 1;

void CALLBACK bruh(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
}

void Gameplay::initControls()
{
	controls.clear();

	std::string keybinds = Game::save->GetString("Keybinds");

	for (int i = 0; i < keybinds.size(); i++)
	{
		gameplayControl ctrl;
		ctrl.code = std::tolower(keybinds[i]);
		ctrl.lane = i;
		controls.push_back(ctrl);
	}
}

const float min(const float a, const float b)
{
	return (b < a) ? b : a;
}

void Gameplay::updateAccuracy(double hitWorth)
{
	MUTATE_START
	if (hitWorth != 0)
	{
		notesPlayed++;
		notesHit += hitWorth;
		accuracy = ((notesHit / notesPlayed) * 100);
	}
	else
		accuracy = 100;
	if (accuracy < 0)
		accuracy = 0;

	std::string format = std::to_string((double)((int)(accuracy * 100)) / 100);
	format.erase(format.find_last_not_of('0') + 1, std::string::npos);

	if (accuracy == (int)accuracy)
		format.erase(format.find_last_not_of('.') + 1, std::string::npos);

	ScoreText->setText(std::to_string(score));
	ScoreText->x = (Game::gameWidth - ScoreText->surfW) - 24;

	Accuracy->setText(format + "%");
	Accuracy->x = (Game::gameWidth - Accuracy->surfW) - 24;

	// calculate rank here

	std::string rank = "";

	if (hitWorth != 0)
	{
		if (accuracy == 100)
			rank = "S";
		else if (accuracy >= 96)
			rank = "A+";
		else if (accuracy >= 93)
			rank = "A";
		else if (accuracy >= 80)
			rank = "B";
		else if (accuracy >= 70)
			rank = "C";
		else if (accuracy >= 60)
			rank = "D";
		else if (accuracy <= 59)
			rank = "F";
	}

	ranking->setText(rank);
	ranking->x = (Game::gameWidth - ranking->surfW) - 24;

	MUTATE_END
}

void Gameplay::removeNote(NoteObject* object)
{
	spawnedNotes.erase(
		std::remove_if(spawnedNotes.begin(), spawnedNotes.end(), [&](NoteObject* const nn) {
			return nn->beat == object->beat && nn->lane == object->lane;
			}),
		spawnedNotes.end());
	object->connected->killed = true;
	removeObj(object);

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
	Judgement->setText("miss");

	Judgement->setX((Game::gameWidth / 2) - (Judgement->surfW / 2));
	Judgement->setY((Game::gameHeight / 2));


	Combo->setText("  ");
	Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
	Combo->setY((Game::gameHeight / 2) + 40);	
}

const char* ordinal_suffix(int n)
{
	static const char suffixes[][3] = { "th", "st", "nd", "rd" };
	auto ord = n % 100;
	if (ord / 10 == 1) { ord = 0; }
	ord = ord % 10;
	if (ord > 3) { ord = 0; }
	return suffixes[ord];
}

void Gameplay::onPacket(PacketType pt, char* data, int32_t length)
{
	MUTATE_START

	if (!MultiplayerLobby::inLobby || !created)
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

	int rankin = 0;

	switch (pt)
	{
	case eSPacketUpdateLeaderboard:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(pack);

		for (PlayerScore score : pack.orderedScores)
		{
			if (score.SteamID64 == std::to_string(SteamUser()->GetSteamID().ConvertToUint64()))
			{
				int realRank = rankin + 1;
				Placement->setText(std::to_string(realRank) + std::string(ordinal_suffix(realRank)) + " Place");
				Placement->x = (Game::gameWidth - Placement->surfW) - 24;
				Game::DiscordUpdatePresence(MainerMenu::currentSelectedSong.meta.songName, "Playing Multiplayer (" + Placement->text + ")", "Average4K", MultiplayerLobby::CurrentLobby.Players, MultiplayerLobby::CurrentLobby.MaxPlayers, "");
			}
			bool found = false;
			int index = 0;
			for (leaderboardSpot& spot : leaderboard)
			{
				if (spot.score.SteamID64 == score.SteamID64)
				{
					if (rankin >= 10)
					{
						removeObj(spot.accuracy);
						removeObj(spot.scoreText);
						removeObj(spot.owner);
						removeObj(spot.t);
						leaderboard.erase(leaderboard.begin() + index);
						continue;
					}
					std::string username = "";
					if (score.Username.size() > 14)
						username = score.Username.substr(0, 14) + "...";
					else
						username = score.Username;
					spot.score = score; // copy it over
					found = true;
					int y = ((leaderboardText->y + leaderboardText->h) + 24) + ((82 * rankin) + 4);

					if (spot.rankin > rankin)
					{
						leaderboardhighlight high;
						high.rect = new AvgRect(0, y, 227, 82);
						high.time = 0;
						high.spot = rankin;
						high.rect->alpha = 0.8;
						high.rect->c = { 255,255,255 };
						add(high.rect);
						highlights.push_back(high);
					}

					if (rankin != spot.rankin)
						Tweening::TweenManager::createNewTween("scoreboard" + score.SteamID64, spot.t, Tweening::tt_Y, 750, spot.t->y,y,NULL, Easing::EaseInCubic);
					spot.rankin = rankin;

					std::string format = "0";
					if (score.Accuracy != 0)
					{
						format = std::to_string((double)((int)(score.Accuracy * 10000)) / 100);
						format.erase(format.find_last_not_of('0') + 1, std::string::npos);

						if (score.Accuracy == (int)score.Accuracy)
							format.erase(format.find_last_not_of('.') + 1, std::string::npos);
					}

					spot.accuracy->setText(format + "% accuracy");
					spot.scoreText->setText(std::to_string(score.score));
					spot.t->setText(username);
				}
				index++;
			}
			if (!found && rankin < 10)
			{
				int y = ((leaderboardText->y + leaderboardText->h) + 24) + ((82 * rankin) + 4);
				std::string username = "";
				cspot.rankin = rankin;
				if (score.Username.size() > 14)
					username = score.Username.substr(0, 14) + "...";
				else
					username = score.Username;
				cspot.score = score;
				float off = (avatars[cspot.score.SteamID64]->w + 4);
				cspot.t = new Text(7 + off, y + 4, username, 16, "arial");
				cspot.owner = new Text(7 + off, cspot.t->y + cspot.t->surfH, "", 16, "arialbd");
				if (MultiplayerLobby::hostSteamId == score.SteamID64)
					cspot.owner->setText("lobby owner");
				else
				{
					cspot.owner->text = "";
					cspot.owner->surfH = cspot.t->surfH;
					cspot.owner->y = cspot.t->y;
				}
				std::string format = "0";
				if (score.Accuracy != 0)
				{
					format = std::to_string((double)((int)(score.Accuracy * 10000)) / 100);
					format.erase(format.find_last_not_of('0') + 1, std::string::npos);

					if (score.Accuracy == (int)score.Accuracy)
						format.erase(format.find_last_not_of('.') + 1, std::string::npos);
				}

				cspot.accuracy = new Text(7 + off,cspot.owner->y + cspot.owner->surfH, format + "% accuracy", 16, "ariali");


				cspot.scoreText = new Text(7 + off, cspot.accuracy->y + cspot.accuracy->surfH, std::to_string(score.score), 16, "arialbd");
				leaderboard.push_back(cspot);
				add(cspot.accuracy);
				add(cspot.scoreText);
				add(cspot.owner);
				add(cspot.t);

				if (rankin == 0)
				{
					leaderboardCrown->x = -23;
					leaderboardCrown->y = (cspot.t->y - cspot.t->surfH) - 10;
				}

				cspot.accuracy->setCharacterSpacing(3);
				cspot.scoreText->setCharacterSpacing(3);
				cspot.owner->setCharacterSpacing(3);
				cspot.t->setCharacterSpacing(3);
			}
			rankin++;
		}


		for (it = avatars.begin(); it != avatars.end(); it++)
		{
			it->second->x = -1000;
		}

		for (leaderboardSpot& spot : leaderboard)
		{
			avatars[spot.score.SteamID64]->y = spot.t->y;
			avatars[spot.score.SteamID64]->x = 3;
			//add(avatars[spot.score.SteamID64]);
		}
		break;
	case eSPacketFinalizeChart:

		leaderboard.clear();

		MainerMenu::currentSelectedSong.destroy();
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

	if (MainerMenu::currentSelectedSong.meta.difficulties.size() == 0)
	{
		if (!MultiplayerLobby::inLobby)
		{
			Game::instance->transitionToMenu(new MainerMenu());
		}
		else
		{
			Game::instance->transitionToMenu(new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost,false));
		}
		return;
	}

	std::string bg = MainerMenu::currentSelectedSong.meta.folder + "/" + MainerMenu::currentSelectedSong.meta.background;

	background = new AvgSprite(0, 0, bg);
	background->w = Game::gameWidth;
	background->h = Game::gameHeight;

	background->alpha = Game::save->GetDouble("Background Transparency");

	add(background);

	if (background->tex->pixels)
	{
		lightestColor = stbi_h::getLightestPixel(background->tex->pixels, background->tex->width, background->tex->height);
		darkestColor = stbi_h::getDarkestPixel(background->tex->pixels, background->tex->width, background->tex->height);

		darkestColor.r *= 0.46;
		darkestColor.g *= 0.46;
		darkestColor.b *= 0.46;

		if (Game::save->GetBool("Auto Accent Colors"))
		{

			darkestAccent = stbi_h::getAccentPixel(background->tex->pixels, background->tex->width, background->tex->height, lightestColor, darkestColor);
			Pixel p;
			p.r = darkestAccent.r += 20;
			p.g = darkestAccent.g += 20;
			p.b = darkestAccent.b += 20;
			lighterAccent = stbi_h::getAccentPixel(background->tex->pixels, background->tex->width, background->tex->height, lightestColor, p);
			p.r = lighterAccent.r += 40;
			p.g = lighterAccent.g += 40;
			p.b = lighterAccent.b += 40;
			lightestAccent = stbi_h::getAccentPixel(background->tex->pixels, background->tex->width, background->tex->height, lightestColor, p);
		}
		else
		{
			darkestAccent = {(int)Game::save->GetDouble("Accent Color R"),(int)Game::save->GetDouble("Accent Color G"),(int)Game::save->GetDouble("Accent Color B") };
			darkestAccent.a = Game::save->GetDouble("Lane Underway Transparency");
			darkestColor = darkestAccent;
			lightestColor = darkestAccent;
			lighterAccent = darkestAccent;
			lightestAccent = darkestAccent;
		}
	}

	AvgRect* rOverlay = new AvgRect(0, 0, background->w, background->h);
	rOverlay->alpha = 0.46;
	add(rOverlay);


	float bassRate = (rate * 100) - 100;

	std::string path = MainerMenu::currentSelectedSong.meta.folder + "/" + MainerMenu::currentSelectedSong.meta.audio;

	std::cout << "playing " << path << std::endl;

	song = SoundManager::createChannel(path.c_str(), "gameplaySong");
	clap = SoundManager::createChannel("assets/sounds/hitSound.wav", "clapFx");


	song->createFXStream();

	songLength = song->length;

	int diff = MainerMenu::selectedDiffIndex;

	positionAndBeats = new Text(0, 40, "", 16, "NotoSans-Regular");
	positionAndBeats->create();
	add(positionAndBeats);


	notesToPlay = MainerMenu::currentSelectedSong.meta.difficulties[diff].notes;

	Rect laneUnderway;

	laneUnderway.x = ((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * 3) - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size"));
	laneUnderway.h = 1280;

	AvgSprite* lunder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underway.png"));
	add(lunder);
	lunder->alpha = Game::save->GetDouble("Lane Underway Transparency");

	lunder->colorR = darkestColor.r;
	lunder->colorG = darkestColor.g;
	lunder->colorB = darkestColor.b;

	AvgSprite* lunderBorder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underwayBorder.png"));
	add(lunderBorder);
	lunderBorder->alpha = 1 * (0.8 / Game::save->GetDouble("Lane Underway Transparency"));

	lunderBorder->colorR = lightestAccent.r;
	lunderBorder->colorG = lightestAccent.g;
	lunderBorder->colorB = lightestAccent.b;

	lunder->w = laneUnderway.w;
	lunder->h = laneUnderway.h;
	lunderBorder->w = laneUnderway.w;
	lunderBorder->h = laneUnderway.h;

	Judgement = new Text(Game::gameWidth / 2, Game::gameHeight / 2, " ", 23, "Futura Bold");
	Judgement->setCharacterSpacing(4.25);
	Judgement->create();

	Judgement->borderColor = { 255,255,255 };
	Judgement->border = true;
	Judgement->borderSize = 1;

	Combo = new Text(Game::gameWidth / 2, Game::gameHeight / 2 + 40, " ", 24, "Futura Bold");
	Combo->create();


	Combo->borderSize = 1;
	Combo->border = true;

	AvgSprite* rightGrad = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "rightGraid.png"));
	rightGrad->x = Game::gameWidth - rightGrad->w;

	rightGrad->colorR = darkestColor.r;
	rightGrad->colorG = darkestColor.g;
	rightGrad->colorB = darkestColor.b;
	rightGrad->alpha = Game::save->GetDouble("Lane Underway Transparency");
	add(rightGrad);

	if (MultiplayerLobby::inLobby)
	{
		Game::DiscordUpdatePresence(MainerMenu::currentSelectedSong.meta.songName, "Playing Multiplayer", "Average4K", MultiplayerLobby::CurrentLobby.Players, MultiplayerLobby::CurrentLobby.MaxPlayers, "");
		AvgSprite* leftGrad = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "leftGraid.png"));
		leftGrad->colorR = darkestColor.r;
		leftGrad->colorG = darkestColor.g;
		leftGrad->colorB = darkestColor.b;
		leftGrad->alpha = Game::save->GetDouble("Lane Underway Transparency");
		add(leftGrad);

		AvgSprite* leftGradBorder = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "leftBorder.png"));
		leftGradBorder->colorR = lightestAccent.r;
		leftGradBorder->colorG = lightestAccent.g;
		leftGradBorder->colorB = lightestAccent.b;
		leftGradBorder->alpha = (0.8 / Game::save->GetDouble("Lane Underway Transparency"));
		add(leftGradBorder);

		for (int i = 0; i < MultiplayerLobby::CurrentLobby.PlayerList.size(); i++)
		{
			player pp = MultiplayerLobby::CurrentLobby.PlayerList[i];
			const char* pog = pp.AvatarURL.c_str();
			Texture* s = Steam::getAvatar(pog);
			avatars[pp.SteamID64] = new AvgSprite(0, 0, s);
			avatars[pp.SteamID64]->w = 52;
			avatars[pp.SteamID64]->h = 52;
			add(avatars[pp.SteamID64]);
		}

		if (avatars.size() == 0)
		{
			Game::instance->transitionToMenu(new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, false));
			return;
		}
		else
		{
			for (int i = 0; i < MultiplayerLobby::CurrentLobby.PlayerList.size(); i++)
			{
				player pp = MultiplayerLobby::CurrentLobby.PlayerList[i];
				if (avatars[pp.SteamID64] == nullptr)
				{
					Game::instance->transitionToMenu(new MultiplayerLobby(MultiplayerLobby::CurrentLobby, MultiplayerLobby::isHost, false));
					return;
				}
			}
		}

		leaderboardText = new Text(15, 12, "Leaderboard", 18, "arialbd");
		add(leaderboardText);
		leaderboardText->setCharacterSpacing(3);

		leaderboardCrown = new AvgSprite(100, 100, Noteskin::getGameplayElement(Game::noteskin, "crown.png"));
		leaderboardCrown->scale = 0.3;
		leaderboardCrown->colorR = 255;
		leaderboardCrown->colorG = 209;
		leaderboardCrown->colorB = 83;
		add(leaderboardCrown);

		Placement = new Text(Game::gameWidth - 200, 10, "", 36, "Futura Bold");
		add(Placement);
		Placement->setCharacterSpacing(6);
		Placement->border = true;
		Placement->borderAlpha = 0.5;
		Placement->borderSize = 2;
	}
	else
		Game::DiscordUpdatePresence(MainerMenu::currentSelectedSong.meta.artist + " - " + MainerMenu::currentSelectedSong.meta.songName, "Playing Solo Play", "Average4K", -1, -1, "");

	ScoreText = new Text(Game::gameWidth - 200, 10, "", 36, "Futura Bold");
	add(ScoreText);
	ScoreText->setCharacterSpacing(6);


	Accuracy = new Text(Game::gameWidth - 200, 46, "", 36, "Futura Bold");
	add(Accuracy);
	Accuracy->setCharacterSpacing(6);

	Accuracy->border = true;
	Accuracy->borderAlpha = 0.5;
	Accuracy->borderSize = 2;

	ScoreText->border = true;
	ScoreText->borderAlpha = 0.5;
	ScoreText->borderSize = 2;

	ranking = new Text(0, 82, "", 36, "Futura Bold");
	ranking->border = true;
	ranking->borderAlpha = 0.5;
	ranking->borderSize = 2;
	add(ranking);
	ranking->setCharacterSpacing(6);

	if (Placement)
	{
		Accuracy->y += 36;
		ranking->y += 36;
		ScoreText->y += 36;
	}


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

	add(Judgement);
	add(Combo);

	songPosBar = new AvgRect(receptors[0]->x, 24, ((receptors[3]->x + (64 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength)), 24);
	//add(songPosBar);
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
	updateAccuracy(0);
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

	if (!song)
		return;

	if (positionInSong >= startTime)
	{
		if (!play)
		{
			song->setPos(0);
			song->setVolume(Game::save->GetDouble("Music Volume"));
			clap->setVolume(Game::save->GetDouble("Hitsounds Volume"));
			song->play();
			play = true;
			lastBPM = 0;
		}
	}

	
	if (play)
	{
		/*float songPos = song->getPos();
		
		float bruh = positionInSong;

		if ((bruh - songPos) <= -0.05
			|| (bruh + songPos) >= 0.05)
		{
			std::cout << "bruh" << " " << (bruh - songPos) << std::endl;
			positionInSong = songPos;
		}
		else
			positionInSong += (Game::deltaTime - Game::save->GetDouble("offset"));*/
		positionInSong = song->getPos() + Game::save->GetDouble("offset");
	}
	else
		positionInSong += Game::deltaTime;
	 
	SDL_FRect bruh;
	bruh.x = 0;
	bruh.y = 0;
	bruh.h = 720;
	bruh.w = 1280;

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


	if (MainerMenu::currentSelectedSong.meta.songName.size() == 0)
		return;

	curSeg = MainerMenu::currentSelectedSong.getSegmentFromTime(positionInSong);
	beat = MainerMenu::currentSelectedSong.getBeatFromTimeOffset(positionInSong, curSeg);
	if (lastBPM != curSeg.bpm)
	{
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


	// debug stuff


	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 128);
	//SDL_RenderFillRectF(Game::renderer, &overlayForAccuracy);
	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);

	// multiplayer shit

	// leaderboard

	if (MultiplayerLobby::inLobby)
	{
		for (leaderboardSpot& spot : leaderboard)
		{
			if (spot.owner->text != "")
				spot.owner->y = spot.t->y + spot.t->surfH;
			else
				spot.owner->y = spot.t->y;
			spot.accuracy->y = spot.owner->y + spot.owner->surfH;
			spot.scoreText->y = spot.accuracy->y + spot.accuracy->surfH;

			if (spot.rankin == 0)
			{
				leaderboardCrown->x = -23;
				leaderboardCrown->y = (spot.t->y - spot.t->surfH) - 10;
			}

			avatars[spot.score.SteamID64]->y = spot.t->y;
		}


		for (leaderboardhighlight& light : highlights)
		{
			if (light.time == -1)
				continue;
			light.time += Game::deltaTime * 0.6;
			if (light.rect)
				light.rect->alpha = lerp(0.8, 0, light.time / 500);
			if (light.time > 500)
				light.time = -1;
			
		}
		int index = 0;
		for (leaderboardhighlight light : highlights)
		{
			if (light.time == -1)
			{
				removeObj(highlights[index].rect);
				highlights.erase(highlights.begin() + index);
				index--;
			}
			index++;
		}
	}

	//SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

	//SDL_RenderFillRectF(Game::renderer, &posBar);

	//SDL_RenderDrawRectF(Game::renderer, &outline);

	// spawning shit

	if (notesToPlay.size() > 0)
	{
		note& n = notesToPlay[0];
		if (n.beat < beat + 16 && (n.type != Note_Tail && n.type != Note_Mine)) // if its in 16 beats
		{
				NoteObject* object = new NoteObject();
				object->currentChart = &MainerMenu::currentSelectedSong;
				object->size = Game::save->GetDouble("Note Size");
				object->connected = &n;
				SDL_FRect rect;
				object->wasHit = false;
				object->clapped = false;
				object->active = true;

				bpmSegment preStopSeg = MainerMenu::currentSelectedSong.getSegmentFromBeat(n.beat);

				float stopOffset = MainerMenu::currentSelectedSong.getStopOffsetFromBeat(n.beat);

				double stopBeatOffset = (stopOffset / 1000) * (preStopSeg.bpm / 60);

				object->stopOffset = stopBeatOffset;

				object->beat = (double) n.beat + stopBeatOffset;
				object->lane = n.lane;
				object->connectedReceptor = receptors[n.lane];
				object->type = n.type;
				object->endTime = -1;
				object->endBeat = -1;

				bpmSegment noteSeg = MainerMenu::currentSelectedSong.getSegmentFromBeat(object->beat);

				object->time = MainerMenu::currentSelectedSong.getTimeFromBeatOffset(object->beat, noteSeg);
				rect.y = Game::gameHeight + 400;
				rect.x = 0;
				rect.w = 64 * Game::save->GetDouble("Note Size");
				rect.h = 64 * Game::save->GetDouble("Note Size");
				object->rect = rect;

				note tail;

				bpmSegment bruh = MainerMenu::currentSelectedSong.getSegmentFromBeat(object->beat);

				float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(beat, bruh);

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

						bpmSegment npreStopSeg = MainerMenu::currentSelectedSong.getSegmentFromBeat(nn.beat);

						float nstopOffset = MainerMenu::currentSelectedSong.getStopOffsetFromBeat(nn.beat);

						double nstopBeatOffset = (nstopOffset / 1000) * (npreStopSeg.bpm / 60);

						object->endBeat = nn.beat + nstopBeatOffset;
						
						object->endTime = MainerMenu::currentSelectedSong.getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
						tail = nn;
						break;
					}
				}

				float time = SDL_GetTicks();

				if (object->type == Note_Head)
				{

					for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
					{
						bpmSegment holdSeg = MainerMenu::currentSelectedSong.getSegmentFromTime(i);

						double beat = MainerMenu::currentSelectedSong.getBeatFromTimeOffset(i, holdSeg);

						float whHold = MainerMenu::currentSelectedSong.getTimeFromBeatOffset(beat, holdSeg);

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
							rect.h = 64 * Game::save->GetDouble("Note Size");
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
				notesToPlay.erase(notesToPlay.begin());
				add(object);
			}
			else if (n.type == Note_Tail || n.type == Note_Mine)
			{
				notesToPlay.erase(notesToPlay.begin());
			}
	}
	else
	{

		if (!ended && notesToPlay.size() == 0 && positionInSong - Game::save->GetDouble("offset") >= song->length)
		{
			ended = true;
			if (!MultiplayerLobby::inLobby)
			{
				MainerMenu::currentSelectedSong.destroy();
				cleanUp();
				Game::instance->transitionToMenu(new MainerMenu());
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
			if (Game::noteskin->shrink)
				receptors[i]->scale = 0.85;
		}
		else
		{
			if (!botplay)
				receptors[i]->lightUpTimer = 0;
			if (receptors[i]->scale < 1.0 && Game::noteskin->shrink)
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
			note->rTime = positionInSong;

			if (!note->destroyed)
			{
				float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(note->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(note->beat));

				if (wh - positionInSong < 2)
				{
					if (botplay && note->active)
					{
						float diff = (wh - positionInSong);

						std::string format = std::to_string(diff - fmod(diff, 0.01));
						format.erase(format.find_last_not_of('0') + 1, std::string::npos);
						receptors[note->lane]->lightUpTimer = 195;
						Judgement->setText("botplay");
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 255;
						Marvelous++;
						score += Judge::scoreNote(diff);
						updateAccuracy(1);
						note->active = false;

						combo++;
						if (Game::noteskin->bounce)
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
							float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(tile.beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(tile.beat));
							float offset = wh;
							if (botplay && offset - positionInSong > 0 && offset - positionInSong < (Judge::hitWindows[1] * 0.5))
							{
								receptors[note->lane]->lightUpTimer = 195;
							}
							if (offset - positionInSong <= Judge::hitWindows[1] * 0.1 && !tile.fucked)
							{
								tile.active = false;
							}
						}
					}
				}

				if (note->lane < 4 && note->lane >= 0)
				{
					Rect receptorRect;
					receptorRect.w = receptors[note->lane]->w;
					receptorRect.h = receptors[note->lane]->h;
					receptorRect.x = receptors[note->lane]->x;
					receptorRect.y = receptors[note->lane]->y;
					note->debug = debug;
					//SDL_SetRenderTarget(Game::renderer, Game::mainCamera->cameraTexture);

				}

				if (wh - positionInSong <= -Judge::hitWindows[4] && note->active && playing)
				{
					note->active = false;
					miss(note);
				}

				bool condition = true;


				if ((wh - positionInSong <= -1000 && !note->active) && note->holdsActive == 0 && playing)
				{
					removeNote(note);
					//std::cout << "remove note " << wh << " " << positionInSong << std::endl;
				}

				for (int i = 0; i < note->heldTilings.size(); i++)
				{
					holdTile& tile = note->heldTilings[i];

					float whHold = MainerMenu::currentSelectedSong.getTimeFromBeat(tile.beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(tile.beat));
					float diff = whHold - positionInSong;

					if (diff < -Judge::hitWindows[4] * 1.5 && tile.active && playing)
					{
						//std::cout << note->lane << " fucked " << diff << " time: " << whHold << " song: " << positionInSong << std::endl;
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

	MUTATE_START
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

	MUTATE_END
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
			MainerMenu::currentSelectedSong.destroy();
			cleanUp();
			Game::instance->transitionToMenu(new MainerMenu());
		
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
		case SDLK_EQUALS:
			if (Game::instance->flowtime && !MultiplayerLobby::inLobby)
			{
				if (playing)
				{
					playing = false;
					song->stop();
				}
				rate += 0.1;
				song->rateChange(rate);
				Game::instance->db_addLine("rate changed to " + std::to_string(rate));
			}
			break;
		case SDLK_MINUS:
			if (Game::instance->flowtime && !MultiplayerLobby::inLobby)
			{
				if (playing)
				{
					playing = false;
					song->stop();
				}
				rate -= 0.1;
				song->rateChange(rate);
				Game::instance->db_addLine("rate changed to " + std::to_string(rate));
			}
			break;
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

				const auto wh = MainerMenu::currentSelectedSong.getTimeFromBeat(object->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(object->beat));

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

			float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(closestObject->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(closestObject->beat));

			float diff = (wh - positionInSong);

			float hw = Judge::hitWindows[4];

			if (closestObject->holdsActive > 0)
				hw = Judge::hitWindows[2];

			if (closestObject->active && diff <= hw && diff > -hw)
			{
				closestObject->active = false;
				closestObject->wasHit = true;

				judgement judge = Judge::judgeNote(diff);
				score += Judge::scoreNote(diff);

				std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				if (Game::save->GetBool("hitsounds") && !closestObject->clapped)
				{
					closestObject->clapped = true;
					if (SoundManager::getChannelByName("clapFx") != NULL)
					{
						clap->stop();
						clap->play();
					}
				}

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
						(*Judgement).color.r = 100;
						(*Judgement).color.g = 220;
						(*Judgement).color.b = 225;
						Judgement->setText("marvelous");
						Marvelous++;
						updateAccuracy(1);

						break;
					case judgement::Judge_perfect:
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Judgement->setText("perfect");
						Perfect++;
						updateAccuracy(0.925);
						break;
					case judgement::Judge_great:
						(*Judgement).color.r = 0;
						(*Judgement).color.g = 255;
						(*Judgement).color.b = 0;
						Judgement->setText("great");
						Great++;
						updateAccuracy(0.7);
						break;
					case judgement::Judge_good:
						(*Judgement).color.r = 255;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Judgement->setText("eh");
						Eh++;
						updateAccuracy(0.35);
						break;
					case judgement::Judge_bad:
						combo = 0;
						(*Judgement).color.r = 128;
						(*Judgement).color.g = 0;
						(*Judgement).color.b = 0;
						Judgement->setText("yikes");
						Yikes++;
						updateAccuracy(0.1);
						break;
					}

					combo++;
					if (Game::noteskin->bounce)
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
