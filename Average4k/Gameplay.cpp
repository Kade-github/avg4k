#include "Gameplay.h"
#include "SongSelect.h"
#include "CPacketHostEndChart.h"
#include <chrono>
#include "TweenManager.h"
#include "MainerMenu.h"
#include "ArrowEffects.h"
#include "picosha2.h"
#include "CPacketSubmitScore.h"

std::mutex weirdPog;



bool hasSubmited = false;

std::map<float, float> noteTimings;

std::map<std::string, AvgSprite*> avatars;

float Gameplay::rate = 1;

Gameplay* Gameplay::instance = nullptr;

float lastTime = 0;

void CALLBACK bruh(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
}

void Gameplay::initControls()
{
	controls.clear();

	std::string keybinds = Game::save->GetString("Keybinds ");

	std::vector<std::string> stuff = Chart::split(keybinds, '-');

	for (int i = 0; i < stuff.size(); i++)
	{
		gameplayControl ctrl;
		ctrl.code = SDL_GetKeyFromName(stuff[i].c_str());
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
	notesPlayed++;
	notesHit += hitWorth;
	accuracy = ((notesHit / notesPlayed) * 100);
	if (accuracy < 0)
		accuracy = 0;

	std::string format = std::to_string((double)((int)(accuracy * 100)) / 100);
	format.erase(format.find_last_not_of('0') + 1, std::string::npos);

	if (accuracy == (int)accuracy)
		format.erase(format.find_last_not_of('.') + 1, std::string::npos);

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

	Mrv->setText("Marvelous: " + std::to_string(Marvelous));
	Prf->setText("Perfect: " + std::to_string(Perfect));
	God->setText("Great: " + std::to_string(Great));
	Ehh->setText("Good: " + std::to_string(Eh));
	Yke->setText("Bad: " + std::to_string(Yikes));
	Mis->setText("Misses: " + std::to_string(Misses));

	MUTATE_END
}


void Gameplay::removeNote(NoteObject* object, Playfield* p)
{
	spawnedNotes.erase(
		std::remove_if(spawnedNotes.begin(), spawnedNotes.end(), [&](NoteObject* const nn) {
			return nn->beat == object->beat && nn->lane == object->lane;
			}),
		spawnedNotes.end());

		NoteObject* o = NULL;
		for (NoteObject* no : p->screenNotes)
			if (no->beat == object->beat && object->lane == no->lane)
				o = no;
		if (o != NULL)
		{
			o->connected->killed = true;
			gameplay->removeObj(o);
			p->screenNotes.erase(
				std::remove_if(p->screenNotes.begin(), p->screenNotes.end(), [&](NoteObject* const nn) {
					return nn->beat == object->beat && nn->lane == object->lane;
					}),
				p->screenNotes.end());
		}
	
}

void Gameplay::miss(NoteObject* object)
{
	if (MainerMenu::isInLobby)
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

	noteTimings[positionInSong] = Judge::hitWindows[4];

	Misses++;
	updateAccuracy(0);
	combo = 0;
	judge->changeOutTexture(Game::noteskin->judge_5);
	judge->w *= (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0));
	judge->h *= (1 + (Game::multipliery != 1 ? Game::multipliery : 0));
	judge->drawCall = true;

	judge->setX((Game::gameWidth / 2) - (judge->w / 2));
	judge->setY((Game::gameHeight / 2) - (judge->h / 2));


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

	if (!MainerMenu::isInLobby || !created)
		return;

	SPacketUpdateLeaderboard pack;
	SPacketFinalizeChart ch;
	SPacketStatus f;
	msgpack::unpacked result;

	msgpack::object obj;
	std::map<std::string, AvgSprite*>::iterator it;
	leaderboardSpot cspot;
	std::vector<std::string> ids = {};

	std::vector<leaderboardSpot> copy = leaderboard;

	int rankin = 0;
	std::vector<std::string> spots;
	std::vector<leaderboardSpot> toRemove;
	switch (pt)
	{
	case eSPacketStatus:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(f);
		switch (f.code)
		{
		case 803:
			MainerMenu::isHost = true;
			break;
		}
	case eSPacketUpdateLeaderboard:

		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(pack);


		for (PlayerScore score : pack.orderedScores)
		{
			spots.push_back(score.SteamID64);
			int realRank = rankin + 1;
			std::string placementt = std::to_string(realRank) + std::string(ordinal_suffix(realRank)) + " Place";

			if (score.SteamID64 == std::to_string(SteamUser()->GetSteamID().ConvertToUint64()) && Placement->text != placementt)
			{
				Placement->setText(placementt);
				Placement->x = (Game::gameWidth - Placement->surfW) - 24;
				Steam::SetPresence(("Playing " + MainerMenu::currentSelectedSong.meta.songName + " | " + placementt + " in multiplayer.").c_str());
				Game::DiscordUpdatePresence(MainerMenu::currentSelectedSong.meta.songName + " in " + MainerMenu::currentLobby.LobbyName, "Playing Multiplayer (" + Placement->text + ")", "Average4K", MainerMenu::currentLobby.Players, MainerMenu::currentLobby.MaxPlayers, "");
			}
			bool found = false;
			int index = 0;

			leaderboardSpot* spot = nullptr;

			for (leaderboardSpot& sp : leaderboard)
			{
				if (sp.score.SteamID64 == score.SteamID64)
				{
					spot = &sp;
					found = true;
				}
			}

			if (spot != nullptr)
			{
				if (rankin < 10)
				{
					std::string username = "";
					if (score.Username.size() > 14)
						username = score.Username.substr(0, 14) + "...";
					else
						username = score.Username;
					int lastRank = spot->rankin;
					spot->score = score; // copy it over
					int y = ((leaderboardText->y + leaderboardText->h) + 24) + ((82 * rankin) + 4);

					if (spot->rankin > rankin && !spot->isHighlighted)
					{
						leaderboardhighlight high;
						high.rect = new AvgRect(0, y, 227, 82);
						high.time = 0;
						high.spot = rankin;
						high.spotObj = spot;
						high.rect->alpha = 0.8;
						high.rect->c = { 255,255,255 };
						add(high.rect);
						highlights.push_back(high);
						spot->isHighlighted = true;
					}

					if (rankin != spot->rankin)
					{
						std::string test = "scoreboard" + score.SteamID64;
						if (Tweening::TweenManager::doesTweenExist(test))
						{
							Tweening::TweenManager::removeTween(test);
							spot->t->y = ((leaderboardText->y + leaderboardText->h) + 24) + ((82 * (lastRank)) + 4);
						}
						Tweening::TweenManager::createNewTween(test, spot->t, Tweening::tt_Y, 250, spot->t->y, y, NULL, Easing::EaseOutCubic);
					}
					spot->rankin = rankin;

					std::string format = "0";
					if (score.Accuracy != 0)
					{
						format = std::to_string((double)((int)(score.Accuracy * 10000)) / 100);
						format.erase(format.find_last_not_of('0') + 1, std::string::npos);

						if (score.Accuracy == (int)score.Accuracy)
							format.erase(format.find_last_not_of('.') + 1, std::string::npos);
					}

					spot->accuracy->setText(format + "% accuracy");
					spot->t->setText(username);
				}
				else if (found && rankin >= 10)
				{
					if (spot->rankin < 10)
					{
						int y = ((leaderboardText->y + leaderboardText->h) + 24) + ((82 * rankin) + 4);
						Tweening::TweenManager::createNewTween("scoreboard" + score.SteamID64, spot->t, Tweening::tt_Y, 750, spot->t->y, y, NULL, Easing::EaseInCubic);
					}
				}
			}

			if (!found)
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
				if (MainerMenu::currentLobby.Host.SteamID64 == score.SteamID64)
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

				cspot.accuracy->setCharacterSpacing(3);
				cspot.owner->setCharacterSpacing(3);
				cspot.t->setCharacterSpacing(3);

				leaderboard.push_back(cspot);
				add(cspot.accuracy);
				add(cspot.owner);
				add(cspot.t);

				if (rankin == 0)
				{
					leaderboardCrown->x = -23;
					leaderboardCrown->y = (cspot.t->y - cspot.t->surfH) - 10;
				}

			}
			rankin++;
		}

		if (spots.size() > 0)
		{
			for (leaderboardSpot& spott : leaderboard)
			{
				bool found = false;
				for (std::string s : spots)
				{
					if (s == spott.score.SteamID64)
						found = true;
				}
				if (!found)
					toRemove.push_back(spott);
				avatars[spott.score.SteamID64]->x = 3;
				//add(avatars[spot->score.SteamID64]);
			}

			for (leaderboardSpot& spot : toRemove)
			{
				removeObj(spot.accuracy);
				removeObj(spot.owner);
				removeObj(spot.t);
				removeObj(avatars[spot.score.SteamID64]);
				avatars.erase(spot.score.SteamID64);
				leaderboard.erase(std::remove_if(leaderboard.begin(), leaderboard.end(),
					[&spot](leaderboardSpot& i) { return i.score.SteamID64 == spot.score.SteamID64; }));
			}
		}
		toRemove.clear();
		break;
	case eSPacketFinalizeChart:

		leaderboard.clear();

		MainerMenu::currentSelectedSong.destroy();
		cleanUp();

		Game::instance->transitionToMenu(new MainerMenu());
		MainerMenu::isInLobby = true;
		break;
	}
	MUTATE_END
}

