#pragma once
#include "Menu.h"
#include "ReceptorObject.h"
#include "ModManager.h"
#include "NoteObject.h"
#include "Playfield.h"
#include "SoundManager.h"
class ModEditor : public Menu
{
public:
	void create() override;
	void update(Events::updateEvent event) override;
	void postUpdate(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;
	void keyDown(SDL_KeyboardEvent event) override;

	std::vector<Playfield*> playfields;

	int col = -1;

	bool playing = false;

	Channel* song;

	float lastUpdate = 0;


	float currentTime = 0;

	void resetSprites(bool resetAnchor = false);

	float beat = 0;

	bool showDebug = false;

	AvgSprite* playField;
	AvgSprite* spriteField;

	float startTime = 0;

	void doModsUntilThisPos();

	void refresh();

	AvgGroup* gameplay;

	AvgGroup* modGameplay;

	void callModEvent(std::string name, std::string args);
	void callModEvent(std::string name, int args);
	void callModEvent(std::string name, float args);

	void move(float amount);

	void mouseWheel(float wheel) override;

	void leftMouseDown() override;
	void leftMouseUp() override;
	bool mouseDown = false;

	ModManager manager;

	void generateNoteObject(note n, difficulty diff, Chart* selectedChart,  bool findTail = true);

	std::vector<ReceptorObject*> receptors;
	std::vector<NoteObject*> notes;
};

