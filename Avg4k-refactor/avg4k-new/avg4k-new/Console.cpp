#include "Console.h"

#include "Display.h"
#include "Game.h"
#include "implot.h"
#include "CPU.h"

using namespace AvgEngine::Debug;

Console* Console::instance = NULL;

bool shouldFocus = false;

static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }

static void keyPress(AvgEngine::Events::Event e)
{
	if (e.data == GLFW_KEY_GRAVE_ACCENT)
	{
		Console::instance->open = !Console::instance->open;
		shouldFocus = Console::instance->open;
	}
}

ImVector<const char*> History;
int HistoryPos = -1;

// thank you imgui demo file <3
int TextEditCallback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.size() - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.size())
						HistoryPos = -1;
			}
			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
	}
	return 0;
}

void Console::registerEvents(Events::EventManager& e)
{
	instance = this;
	e.Subscribe(Events::EventType::Event_KeyPress, keyPress, false, true);

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
	if (ImGui::InputText("##InputText", input,sizeof(input), (ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory), TextEditCallback))
	{
		std::string s = std::string(input);

		Logging::writeLog("[User] " + s);

		if (!s.empty())
		{
			HistoryPos = -1;
			History.push_back(Strdup(s.c_str()));
			handler->Handle(s);
		}
		else
			Logging::writeLog("[Error] That command doesn't exist!");
		shouldFocus = instance->open;
		memset(input, 0, sizeof input);
	}
	ImGui::End();
}
