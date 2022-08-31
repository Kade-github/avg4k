#include "ModEditor.h"
#include "imgui.h"
#include "Game.h"
#include "FuckinEditor.h"
void ModEditor::create()
{
	for (int i = 0; i < 4; i++)
	{
		ReceptorObject* r;

		int index = i + 1;
		r = new ReceptorObject(
			((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * i), (Game::gameHeight / 2) - 300, i);
		r->lightUpTimer = 0;
		r->create();
		receptors.push_back(r);
		add(r);
	}
	created = true;
}

void ModEditor::update(Events::updateEvent event)
{
}

void ModEditor::imguiUpdate(float elapsed)
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

		}

		ImGui::TextColored({128,128,255,1},"Controls: Space to pause/play, scrollwheel to move up or down");
	}
}
