#include "ModEditor.h"
#include "imgui.h"
#include "Game.h"
#include "FuckinEditor.h"

float lastPos = 0;

int ssnap;
int ssnapSelect;

std::map<float, float> ssnapConvert;

void ModEditor::resetSprites(bool resetAnchor)
{
	for (auto& [key, value] : manager.sprites) {
		value.confusion = 0;
		value.stealth = 0;
		value.finish = "";
		value.movex = 0;
		value.movey = 0;
		if (resetAnchor)
			value.anchor = "";
		value.offsetX = 0;
		value.offsetY = 0;
		value.isPlayField = false;
		value.mini = 0.5;
		value.confusion = 0;
		if (value.spr != NULL)
		{
			value.spr->x = value.ogX;
			value.spr->y = value.ogY;
		}
	}
}


void ModEditor::create()
{
	Steam::SetPresence(("In the mod editor, previewing " + FuckinEditor::selectedChart->meta.songName + ".").c_str());
	ssnapSelect = 3;
	ssnap = 16;
	ssnapConvert = { {4,1}, {8,2},{12,3},{16,4}, {24,6}, {32,8}, {64,16} };

	gameplay = new AvgGroup(0, 0, 1280, 720);
	//modGameplay = new AvgGroup(0, 0, 1280, 720);

	Chart* currentChart = FuckinEditor::selectedChart;

	difficulty diff = currentChart->meta.difficulties[FuckinEditor::currentDiff];

	std::string pathj = currentChart->meta.folder + "/" + currentChart->meta.audio;

	song = SoundManager::createChannel(pathj.c_str(), "modSong");

	song->createFXStream();

	lastPos = 0;
	currentTime = 0;
	song->setPos(0);

	if (Noteskin::type != Game::save->GetString("Noteskin"))
	{
		Noteskin::resetNoteskin(Game::noteskin);
		Game::noteskin = Noteskin::getNoteskin();
	}

	Playfield* p = new Playfield(
			(640 - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)), 60, gameplay);
	p->mod = true;
	playfields.push_back(p);

	ModManager::doMods = true;
	ModManager::initLuaFunctions();
	manager = ModManager(currentChart->pathToLua);
	manager.modGame = gameplay;
	manager.gamePlayfields = &playfields;
	manager.modPlayfields[manager.currentPId] = playfields[0];
	manager.currentPId++;
	manager.cam = cam;
	manager.instance = &manager;
	manager.isInEditor = true;
	created = true;


	p->addReceptors();

	for (note n : diff.notes)
	{
		generateNoteObject(n, diff, currentChart);
	}

	playField = new AvgSprite(0, 0, gameplay->ctb);
	playField->w = Game::gameWidth;
	playField->h = Game::gameHeight;
	playField->flip = true;
	playField->dontDelete = true;
	gameplay->fuckingNo = true;
;

	if (!manager.killed)
	{
		spriteField = new AvgSprite(0, 0, manager.spriteCamera->ctb);
		spriteField->w = Game::gameWidth;
		spriteField->h = Game::gameHeight;
		spriteField->flip = true;
		spriteField->dontDelete = true;
		add(manager.spriteCamera);
		manager.spriteCamera->fuckingNo = true;
		add(spriteField);
	}

	add(gameplay);

	add(playField);

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

	callModEvent("create", 0);

}

void ModEditor::doModsUntilThisPos()
{
	resetSprites();
	for (Playfield* p : playfields)
		p->arrowEff.resetEffects();

	std::sort(manager.appliedMods.begin(), manager.appliedMods.end(), AppliedMod());
	for (AppliedMod& mod : manager.appliedMods)
	{
		mod.instant = false;
		mod.started = false;
		mod.done = false;

		if (mod.tweenStart > beat)
			continue;
		float endMod = mod.tweenStart + mod.tweenLen;

		if (endMod < beat)
		{
			manager.runMods(mod, endMod);
		}
		else if (endMod > beat && mod.tweenStart < beat)
		{
			manager.runMods(mod, beat);
		}
	}

	for (FunctionMod& m : manager.funcMod)
	{
		if (m.beat == 0)
			m.beat = beat;
		m.hit = false;
		if (beat >= m.beat && !m.hit)
		{
			m.hit = true;
			m.toCall();
		}
	}

}

