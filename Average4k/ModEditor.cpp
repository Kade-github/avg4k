#include "ModEditor.h"
#include "imgui.h"
#include "Game.h"
#include "FuckinEditor.h"
#include "ArrowEffects.h"

float lastPos = 0;

int ssnap;
int ssnapSelect;

std::map<float, float> ssnapConvert;

void ModEditor::resetSprites()
{
	for (auto& [key, value] : manager->sprites) {
		value.confusion = 0;
		value.stealth = 0;
		value.movex = 0;
		value.movey = 0;
		value.confusion = 0;
	}
}


void ModEditor::create()
{
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

	ArrowEffects::resetEffects();

	ModManager::doMods = true;
	ModManager::initLuaFunctions();
	manager = new ModManager(currentChart->pathToLua);
	manager->instance = manager;
	created = true;


	for (int i = 0; i < 4; i++)
	{
		ReceptorObject* r;

		int index = i + 1;
		r = new ReceptorObject(
			((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * i), (Game::gameHeight / 2) - 300, i);
		r->lightUpTimer = 0;
		r->create();
		receptors.push_back(r);
		gameplay->add(r);
	}


	for (note n : diff.notes)
	{
		generateNoteObject(n, diff, currentChart);
	}

	playField = new AvgSprite(0, 0, gameplay->ctb);
	gameplay->fuckingNo = true;
	playField->flip = true;
	playField->dontDelete = true;

	add(manager->spriteCamera);

	gameplay->drawLast = true;
	playField->drawLast = true;
	add(gameplay);

	add(playField);

	manager->cam = cam;

	SpriteMod mod;
	mod.anchor = "";
	mod.confusion = 0;
	mod.finish = "";
	mod.movex = 0;
	mod.movey = 0;
	mod.offsetX = 0;
	mod.offsetY = 0;
	mod.stealth = 0;
	mod.spr = playField;
	mod.notModCreated = true;

	manager->sprites["playField"] = mod;

	callModEvent("create", 0);

}

void ModEditor::doModsUntilThisPos()
{
	ArrowEffects::resetEffects();
	resetSprites();
	for (AppliedMod& mod : manager->appliedMods)
	{
		mod.started = false;
		float endMod = mod.tweenStart + mod.tweenLen;

		if (mod.tweenStart + mod.tweenLen < beat)
		{
			manager->runMods(mod, endMod);
		}
		else if (mod.tweenStart < beat && endMod > beat)
		{
			manager->runMods(mod, beat);
		}
	}

	for (auto& [key, value] : manager->sprites) {
		value.offsetX = 0;
		value.offsetY = 0;


		if (value.finish != "" && value.spr->sparrow)
		{
			(*manager->lua)[value.finish](value.spr->sparrow->currentAnim);
		}
	}
}

void ModEditor::refresh()
{
	for (NoteObject* obj : notes)
	{
		obj->wasHit = false;
	}
	removeObj(manager->spriteCamera);
	for (const auto& [key, value] : manager->sprites) {
		if (value.isPlayField)
		{
			removeObj(value.spr);
			delete value.spr;
		}
	}

	manager->kill();
	delete manager->spriteCamera;
	delete manager;
	manager->sprites.clear();
	ModManager::doMods = true;
	ModManager::initLuaFunctions();
	manager = new ModManager(FuckinEditor::selectedChart->pathToLua);
	manager->instance = manager;
	Game::instance->db_addLine("[Mod Editor] Refreshed modfile!");
	add(manager->spriteCamera);

	SpriteMod mod;
	mod.anchor = "";
	mod.confusion = 0;
	mod.finish = "";
	mod.movex = 0;
	mod.movey = 0;
	mod.offsetX = 0;
	mod.offsetY = 0;
	mod.stealth = 0;
	mod.spr = playField;
	mod.notModCreated = true;

	manager->sprites["playField"] = mod;

	manager->cam = cam;

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
	manager->callEvent(name, args);
}

void ModEditor::callModEvent(std::string name, int args)
{
	manager->callEvent(name, args);
}

void ModEditor::callModEvent(std::string name, float args)
{
	manager->callEvent(name, args);
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

	object->beat = (double)n.beat + stopBeatOffset;
	object->lane = n.lane;
	object->connectedReceptor = receptors[n.lane];
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
	notes.push_back(object);
	gameplay->add(object);
}

void ModEditor::update(Events::updateEvent event)
{
	if (playing)
	{
		currentTime = song->getPos() + (FuckinEditor::selectedChart->BASS_OFFSET * 1000);

		bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);

 		beat = FuckinEditor::selectedChart->getBeatFromTime(currentTime, curSeg);
	}

	for (NoteObject* obj : notes)
	{
		if (obj->beat > beat + ArrowEffects::drawBeats || obj->beat < beat - ArrowEffects::drawBeats)
		{
			obj->drawCall = false;
			continue;
		}
		else
			obj->drawCall = true;
		obj->rTime = currentTime;

		if (currentTime >= obj->time)
		{
			if (!obj->wasHit || (obj->endTime > currentTime))
			{
				obj->wasHit = true;
				manager->callEvent("hit", obj->lane);
			}
		}
	}

	for (int i = 0; i < receptors.size(); i++)
	{
		ReceptorObject* rec = receptors[i];
		rec->positionInSong = currentTime;
		rec->modX = rec->x;
		rec->modY = rec->y;
		rec->endX = rec->modX;
		rec->endY = rec->modY;

		if (ArrowEffects::ShowSplines)
			ArrowEffects::drawLine(rec->x, rec->y, i, beat, *FuckinEditor::selectedChart);
	}

	for (NoteObject* obj : notes)
	{
		obj->modX = obj->x;
		obj->modY = obj->y;
		obj->endX = obj->modX;
		obj->endY = obj->modY;
		obj->modCMOD = obj->cmod;
		obj->endCMOD = obj->modCMOD;

		for (holdTile& tile : obj->heldTilings)
		{
			tile.modX = tile.ogX;
			tile.modY = tile.ogY;
			tile.endX = tile.modX;
			tile.endY = tile.modY;
		}
	}


	manager->beat = beat;
	manager->time = currentTime;

	if (playing)
	{
		manager->runMods();
		callModEvent("update", beat);
	}

}

