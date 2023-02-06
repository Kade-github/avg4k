#include "Console.h"

#include "Display.h"
#include "Game.h"
#include "implot.h"

using namespace AvgEngine::Debug;

Console* instance;

bool shouldFocus = false;

void keyPress(AvgEngine::Events::Event e)
{
	if (e.data == GLFW_KEY_GRAVE_ACCENT)
	{
		instance->open = !instance->open;
		shouldFocus = instance->open;
	}
}


void Console::registerEvents(Events::EventManager& e)
{
	instance = this;
	e.Subscribe(Events::EventType::Event_KeyPress, keyPress, false);

}

inline constexpr auto hash_djb2a(const std::string_view sv) {
	unsigned long hash{ 5381 };
	for (unsigned char c : sv) {
		hash = ((hash << 5) + hash) ^ c;
	}
	return hash;
}

inline constexpr auto operator"" _sh(const char* str, size_t len) {
	return hash_djb2a(std::string_view{ str, len });
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
			ImPlot::SetupAxesLimits(0, 25, 0, 255, ImPlotCond_Always);
			ImPlot::PlotLine("##FpsStuff", fpsData, 25);
			ImPlot::EndPlot();
		}
		ImGui::Text("Draw Calls: %d", Game::Instance->CurrentMenu->camera.drawCalls.size());
		if (ImPlot::BeginPlot("Draw Calls")) {
			ImPlot::SetupAxes("Report ID", "Draw Calls");
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, 25, ImPlotCond_Always);
			ImPlot::PlotLine("##DrawCalls", drawData, 25);
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

		std::vector<std::string> split = Utils::StringTools::Split(s, " ");

		if (!split.empty())
		{
			std::string cmd = split[0];
			Utils::StringTools::ToLower(cmd);
			switch(hash_djb2a(cmd))
			{
				case "help"_sh:
					Logging::writeLog("[Help] Commands:");
					Logging::writeLog("[Help] game, performgraph");
					break;
				case "game"_sh:
					Logging::writeLog("[Game] " + Game::Instance->Title + ":" + Game::Instance->Version);
					break;
				case "performgraph"_sh:
					showPerformance = !showPerformance;
					break;
				default:
					Logging::writeLog("[Error] That command doesn't exist!");
					break;
			}

		}
		else
			Logging::writeLog("[Error] That command doesn't exist!");
		memset(input, 0, sizeof input);
	}
	ImGui::End();
}
