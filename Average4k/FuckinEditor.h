#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h";
#include "Chart.h"
#include "imgui.h"
#include "NoteObject.h"
typedef void(__cdecl* drawCall)();
struct editorWindow {
	std::string title;
	ImVec2 size;
	drawCall draw;
	bool shouldDraw;
	float xOff = 0;
};

class FuckinEditor : public Menu
{
public:
	void create() override;

	bool songPlaying = false;

	void update(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;
	std::vector<AvgSprite*> fuck;
	std::vector<editorWindow> windows;
	std::vector<NoteObject*> notes;

	void keyDown(SDL_KeyboardEvent event) override;

	void loadNotes(difficulty diff);

	void createWindow(std::string title, ImVec2 size, drawCall draw, bool shouldDraw)
	{
		editorWindow wind;
		wind.title = title;
		wind.shouldDraw = shouldDraw;
		wind.size = size;
		wind.draw = draw;
		windows.push_back(wind);
	}

	editorWindow& findWindow(std::string title)
	{
		editorWindow found;
		for (editorWindow& wind : windows)
			if (wind.title == title)
				return wind;
		return found;
	}
};