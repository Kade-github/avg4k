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
	manager.sprites["background"].stealth = 0.4;
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

	std::string bg = currentChart->meta.folder + "/" + currentChart->meta.background;

	background = new AvgSprite(0, 0, bg);

	if (background->tex->fromSTBI)
	{
		background->w = Game::gameWidth;
		background->h = Game::gameHeight;

		background->alpha = 0.6;
	}

	if (!manager.killed)
	{
		spriteField = new AvgSprite(0, 0, manager.spriteCamera->ctb);
		spriteField->w = Game::gameWidth;
		spriteField->h = Game::gameHeight;
		spriteField->flip = true;
		spriteField->dontDelete = true;
		manager.spriteCamera->add(background);
		add(manager.spriteCamera);
		manager.spriteCamera->fuckingNo = true;
		add(spriteField);
	}



	add(gameplay);

	add(playField);

	SpriteMod mod3;
	mod3.anchor = "";
	mod3.confusion = 0;
	mod3.finish = "";
	mod3.movex = 0;
	mod3.movey = 0;
	mod3.offsetX = 0;
	mod3.offsetY = 0;
	mod3.stealth = 0.4;
	mod3.mini = 0.5;
	mod3.spr = background;
	mod3.notModCreated = true;
	mod3.def = background;
	manager.sprites["background"] = mod3;

	SpriteMod mod;
	mod.anchor = "";
	mod.confusion = 0;
	mod.finish = "";
	mod.movex = 0;
	mod.movey = 0;
	mod.offsetX = 0;
	mod.offsetY = 0;
	mod.stealth = 0;
	mod.mini = 0.5;
	mod.spr = playField;
	mod.notModCreated = true;
	mod.def = playField;
	manager.sprites["camera"] = mod;

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
	mod2.spr = spriteField;
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

	// last sprite check

	for (auto m : manager.sprites)
	{
		if (m.second.isPlayField || m.second.spr == NULL)
			continue;

		SpriteMod& value = m.second;

		float anchorX = value.spr->defX;
		float anchorY = value.spr->defY;

		if (value.anchor != "")
		{
			SpriteMod anchor = ModManager::instance->sprites[value.anchor];
			anchorX = anchor.spr->defX + anchor.movex;
			anchorY = anchor.spr->defY + anchor.movey;
		}

		float x = anchorX + value.movex;
		float y = anchorY + value.movey;
		float rot = value.spr->defRot + value.confusion;

		if (value.isPlayField)
		{
			x = anchorX + (value.movex * (1 + (Game::multiplierx != 1 ? Game::multiplierx : 0)));
			y = anchorY + (value.movey * (1 + (Game::multipliery != 1 ? Game::multipliery : 0)));
		}

		value.spr->x = x + value.offsetX;
		value.spr->y = y + value.offsetY;

		value.spr->angle = rot;
		value.spr->alpha = 1 - value.stealth;
		value.spr->scale = 0.5 / value.mini;

		if (value.spr->animationFinished && value.finish != "" && value.spr->sparrow)
			(*manager.instance->lua)[value.finish](value.spr->sparrow->currentAnim);
	}

}

void ModEditor::refresh()
{
	notes.clear();
	for (const auto& s : manager.shaders) 
	{
		delete s.second;
	}
	manager.shaders.clear();
	if (!manager.killed)
	{
		manager.spriteCamera->removeObj(background);
		removeObj(spriteField);
		delete spriteField;
		removeObj(manager.spriteCamera);
		delete manager.spriteCamera;
	}
	for (Playfield* p : playfields)
	{
		delete p;
	}
	playfields.clear();
	Chart* currentChart = FuckinEditor::selectedChart;

	std::string bg = currentChart->meta.folder + "/" + currentChart->meta.background;

	background = new AvgSprite(0, 0, bg);

	if (background->tex->fromSTBI)
	{
		background->w = Game::gameWidth;
		background->h = Game::gameHeight;
		background->alpha = 0.6;
	}

	Playfield* p = new Playfield(
		(640 - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)), 60, gameplay);
	p->mod = true;
	playfields.push_back(p);



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
		manager.spriteCamera->add(background);
		add(manager.spriteCamera);
		manager.spriteCamera->fuckingNo = true;
		add(spriteField);
	}

	playField->drawLast = true;

	SpriteMod mod3;
	mod3.anchor = "";
	mod3.confusion = 0;
	mod3.finish = "";
	mod3.movex = 0;
	mod3.movey = 0;
	mod3.offsetX = 0;
	mod3.offsetY = 0;
	mod3.stealth = 0.4;
	mod3.mini = 0.5;
	mod3.spr = background;
	mod3.notModCreated = true;
	mod3.def = background;
	manager.sprites["background"] = mod3;


	SpriteMod mod;
	mod.anchor = "";
	mod.confusion = 0;
	mod.finish = "";
	mod.movex = 0;
	mod.movey = 0;
	mod.offsetX = 0;
	mod.offsetY = 0;
	mod.stealth = 0;
	mod.mini = 0.5;
	mod.spr = playField;
	playField->customShader = NULL;
	mod.notModCreated = true;
	mod.def = playField;
	manager.sprites["camera"] = mod;

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
	mod2.spr = spriteField;
	mod2.notModCreated = true;
	mod2.def = manager.spriteCamera;
	spriteField->customShader = NULL;
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
	callModEvent("editor_scroll", 0);
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

		p->update(currentTime, beat);
	}

	manager.beat = beat;
	manager.time = currentTime;

	if (playing)
	{
		if (std::abs(currentTime - lastUpdate) > 2)
		{
			lastUpdate = currentTime;
			callModEvent("update", beat);
		}
		manager.runMods();
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
		song->stop();
		song->free();
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