void ModEditor::refresh()
{
	notes.clear();
	if (!manager.killed)
	{
		removeObj(spriteField);
		delete spriteField;
		removeObj(manager.spriteCamera);
		delete manager.spriteCamera;
	}
	for (Playfield* p : playfields)
		delete p;
	playfields.clear();

	Playfield* p = new Playfield(
		(640 - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)), 60, gameplay);
	p->mod = true;
	playfields.push_back(p);


	Chart* currentChart = FuckinEditor::selectedChart;

	difficulty diff = currentChart->meta.difficulties[FuckinEditor::currentDiff];

	p->addReceptors();

	for (note n : diff.notes)
	{
		generateNoteObject(n, diff, currentChart);
	}

	manager.kill();

	manager.sprites.clear();
	ModManager::doMods = true;
	ModManager::initLuaFunctions();
	manager = ModManager(currentChart->pathToLua);
	manager.modGame = gameplay;
	manager.gamePlayfields = &playfields;
	manager.currentPId = 0;
	manager.modPlayfields.clear();
	manager.modPlayfields[manager.currentPId] = playfields[0];
	manager.currentPId = 1;
	manager.cam = cam;
	manager.instance = &manager;
	manager.isInEditor = true;
	Game::instance->db_addLine("[Mod Editor] Refreshed modfile!");

	if (!manager.killed)
	{
		spriteField = new AvgSprite(0, 0, manager.spriteCamera->ctb);
		spriteField->dontDelete = true;
		spriteField->w = Game::gameWidth;
		spriteField->h = Game::gameHeight;
		spriteField->flip = true;
		add(manager.spriteCamera);
		manager.spriteCamera->fuckingNo = true;
		add(spriteField);
	}

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

	callModEvent("create", 0);
	doModsUntilThisPos();

}

float beaterMath(float amount, float increase, float startBeat)
{
	float beats = 0.0f;
	if (amount < 0)
	{
		float flor = roundf(startBeat * static_cast<float>(ssnapConvert[ssnap]));
		beats = (flor / static_cast<float>(ssnapConvert[ssnap])) + increase;
	}
	else
	{
		float ceal = roundf(startBeat * static_cast<float>(ssnapConvert[ssnap]));
		beats = (ceal / static_cast<float>(ssnapConvert[ssnap])) - increase;
	}
	return beats;
}


void ModEditor::callModEvent(std::string name, std::string args)
{
	manager.callEvent(name, args);
}

void ModEditor::callModEvent(std::string name, int args)
{
	manager.callEvent(name, args);
}

void ModEditor::callModEvent(std::string name, float args)
{
	manager.callEvent(name, args);
}

void ModEditor::move(float amount)
{
	if (playing)
	{
		lastPos = song->getPos();
		playing = false;
		song->stop();
	}
	float nextTime = currentTime + amount;

	currentTime = nextTime;
	if (currentTime < FuckinEditor::selectedChart->meta.chartOffset)
		currentTime = FuckinEditor::selectedChart->meta.chartOffset;
	bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);

	beat = FuckinEditor::selectedChart->getBeatFromTime(currentTime, curSeg);

	lastPos = currentTime;
	song->setPos(currentTime);

	doModsUntilThisPos();
}


void ModEditor::generateNoteObject(note n, difficulty diff, Chart* selectedChart, bool findTail)
{
	if (n.type == noteType::Note_Tail || n.type == noteType::Note_Mine)
		return;
	bool spawned = false;
	for (Playfield* p : playfields)
	{

		float noteZoom = Game::instance->save->GetDouble("Note Size");
		bool downscroll = false;
		NoteObject* object = new NoteObject();
		object->size = noteZoom;
		object->currentChart = selectedChart;
		object->connected = &n;
		SDL_FRect rect;
		object->wasHit = false;
		object->clapped = false;
		object->active = true;

		bpmSegment preStopSeg = selectedChart->getSegmentFromBeat(n.beat);

		float stopOffset = selectedChart->getStopOffsetFromBeat(n.beat);

		double stopBeatOffset = (stopOffset / 1000) * (preStopSeg.bpm / 60);

		object->stopOffset = stopBeatOffset;
		object->curSeg = preStopSeg;

		object->beat = (double)n.beat + stopBeatOffset;
		object->lane = n.lane;
		object->connectedReceptor = p->screenReceptors[n.lane];
		object->type = n.type;
		object->endTime = -1;
		object->endBeat = -1;

		bpmSegment noteSeg = selectedChart->getSegmentFromBeat(object->beat);

		object->time = selectedChart->getTimeFromBeatOffset(object->beat, noteSeg);
		rect.y = Game::gameHeight + 400;
		rect.x = 0;
		rect.w = 64 * noteZoom;
		rect.h = 64 * noteZoom;
		object->rect = rect;

		note tail;

		bpmSegment bruh = selectedChart->getSegmentFromBeat(object->beat);

		float wh = selectedChart->getTimeFromBeatOffset(n.beat, bruh);

		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		if (findTail)
		{
			if (object->type == Note_Head)
			{
				for (int i = 0; i < diff.notes.size(); i++)
				{
					note& nn = diff.notes[i];
					if (nn.type != Note_Tail)
						continue;
					if (nn.lane != object->lane || nn.connectedBeat != n.beat)
						continue;

					bpmSegment npreStopSeg = selectedChart->getSegmentFromBeat(nn.beat);

					float nstopOffset = selectedChart->getStopOffsetFromBeat(nn.beat);

					double nstopBeatOffset = (nstopOffset / 1000) * (npreStopSeg.bpm / 60);

					object->endBeat = nn.beat + nstopBeatOffset;
					object->tailBeat = nn.beat;

					object->endTime = selectedChart->getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
					tail = nn;
					break;
				}
			}
		}
		else
		{
			note nn;
			nn.beat = n.connectedBeat;
			nn.connectedBeat = -1;
			nn.type = noteType::Note_Tail;
			nn.lane = n.lane;
			object->tailBeat = nn.beat;
			tail = nn;
			object->endTime = selectedChart->getTimeFromBeatOffset(n.connectedBeat, noteSeg);
		}

		float time = SDL_GetTicks();

		// generate minimap line

		line l;
		l.beat = object->beat;
		l.time = object->time;

		l.rect = new AvgRect((6 * object->lane) + 9, 0, 2, 2);
		l.lane = object->lane;

		float beatRow = (object->beat - stopBeatOffset) * 48;

		if (fmod(beatRow, (192 / 4)) == 0)
			l.rect->c = { 255,0,0 };
		else if (fmod(beatRow, (192 / 8)) == 0)
			l.rect->c = { 0,0,255 };
		else if (fmod(beatRow, (192 / 12)) == 0)
			l.rect->c = { 0,255,0 };
		else if (fmod(beatRow, (192 / 16)) == 0)
			l.rect->c = { 255,255,0 };
		else if (fmod(beatRow, (192 / 24)) == 0)
			l.rect->c = { 0,255,0 };
		else if (fmod(beatRow, (192 / 32)) == 0)
			l.rect->c = { 255,215,0 };
		else
			l.rect->c = { 105,105,105 };

		object->create();
		if (!spawned)
			notes.push_back(object);
		spawned = true;
		p->addNote(object);
	}
}

