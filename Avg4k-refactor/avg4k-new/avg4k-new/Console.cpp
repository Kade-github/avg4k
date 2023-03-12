#include "Console.h"

#include "Display.h"
#include "Game.h"
#include "implot.h"
#include "CPU.h"

using namespace AvgEngine::Debug;

Console* Console::instance = NULL;

bool shouldFocus = false;

static void keyPress(AvgEngine::Events::Event e)
{
	if (e.data == GLFW_KEY_GRAVE_ACCENT)
	{
		Console::instance->open = !Console::instance->open;
		shouldFocus = Console::instance->open;
	}
}


void Console::registerEvents(Events::EventManager& e)
{
	instance = this;
	e.Subscribe(Events::EventType::Event_KeyPress, keyPress, false);

	handler = new ConsoleCommandHandler();
}

void Console::update()
{
	if (!open)
		return;

	if (showPerformance)
	{
		ImGui::SetNextWindowSizeConstraints({ 345,400 }, { static_cast<float>(Render::Display::width),static_cast<float>(Render::Display::height) });
		ImGui::Begin("Performance Graphs");
		ImGui::Text("Current FPS: %d", Game::Instance->fps);
		if (ImPlot::BeginPlot("FPS")) {
			ImPlot::SetupAxes("Report ID", "FPS");
			ImPlot::SetupAxesLimits(0, 50, 0, 255, ImPlotCond_Always);
			ImPlot::PlotLine("##FpsStuff", fpsData, 50);
			ImPlot::EndPlot();
		}
		ImGui::Text("Draw Calls: %d", Game::Instance->CurrentMenu->camera.drawCalls.size());
		if (ImPlot::BeginPlot("Draw Calls")) {
			ImPlot::SetupAxes("Report ID", "Draw Calls");
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, 50, ImPlotCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImPlotCond_Always);
			ImPlot::PlotLine("##DrawCalls", drawData, 50);
			ImPlot::EndPlot();
		}
		ImGui::End();
	}

	ImVec2 size = { static_cast<float>(Render::Display::width), 150.0f };
	ImGui::SetNextWindowSizeConstraints(size, {size.x, static_cast<float>(Render::Display::height)});
	ImGui::SetNextWindowPos({ 0.0f,0.0f });
	ImGui::Begin("AvgEngine Console");

	ImVec2 s = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("Console", { s.x,s.y - 24 });
	for(ConsoleLog& log : Logging::consoleLog)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, log.color.Value);
		ImGui::TextWrapped(log.text.c_str());
		ImGui::PopStyleColor(1);
	}
	if (shouldFocus)
		ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();

	ImGui::PushItemWidth(s.x);
	if (shouldFocus)
	{
		ImGui::SetKeyboardFocusHere(0);
		shouldFocus = false;
	}
	if (ImGui::InputText("##InputText", input,sizeof(input), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		std::string s = std::string(input);

		Logging::writeLog("[User] " + s);

		if (!s.empty())
		{
			handler->Handle(s);
		}
		else
			Logging::writeLog("[Error] That command doesn't exist!");
		shouldFocus = instance->open;
		memset(input, 0, sizeof input);
	}
	ImGui::End();
}