void ModEditor::postUpdate(Events::updateEvent event)
{

	if (mouseDown)
	{
		int mx, my;
		Game::GetMousePos(&mx, &my);

		float nSize = Game::instance->save->GetDouble("Note Size");

		if (col == -1)
		{
			for (int i = 0; i < receptors.size(); i++)
			{
				ReceptorObject* rec = receptors[i];

				if (mx > rec->modX - (16 * nSize)
					&& my > rec->modY - (16 * nSize)
					&& mx < rec->modX + rec->w + (16 * nSize)
					&& my < rec->modY + rec->h + (16 * nSize))
				{
					col = i;
				}
			}
		}
		else
		{
			ReceptorObject* rec = receptors[col];

			float newX = (mx - (32 * nSize));
			float newY = (my - (32 * nSize));

			ArrowEffects::movex[col] = (newX - rec->modX);
			ArrowEffects::movey[col] = (newY - rec->modY);
		}
		

	}
}

void ModEditor::imguiUpdate(float elapsed)
{
	if (!showDebug)
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::MenuItem("Go Back"))
			{
				ArrowEffects::resetEffects();
				FuckinEditor::dontDeleteChart = true;
				Game::instance->transitionToMenu(new FuckinEditor());
				return;
			}
			if (ImGui::MenuItem("Refresh Modfile"))
			{
				lastPos = currentTime;
				playing = false;
				ArrowEffects::resetEffects();
				song->stop();
				refresh();
			}

			ImGui::TextColored({ 0.5,0.5,1,1 }, ("Controls: Space to pause/play, scrollwheel to move up or down | Current Beat: " + std::to_string(beat)).c_str());
			ImGui::EndMainMenuBar();
		}
	}
	if (showDebug)
	{

			ImGui::PushItemWidth(400);
			ImGui::BeginMainMenuBar();
			{
				if (ImGui::BeginMenu("Arrow Path"))
				{
					ImGui::Text("Path Alpha");
					ImGui::SliderFloat("##SplineAlpha", &ArrowEffects::SplineAlpha, 0.1, 1);
					if (ImGui::Button("Reset"))
					{
						ArrowEffects::SplineAlpha = 0.75;
					}
					ImGui::Text("Path Density");
					ImGui::SliderFloat("##SplineDensity", &ArrowEffects::SplineDensity, 0.008, 1);
					if (ImGui::Button("Reset ##den"))
					{
						ArrowEffects::SplineDensity = 0.05;
					}

					ImGui::Checkbox("Show Paths", &ArrowEffects::ShowSplines);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Drunk"))
				{
					ImGui::Text("Drunk");
					ImGui::SliderFloat("##Drunk", &ArrowEffects::drunk, -20, 20);
					if (ImGui::Button("Reset"))
					{
						ArrowEffects::drunk = 0;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Tipsy"))
				{
					ImGui::Text("Tipsy");
					ImGui::SliderFloat("##Tipsy", &ArrowEffects::tipsy, -20, 20);
					if (ImGui::Button("Reset"))
					{
						ArrowEffects::tipsy = 0;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("AMove"))
				{
					ImGui::Text("Amovex");
					ImGui::InputFloat("##Amovex", &ArrowEffects::amovex, 1, 10);
					ImGui::Text("Amovey");
					ImGui::InputFloat("##Amovey", &ArrowEffects::amovey, 1, 10);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Move"))
				{
					for (int i = 0; i < 4; i++)
					{
						ImGui::Text("Col %d's movex", i);
						ImGui::InputFloat (("##Movex" + std::to_string(i)).c_str(), &ArrowEffects::movex[i], 1, 10);
						ImGui::Text("Col %d's movey", i);
						ImGui::InputFloat(("##Movey" + std::to_string(i)).c_str(), &ArrowEffects::movey[i], 1, 10);
						ImGui::Separator();
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Reverse"))
				{
					for (int i = 0; i < 4; i++)
					{
						ImGui::Text("Col %d reverse", i);
						ImGui::SliderFloat(("##Reverse" + std::to_string(i)).c_str(), &ArrowEffects::reverse[i], -1, 1);
						if (ImGui::Button(("Reset ##" + std::to_string(i)).c_str()))
						{
							ArrowEffects::reverse[i] = 0;
						}
						ImGui::Separator();
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Stealth White"))
				{
					for (int i = 0; i < 4; i++)
					{
						ImGui::Text("Col %d stealthWhite", i);
						ImGui::SliderFloat(("##stealthWhite" + std::to_string(i)).c_str(), &ArrowEffects::stealthWhite[i], 0, 1);
						if (ImGui::Button(("Reset ##" + std::to_string(i)).c_str()))
						{
							ArrowEffects::stealthWhite[i] = 0;
						}
						ImGui::Separator();
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Stealth Opacity"))
				{
					for (int i = 0; i < 4; i++)
					{
						ImGui::Text("Col %d stealthOpacity", i);
						ImGui::SliderFloat(("##stealthOpacity" + std::to_string(i)).c_str(), &ArrowEffects::stealthOpacity[i], 0, 1);
						if (ImGui::Button(("Reset ##" + std::to_string(i)).c_str()))
						{
							ArrowEffects::stealthOpacity[i] = 1;
						}
						ImGui::Text("Receptor %d stealthReceptorOpacity", i);
						ImGui::SliderFloat(("##stealthReceptorOpacity" + std::to_string(i)).c_str(), &ArrowEffects::stealthReceptorOpacity[i], 0, 1);
						if (ImGui::Button(("Reset ##" + std::to_string(i)).c_str()))
						{
							ArrowEffects::stealthReceptorOpacity[i] = 1;
						}
						ImGui::Separator();
					}
					ImGui::EndMenu();
				}

			}
			ImGui::EndMainMenuBar();
	}

}

void ModEditor::keyDown(SDL_KeyboardEvent ev)
{
	if (ev.keysym.sym == SDLK_ESCAPE)
	{
		ArrowEffects::resetEffects();
		resetSprites();
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
		ArrowEffects::resetEffects();
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
		ArrowEffects::resetEffects();
		song->stop();
		refresh();
	}
}

void ModEditor::mouseWheel(float wheel)
{
	float amount = wheel > 0 ? 1 : -1;

	move(-(amount * 100));

	for (NoteObject* obj : notes)
	{
		obj->wasHit = false;
	}

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