void ModEditor::update(Events::updateEvent event)
{
	if (playing)
	{
		currentTime = song->getPos() + (FuckinEditor::selectedChart->BASS_OFFSET * 1000);

		bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);

 		beat = FuckinEditor::selectedChart->getBeatFromTime(currentTime, curSeg);
	}

	for (Playfield* p : playfields)
	{
		for (NoteObject* obj : p->screenNotes)
		{
			if (currentTime >= obj->time)
			{
				if (!obj->wasHit || (obj->endTime > currentTime))
				{
					obj->wasHit = true;
					manager.callEvent("hit", obj->lane);
				}
			}
		}
		p->update(currentTime, beat);
	}

	manager.beat = beat;
	manager.time = currentTime;

	if (playing)
	{
		manager.runMods();
		callModEvent("update", beat);
	}

}

void ModEditor::postUpdate(Events::updateEvent event)
{

}

void ModEditor::imguiUpdate(float elapsed)
{
	if (!showDebug)
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::MenuItem("Go Back"))
			{
				FuckinEditor::dontDeleteChart = true;
				Game::instance->transitionToMenu(new FuckinEditor());
				return;
			}
			if (ImGui::MenuItem("Refresh Modfile"))
			{
				lastPos = currentTime;
				playing = false;
				song->stop();
				refresh();
			}

			ImGui::TextColored({ 0.5,0.5,1,1 }, ("Controls: Space to pause/play, scrollwheel to move up or down | Current Beat: " + std::to_string(beat)).c_str());
			ImGui::EndMainMenuBar();
		}
	}

}

void ModEditor::keyDown(SDL_KeyboardEvent ev)
{
	if (ev.keysym.sym == SDLK_ESCAPE)
	{
		resetSprites(true);
		FuckinEditor::dontDeleteChart = true;
		Game::instance->transitionToMenu(new FuckinEditor());
		return;
	}

	if (ev.keysym.sym == SDLK_SPACE)
	{
		if (playing)
		{
			lastPos = song->getPos();
			song->stop();
			playing = false;
		}
		else
		{
			playing = true;
			song->play();
			song->setPos(lastPos);
		}
	}

	if (ev.keysym.sym == SDLK_LEFT)
	{
		ssnapSelect--;
		if (ssnapSelect < 0)
			ssnapSelect = 6;
	}

	if (ev.keysym.sym == SDLK_RIGHT)
	{
		ssnapSelect++;
		if (ssnapSelect > 6)
			ssnapSelect = 0;
	}

	if (ev.keysym.sym == SDLK_F2)
		showDebug = !showDebug;

	if (ev.keysym.sym == SDLK_F3)
	{
		lastPos = 0;
		song->setPos(0);
		currentTime = 0;
		playing = false;
		song->stop();
		refresh();
	}

	auto it = ssnapConvert.begin();
	std::advance(it, ssnapSelect);
	ssnap = it->first;

	if (ev.keysym.sym == SDLK_F5)
	{
		lastPos = currentTime;
		playing = false;
		song->stop();
		refresh();
	}
}

void ModEditor::mouseWheel(float wheel)
{
	float amount = wheel > 0 ? 1 : -1;

	move(-(amount * 100));

	for (NoteObject* obj : notes)
		obj->wasHit = false;

}

void ModEditor::leftMouseDown()
{
	if (!showDebug)
		return;

	mouseDown = true;

}

void ModEditor::leftMouseUp()
{
	col = -1;
	mouseDown = false;
}
