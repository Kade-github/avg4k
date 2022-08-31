#pragma once
#include "Menu.h"
#include "ReceptorObject.h"
#include "ModManager.h"
#include "Chart.h"
#include "NoteObject.h"
#include "SoundManager.h"
class ModEditor : public Menu
{
public:
	void create() override;
	void update(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;
	void keyDown(SDL_KeyboardEvent event) override;

	bool playing = false;

	Channel* song;

	float currentTime = 0;
	float beat = 0;

	void doModsUntilThisPos();

	void callModEvent(std::string name, std::string args);
	void callModEvent(std::string name, int args);
	void callModEvent(std::string name, float args);

	void move(float amount);

	void mouseWheel(float wheel) override;

	ModManager manager;

	void generateNoteObject(note n, difficulty diff, Chart* selectedChart,  bool findTail = true);

	std::vector<ReceptorObject*> receptors;
	std::vector<NoteObject*> notes;
};