Gameplay::Gameplay()
{
}

Shader* shad = nullptr;

void Gameplay::create() {

	MUTATE_START



		if (Noteskin::type != Game::save->GetString("Noteskin"))
		{
			Noteskin::resetNoteskin(Game::noteskin);
			Game::noteskin = Noteskin::getNoteskin();
		}


	lastTime = 0;

	instance = this;

	ModManager::time = 0;
	ModManager::beat = 0;

	noteTimings.clear();

	initControls();

	avatars.clear();

	gameplay = new AvgGroup(0, 0, 1280, 720);

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
		Channel* c = SoundManager::getChannelByName("prevSong");
		c->stop();
	}

	downscroll = Game::save->GetBool("downscroll");
	if (MainerMenu::currentSelectedSong.isModFile)
		downscroll = false;


	Steam::SetPresence(("Playing " + MainerMenu::currentSelectedSong.meta.songName).c_str());

	Judge::initJudge();

	Judge::judgeNote(174);

	if (MainerMenu::currentSelectedSong.meta.difficulties.size() == 0)
	{
		Game::instance->transitionToMenu(new MainerMenu());
		return;
	}

	std::string bg = MainerMenu::currentSelectedSong.meta.folder + "/" + MainerMenu::currentSelectedSong.meta.background;

	background = new AvgSprite(0, 0, bg);
	background->w = Game::gameWidth;
	background->h = Game::gameHeight;

	background->alpha = Game::save->GetDouble("Background Transparency");

	add(background);

	Playfield* p = NULL;
	if (downscroll)
		p = new Playfield(
			((1280 / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)), (720 / 2) + 250, gameplay);
	else
		p = new Playfield(
			((1280 / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)), (720 / 2) - 300, gameplay);


	playFields.push_back(p);

	if (MainerMenu::currentSelectedSong.isModFile)
	{
		ModManager::doMods = true;
		runModStuff = true;
		ModManager::initLuaFunctions();
		p->mod = true;
		manager = ModManager(MainerMenu::currentSelectedSong.pathToLua);
		manager.modGame = gameplay;
		manager.gamePlayfields = &playFields;
		manager.modPlayfields[manager.currentPId] = playFields[0];
		manager.currentPId++;
		manager.cam = cam;
		manager.instance = &manager;
		if (!manager.killed)
		{
			spriteField = new AvgSprite(0, 0, manager.spriteCamera->ctb);
			spriteField->w = Game::gameWidth;
			spriteField->h = Game::gameHeight;
			spriteField->flip = true;
			add(manager.spriteCamera);
			manager.spriteCamera->fuckingNo = true;
			add(spriteField);
		}
	}
	else
	{
		ModManager::doMods = false;
	}



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
	if (ModManager::doMods)
		rOverlay->alpha = Game::save->GetDouble("Lane Underway Transparency");
	add(rOverlay);


	float bassRate = (rate * 100) - 100;

	std::string path = MainerMenu::currentSelectedSong.meta.folder + "/" + MainerMenu::currentSelectedSong.meta.audio;

	if (SoundManager::getChannelByName("prevSong") == NULL)
	{
		song = SoundManager::createChannel(path.c_str(), "prevSong");
	}
	else
		song = SoundManager::getChannelByName("prevSong");

	song->loop = false;

	hasSubmited = false;

	song->createFXStream();

	songLength = song->length;

	int diff = MainerMenu::selectedDiffIndex;

	positionAndBeats = new Text(0, 40, "", 16, "NotoSans-Regular");
	positionAndBeats->create();
	add(positionAndBeats);


	notesToPlay = MainerMenu::currentSelectedSong.meta.difficulties[diff].notes;

	Rect laneUnderway;

	laneUnderway.x = ((1280 / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (((1280 / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * 3) - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size"));
	laneUnderway.h = 1280;

	AvgSprite* lunder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underway.png"));
	if (!MainerMenu::currentSelectedSong.isModFile)
		gameplay->add(lunder);
	lunder->alpha = Game::save->GetDouble("Lane Underway Transparency");

	lunder->colorR = darkestColor.r;
	lunder->colorG = darkestColor.g;
	lunder->colorB = darkestColor.b;

	AvgSprite* lunderBorder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underwayBorder.png"));
	if (!MainerMenu::currentSelectedSong.isModFile)
		gameplay->add(lunderBorder);
	lunderBorder->alpha = 1 * (0.8 / Game::save->GetDouble("Lane Underway Transparency"));

	lunderBorder->colorR = lightestAccent.r;
	lunderBorder->colorG = lightestAccent.g;
	lunderBorder->colorB = lightestAccent.b;

	lunder->w = laneUnderway.w;
	lunder->h = laneUnderway.h;
	lunderBorder->w = laneUnderway.w;
	lunderBorder->h = laneUnderway.h;

	Judgement = new Text(Game::gameWidth / 2, Game::gameHeight / 2, " ", 23 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Judgement->setCharacterSpacing(4.25);
	judge = new AvgSprite(Judgement->x, Judgement->y, NULL);
	judge->dontDelete = true;
	judge->drawCall = false;

	Judgement->borderColor = { 255,255,255 };
	Judgement->border = true;
	Judgement->borderSize = 1;

	Combo = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) + (46 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0))), " ", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Combo->create();
	Combo->SetFontMap(Game::instance->noteskin->fontMap);
	Combo->fontMapSize = (0.9 + (Game::multiplierx != 1 ? Game::multiplierx : 0));

	Mrv = new Text(12, (Game::gameHeight / 2) - 12 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Marvelous: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Mrv->create();

	Prf = new Text(12, (Game::gameHeight / 2) + 12 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Perfect: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Prf->create();

	God = new Text(12, (Game::gameHeight / 2) + 36 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Great: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	God->create();

	Ehh = new Text(12, (Game::gameHeight / 2) + 60 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Good: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Ehh->create();

	Yke = new Text(12, (Game::gameHeight / 2) + 82 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Bad: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Yke->create();

	Mis = new Text(12, (Game::gameHeight / 2) + 104 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Misses: 0", 24 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	Mis->create();

	Combo->borderSize = 1;
	Combo->border = true;

	AvgSprite* rightGrad = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "rightGraid.png"));
	rightGrad->x = Game::gameWidth - rightGrad->w;

	rightGrad->colorR = darkestColor.r;
	rightGrad->colorG = darkestColor.g;
	rightGrad->colorB = darkestColor.b;
	rightGrad->alpha = Game::save->GetDouble("Lane Underway Transparency");
	if (MainerMenu::currentSelectedSong.isModFile)
		rightGrad->alpha = 0;
	if (!ModManager::doMods)
		add(rightGrad);

	if (MainerMenu::isInLobby)
	{
		Game::DiscordUpdatePresence(MainerMenu::currentSelectedSong.meta.songName + " in " + MainerMenu::currentLobby.LobbyName, "Playing Multiplayer", "Average4K", MainerMenu::currentLobby.Players, MainerMenu::currentLobby.MaxPlayers, "");
		AvgSprite* leftGrad = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "leftGraid.png"));
		leftGrad->colorR = darkestColor.r;
		leftGrad->colorG = darkestColor.g;
		leftGrad->colorB = darkestColor.b;
		leftGrad->alpha = Game::save->GetDouble("Lane Underway Transparency");
		if (!ModManager::doMods)
			add(leftGrad);

		AvgSprite* leftGradBorder = new AvgSprite(0, 0, Noteskin::getGameplayElement(Game::noteskin, "leftBorder.png"));
		leftGradBorder->colorR = lightestAccent.r;
		leftGradBorder->colorG = lightestAccent.g;
		leftGradBorder->colorB = lightestAccent.b;
		leftGradBorder->alpha = (0.8 / Game::save->GetDouble("Lane Underway Transparency"));
		if (!ModManager::doMods)
			add(leftGradBorder);

		Mrv->x = leftGradBorder->x + leftGradBorder->w + 12;
		Prf->x = Mrv->x;
		God->x = Mrv->x;
		Ehh->x = Mrv->x;
		Yke->x = Mrv->x;
		Mis->x = Mrv->x;

		for (int i = 0; i < MainerMenu::currentLobby.PlayerList.size(); i++)
		{
			player pp = MainerMenu::currentLobby.PlayerList[i];
			const char* pog = pp.Avatar.c_str();
			Texture* s = Steam::getAvatar(pog);
			avatars[pp.SteamID64] = new AvgSprite(0, 0, s);
			avatars[pp.SteamID64]->w = 52;
			avatars[pp.SteamID64]->h = 52;
			add(avatars[pp.SteamID64]);
		}

		if (avatars.size() == 0)
		{
			Game::instance->transitionToMenu(new MainerMenu());
			return;
		}
		else
		{
			for (int i = 0; i < MainerMenu::currentLobby.PlayerList.size(); i++)
			{
				player pp = MainerMenu::currentLobby.PlayerList[i];
				if (avatars[pp.SteamID64] == nullptr)
				{
					Game::instance->transitionToMenu(new MainerMenu());
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
		Game::DiscordUpdatePresence((MainerMenu::currentSelectedSong.meta.artist.size() == 0 ? "No Artist" : MainerMenu::currentSelectedSong.meta.artist) + " - " + MainerMenu::currentSelectedSong.meta.songName, "Playing Solo Play", "Average4K", -1, -1, "");

	p->addReceptors();

	playField = new AvgSprite(0, 0, gameplay->ctb);
	playField->w = Game::gameWidth;
	playField->h = Game::gameHeight;
	playField->flip = true;
	gameplay->fuckingNo = true;
	add(gameplay);

	add(playField);

	if (ModManager::doMods)
	{
		SpriteMod mod2;
		mod2.anchor = "";
		mod2.confusion = 0;
		mod2.finish = "";
		mod2.movex = 0;
		mod2.movey = 0;
		mod2.offsetX = 0;
		mod2.offsetY = 0;
		mod2.stealth = 0;
		mod2.mini = 0.5;
		mod2.spr = NULL;
		mod2.notModCreated = true;
		mod2.def = manager.spriteCamera;
		manager.sprites["sprites"] = mod2;


	}


	callModEvent("create", 0);

	playField->dontDelete = true;

	Accuracy = new Text(Game::gameWidth - 200, 2, "", 36 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	add(Accuracy);
	Accuracy->setCharacterSpacing(6);

	Accuracy->border = true;
	Accuracy->borderAlpha = 0.5;
	Accuracy->borderSize = 2;

	ranking = new Text(0, 46 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "", 36 * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)), "Futura Bold");
	ranking->border = true;
	ranking->borderAlpha = 0.5;
	ranking->borderSize = 2;
	add(ranking);
	ranking->setCharacterSpacing(6);

	if (Placement)
	{
		Accuracy->y += 36;
		ranking->y += 36;
	}

	add(judge);
	add(Combo);
	if (Game::save->GetBool("Show Judgement Count"))
	{
		add(Mrv);
		add(Prf);
		add(God);
		add(Ehh);
		add(Yke);
		add(Mis);
	}

	created = true;

	if (MainerMenu::isInLobby)
		positionInSong = -5000;
	else
		positionInSong = -(Game::save->GetDouble("Start Delay") * 1000);
	lastSeg.startTime = -1;
	lastSeg.length = 0;

	xmod = Game::save->GetBool("Use XMOD Scroll");

	MUTATE_END
}

void Gameplay::callModEvent(std::string name, std::string args)
{
	if (MainerMenu::currentSelectedSong.isModFile)
	{
		manager.callEvent(name, args);
	}
}

void Gameplay::callModEvent(std::string name, int args)
{
	if (MainerMenu::currentSelectedSong.isModFile)
	{
		manager.callEvent(name, args);
	}
}

void Gameplay::callModEvent(std::string name, float args)
{
	if (MainerMenu::currentSelectedSong.isModFile)
	{
		manager.callEvent(name, args);
	}
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float lastBPM = 0;

void Gameplay::update(Events::updateEvent event)
{
	MUTATE_START

	botplayHittingNote = false;

	if (!song || Game::instance->transitioning)
		return;

	if (positionInSong >= (startTime + (MainerMenu::currentSelectedSong.BASS_OFFSET * 1000)) + Game::save->GetDouble("offset"))
	{
		if (!play)
		{
			song->setPos(0);
			song->setVolume(Game::save->GetDouble("Music Volume"));
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
		if (!paused)
		{
			positionInSong = (song->getPos() + (MainerMenu::currentSelectedSong.BASS_OFFSET * 1000)) + Game::save->GetDouble("offset");
			lastTime += Game::deltaTime;
		}
	}
	else
		positionInSong += Game::deltaTime;


	if (judge->scale > 1)
	{
		Combo->scale = lerp(1.15, 1, scaleStart / scaleTime);
		judge->scale = lerp(1.15, 1, scaleStart / scaleTime);
		scaleStart += Game::deltaTime;
	}

	if (MainerMenu::currentSelectedSong.meta.songName.size() == 0)
		return;

	if (lastSeg.startTime + lastSeg.length < positionInSong)
	{
		curSeg = MainerMenu::currentSelectedSong.getSegmentFromTime(positionInSong);
		lastSeg = curSeg;
	}
	beat = MainerMenu::currentSelectedSong.getBeatFromTime(positionInSong, curSeg);

	if (lastBPM != curSeg.bpm)
	{
		song->bpm = curSeg.bpm;
		lastBPM = curSeg.bpm;
	}

	Rendering::iBeat = beat;
	Rendering::iBpm = lastBPM;

	ModManager::time = positionInSong;
	ModManager::beat = beat;

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

	//songPosBar->w = ((receptors[3]->x + (64 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength));


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

	if (MainerMenu::isInLobby)
	{
		for (leaderboardSpot& spot : leaderboard)
		{
			if (spot.owner->text != "")
				spot.owner->y = spot.t->y + spot.t->surfH;
			else
				spot.owner->y = spot.t->y;
			spot.accuracy->y = spot.owner->y + spot.owner->surfH;

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
				light.spotObj->isHighlighted = false;
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

	bool onGodEnd = true;

	if (ModManager::doMods)
	{
		if (manager.dontStop)
			onGodEnd = false;
	}

	if (!ended && ((notesToPlay.size() == 0 && spawnedNotes.size() == 0) || ((lastTime - positionInSong) > 4000 || !song->isPlaying)) && positionInSong > 0 && onGodEnd)
	{
		ended = true;
		if (!MainerMenu::isInLobby)
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

		}

		if (Game::save->GetBool("Submit Scores") && !hasSubmited && !botplayOnce)
		{
			hasSubmited = true;

			CPacketSubmitScore submit;

			submit.ChartId = (MainerMenu::selected.isSteam ? MainerMenu::selected.steamId : MainerMenu::selectedSong.steamId);
			submit.chartIndex = MainerMenu::packSongIndex;
			submit.noteTiming = noteTimings;
			submit.Order = 0;
			submit.PacketType = eCPacketSubmitScore;
			submit.hash = MainerMenu::selectedSong.c.meta.hash;

			if ((MainerMenu::selected.isSteam || MainerMenu::selectedSong.isSteam))
				Multiplayer::sendMessage<CPacketSubmitScore>(submit);

			Game::instance->save->saveScore(noteTimings, accuracy, highestCombo, MainerMenu::currentSelectedSong.meta.songName, MainerMenu::currentSelectedSong.meta.artist, submit.ChartId, submit.chartIndex, MainerMenu::selectedDiffIndex);

		}
	}

	int pIndex = 0;
	bool spawned = false;
	for (Playfield* p : playFields)
	{
		if (notesToPlay.size() > 0)
		{
			note& n = notesToPlay[0];
			if (n.beat < beat + p->arrowEff.drawBeats && n.type != Note_Tail) // if its in draw beats
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
				object->curSeg = preStopSeg;

				object->beat = (double)n.beat;
				if (!xmod)
					object->beat += stopBeatOffset;
				object->xmod = xmod;
				object->tempStopOffset = object->stopOffset;
				object->lane = n.lane;
				object->connectedReceptor = p->screenReceptors[n.lane];
				object->type = n.type;
				object->endTime = -1;
				object->endBeat = -1;


				object->time = MainerMenu::currentSelectedSong.getTimeFromBeatOffset(object->beat, preStopSeg);
				rect.y = Game::gameHeight + 400;
				rect.x = 0;
				rect.w = 64 * Game::save->GetDouble("Note Size");
				rect.h = 64 * Game::save->GetDouble("Note Size");
				object->rect = rect;

				note tail;


				float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(beat, preStopSeg);

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

						object->endTime = MainerMenu::currentSelectedSong.getTimeFromBeatOffset(nn.beat + nstopBeatOffset, preStopSeg);
						tail = nn;
						break;
					}
				}

				if (!spawned)
					spawnedNotes.push_back(object);
				spawned = true;
				object->create();
				p->addNote(object);
			}
			else if (n.type == Note_Tail)
			{
				notesToPlay.erase(notesToPlay.begin());
			}
		}
		for (int i = 0; i < p->screenReceptors.size(); i++)
		{
			if (keys[i] || holding[i])
			{
				p->screenReceptors[i]->light();
				p->screenReceptors[i]->loop = false;
				if (Game::noteskin->shrink)
					p->screenReceptors[i]->scale = 0.85;
			}
			else
			{
				if (!botplay)
					p->screenReceptors[i]->lightUpTimer = 0;
				if (p->screenReceptors[i]->scale < 1.0 && Game::noteskin->shrink)
				{
					p->screenReceptors[i]->scale += Game::deltaTime * 0.04;
					if (p->screenReceptors[i]->scale > 1.0)
						p->screenReceptors[i]->scale = 1;
				}
			}

			if (p->screenReceptors[i]->lightUpTimer < 1)
			{
				p->screenReceptors[i]->hit = false;
			}
		}

		std::map<int, int> realIndex;
		realIndex[0] = 0;
		realIndex[1] = 0;
		realIndex[2] = 0;
		realIndex[3] = 0;

		{
			for (int i = 0; i < spawnedNotes.size(); i++)
			{
				NoteObject* note = spawnedNotes[i];
				note->rTime = positionInSong;
				if ((keys[note->lane] || holding[note->lane]) && note->type == Note_Mine && (realIndex[note->lane] == 0))
				{
					if (!note->blownUp)
					{
						const auto wh = MainerMenu::currentSelectedSong.getTimeFromBeat(note->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(note->beat));

						float diff = std::abs(wh - positionInSong);

						judgement j = Judge::judgeNote(diff);
						if (j == judgement::Judge_marvelous || j == judgement::Judge_perfect)
						{
							noteTimings[positionInSong] = Judge::hitWindows[4];

							updateAccuracy(0);
							note->blownUp = true;
							Channel* c = SoundManager::createChannel(Noteskin::getSoundElement(Game::instance->noteskin, "explosion_sound.ogg"), "explosion" + std::to_string(note->id));
							c->dieAfterPlay = true;
							c->setVolume(Game::save->GetDouble("Hitsounds Volume"));
							c->play();
							
						}
					}
				}
				if (!note->destroyed)
				{
					float wh = MainerMenu::currentSelectedSong.getTimeFromBeat(note->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(note->beat));
	
						if (wh - positionInSong < 2)
						{
							if (botplay && note->active && note->type != Note_Mine && note->type != Note_Fake)
							{
								float diff = (wh - positionInSong);

								botplayHittingNote = true;


								std::string format = std::to_string(diff - fmod(diff, 0.01));
								format.erase(format.find_last_not_of('0') + 1, std::string::npos);
								for (Playfield* pp : playFields)
								{
									pp->screenReceptors[note->lane]->lightUpTimer = 195;
									pp->screenReceptors[note->lane]->bot = botplay;

									pp->screenReceptors[note->lane]->loop = false;
									pp->screenReceptors[note->lane]->hit = true;
								}

								for (Playfield* pp : playFields)
								{
									for (NoteObject* o : pp->screenNotes)
										if (o->beat == note->beat && o->lane == note->lane)
											o->active = false;
								}
								note->active = false;

								if (pIndex == 0)
								{
									if (ModManager::doMods)
										manager.callEvent("hit", note->lane);
									judge->changeOutTexture(Game::noteskin->judge_bot);
									judge->w *= (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0));
									judge->h *= (1 + (Game::multipliery != 1 ? Game::multipliery : 0));
									Marvelous++;
									judge->drawCall = true;
									//score += Judge::scoreNote(diff);
									updateAccuracy(1);
									if (Game::save->GetBool("hitsounds") && !note->clapped)
									{
										note->clapped = true;
										Channel* c = SoundManager::createChannel(Noteskin::getSoundElement(Game::instance->noteskin, "hitSound.wav"), "clap" + std::to_string(note->beat));
										c->dieAfterPlay = true;
										c->setVolume(Game::save->GetDouble("Hitsounds Volume"));
										c->play();
									}

									combo++;

									if (combo > highestCombo)
										highestCombo = combo;
									if (Game::noteskin->bounce)
									{
										judge->scale = 1.15;
										Combo->scale = 1.15;
										scaleTime = 350;
										scaleStart = 0;
									}

									judge->setX((Game::gameWidth / 2) - (judge->w / 2));
									judge->setY((Game::gameHeight / 2) - (judge->h / 2));


									Combo->setText(std::to_string(combo));
									Combo->setX((Game::gameWidth / 2) - (Combo->surfW / 2));
									Combo->setY((Game::gameHeight / 2) + 40);
								}
							}

						}
	

					if (note->type == Note_Head)
					{
						float startTime = MainerMenu::currentSelectedSong.getTimeFromBeat(note->beat, MainerMenu::currentSelectedSong.getSegmentFromBeat(note->beat));
						float endTime = MainerMenu::currentSelectedSong.getTimeFromBeat(note->endBeat, MainerMenu::currentSelectedSong.getSegmentFromBeat(note->endBeat));

						if (startTime < positionInSong + Judge::hitWindows[2] && positionInSong < endTime + Judge::hitWindows[2])
						{
							note->holdPerc = beat / note->endBeat;
							if (holding[note->lane] || (botplay && startTime < positionInSong && positionInSong < endTime)) // holding that lane!
							{
								if (botplay)
								{
									p->screenReceptors[note->lane]->lightUpTimer = 195;
								}
								if (ModManager::doMods && lastCall + 150 < positionInSong)
								{
									lastCall = positionInSong + 150;
									if (pIndex == 0)
									{
										manager.callEvent("hit", note->lane);
									}
								}
								botplayHittingNote = false;
								p->screenReceptors[note->lane]->loop = true;
								p->screenReceptors[note->lane]->hit = true;
								note->holding = true;
							}
							else if (positionInSong >= startTime + Judge::hitWindows[4] && !holding[note->lane])
							{
								if (note->holding)
								{
									note->holdstoppedbeat = beat;
									note->holdstoppedtime = positionInSong;
								}
								if (!botplay)
									p->screenReceptors[note->lane]->hit = false;
								note->holding = false;
								note->fuckTimer = positionInSong / (note->holdstoppedtime + 250);
								if (pIndex == 0)
								{
									if (note->fuckTimer >= 1 && note->holdstoppedtime + 250 < endTime)
									{
										note->active = false;
										note->missHold = true;
										miss(note);
										removeNote(note, p);
									}

									if (note->holdstoppedtime + 250 > endTime)
									{
										note->active = false;
										removeNote(note, p);
									}
								}
							}
						}

						if (note->holdPerc >= 1 && positionInSong > endTime * 1.05)
						{
							removeNote(note, p);
						}
					}

					if (wh - positionInSong <= -Judge::hitWindows[4] && note->active && (note->type != Note_Fake && note->type != Note_Mine) && playing)
					{
						note->active = false;
						miss(note);
					}

					if ((wh - positionInSong <= -1000 && !note->active) && note->endBeat == -1 && playing)
					{
						removeNote(note, p);
						//std::cout << "remove note " << wh << " " << positionInSong << std::endl;
					}
				}
				if (note->active && note->time > positionInSong)
					realIndex[note->lane]++;
			}
		}

		p->update(positionInSong, beat);
		if (runModStuff)
			manager.runMods();
		pIndex++;
	}

	if (spawned && notesToPlay.size() > 0)
		notesToPlay.erase(notesToPlay.begin());

	if (!ended) 
	{
		if (ModManager::doMods && lastCallUpdates + 2 < positionInSong)
		{
			lastCallUpdates = positionInSong + 2;
			manager.callEvent("update", (float)beat);
		}
	}

	MUTATE_END
}
void Gameplay::postUpdate(Events::updateEvent ev)
{
	if (Game::instance->save->GetBool("Show Song Position"))
	{
		songPosBar.x = 0;
		songPosBar.y = (downscroll ? Game::gameHeight - 10 : 0);
		songPosBar.h = 10;
		songPosBar.w = (Game::gameWidth * (positionInSong / song->length));
		songPosBar.a = 0.5;

		Rect srcPos = { 0,0,1,1 };

		Rendering::drawBatch();
		Rendering::PushQuad(&songPosBar, &srcPos, NULL, NULL);
		Rendering::drawBatch();
	}
}
void Gameplay::cleanUp()
{

	MUTATE_START

		if (ModManager::doMods)
		{
			manager.kill();
			removeObj(manager.spriteCamera);
		}

	for (Playfield* p : playFields)
		delete p;

	playFields.clear();
	spawnedNotes.clear();
	notesToPlay.clear();

	//for (std::map<std::string, SDL_Texture*>::iterator iter = avatars.begin(); iter != avatars.end(); ++iter)
	//{
	//	std::string k = iter->first;
	//	SDL_DestroyTexture(avatars[k]);
	//}


	if (MainerMenu::currentSelectedSong.isModFile && !MainerMenu::isInLobby)
	{
		manager.destroy();
	}

	//if (background)
	//	SDL_DestroyTexture(background);

	MUTATE_END
}

void Gameplay::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START
	float colHeight = 0;

	if (ModManager::doMods)
	{
		manager.callEvent("key_pressed", event.keysym.sym);
	}

	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			
			if (MainerMenu::isInLobby && MainerMenu::isHost) {
				CPacketHostEndChart end;
				end.Order = 0;
				end.PacketType = eCPacketHostEndChart;

				Multiplayer::sendMessage<CPacketHostEndChart>(end);
				return;
			}
			else if (MainerMenu::isInLobby)
			{
				CPacketLeave leave;
				leave.Order = 0;
				leave.PacketType = eCPacketLeave;

				Multiplayer::sendMessage<CPacketLeave>(leave);
				MainerMenu::currentSelectedSong.destroy();
				cleanUp();
				Game::instance->transitionToMenu(new MainerMenu());
			}
			else
			{
				MainerMenu::currentSelectedSong.destroy();
				cleanUp();
				Game::instance->transitionToMenu(new MainerMenu());
			}
		
			return;
		case SDLK_F1:
			if (MainerMenu::isInLobby)
				return;
			botplay = !botplay;
			botplayOnce = true;
			return;
		case SDLK_F2:
			if (MainerMenu::isInLobby)
				return;
			debug = !debug;
			break;
		case SDLK_BACKQUOTE:
			if (MainerMenu::isInLobby)
				return;
			cleanUp();
			Game::instance->transitionToMenu(new Gameplay());
			return;
		case SDLK_SPACE:
			if (debug)
			{
				paused = !paused;

				if (paused)
					song->stop();
				else
				{
					song->play();
					song->setPos(positionInSong - (MainerMenu::currentSelectedSong.BASS_OFFSET * 1000));
				}
			}
			break;
		case SDLK_EQUALS:
			if (Game::instance->flowtime && !MainerMenu::isInLobby)
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
			if (Game::instance->flowtime && !MainerMenu::isInLobby)
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

				if (diff <= Judge::hitWindows[4] && diff < currentDiff && object->lane == control.lane && object->type != Note_Mine && object->type != Note_Fake)
				{
					closestObject = object;
					currentDiff = diff;
				}
			}

			if (!keys[control.lane])
			{
				keys[control.lane] = true;
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
				for (Playfield* p : playFields)
				{
					p->screenReceptors[closestObject->lane]->loop = false;
					p->screenReceptors[closestObject->lane]->hit = true;
					removeNote(closestObject, p);
				}

				if (ModManager::doMods)
					manager.callEvent("hit", closestObject->lane);
				closestObject->active = false;
				closestObject->wasHit = true;


				noteTimings[positionInSong] = diff;

				judgement judgem = Judge::judgeNote(diff);
				//score += Judge::scoreNote(diff);

				std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				if (Game::save->GetBool("hitsounds") && !closestObject->clapped)
				{
					closestObject->clapped = true;
					Channel* c = SoundManager::createChannel("assets/sounds/hitSound.wav", "clap" + std::to_string(closestObject->beat));
					c->dieAfterPlay = true;
					c->setVolume(Game::save->GetDouble("Hitsounds Volume"));
					c->play();
				}

				if (MainerMenu::isInLobby)
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

				switch (judgem)
				{
					case judgement::Judge_marvelous:
						judge->changeOutTexture(Game::noteskin->judge_0);
						Marvelous++;
						updateAccuracy(1);
						break;
					case judgement::Judge_perfect:
						judge->changeOutTexture(Game::noteskin->judge_1);
						Perfect++;
						updateAccuracy(0.925);
						break;
					case judgement::Judge_great:
						judge->changeOutTexture(Game::noteskin->judge_2);
						Great++;
						updateAccuracy(0.7);
						break;
					case judgement::Judge_good:
						judge->changeOutTexture(Game::noteskin->judge_3);
						Eh++;
						updateAccuracy(0.35);
						break;
					case judgement::Judge_bad:
						judge->changeOutTexture(Game::noteskin->judge_4);
						Yikes++;
						updateAccuracy(0.1);
						break;
					}

					judge->drawCall = true;
					judge->w *= (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0));
					judge->h *= (1 + (Game::multipliery != 1 ? Game::multipliery : 0));

					combo++;
					if (Game::noteskin->bounce)
					{
						judge->scale = 1.15;
						Combo->scale = 1.15;
						scaleTime = 350;
						scaleStart = 0;
					}

					judge->setX((Game::gameWidth / 2) - (judge->w / 2));
					judge->setY((Game::gameHeight / 2) - (judge->h / 2));

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
	if (ModManager::doMods)
	{
		manager.callEvent("key_released", ev.keysym.sym);
	}
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
