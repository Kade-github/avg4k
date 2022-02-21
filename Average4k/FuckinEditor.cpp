#include "FuckinEditor.h"
#include "ImGuiFileDialog.h"
#include "MainMenu.h"
#include "Judge.h"
// window draws

Chart* selectedChart;
float currentTime;
float currentBeat;
std::vector<NoteObject*> notes;

int snapSelect = 0;
float startBeats[4] = {};
note saved[4] = {};
note tails[4] = {};

bool topLayer = false;
int currentDiff = 0;
int snap = 16;

std::map<int, int> snapConvert;

void changeTheme(int theme) {
	Game::save->SetDouble("chartTheme", (double)theme);
	Game::save->Save();
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;
	switch (theme)
	{
	case 0:
		// default
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		style->WindowPadding = ImVec2(8.00f, 8.00f);
		style->FramePadding = ImVec2(5.00f, 2.00f);
		style->CellPadding = ImVec2(6.00f, 6.00f);
		style->ItemSpacing = ImVec2(6.00f, 6.00f);
		style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style->IndentSpacing = 25;
		style->ScrollbarSize = 15;
		style->GrabMinSize = 10;
		style->WindowBorderSize = 1;
		style->ChildBorderSize = 1;
		style->PopupBorderSize = 1;
		style->FrameBorderSize = 1;
		style->TabBorderSize = 1;
		style->WindowRounding = 7;
		style->ChildRounding = 4;
		style->FrameRounding = 3;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 9;
		style->GrabRounding = 3;
		style->LogSliderDeadzone = 4;
		style->TabRounding = 4;
		break;
	case 1:
		// light

		style->WindowRounding = 2.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
		style->ScrollbarRounding = 3.0f;             // Radius of grab corners rounding for scrollbar
		style->GrabRounding = 2.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
		style->AntiAliasedLines = true;
		style->AntiAliasedFill = true;
		style->WindowRounding = 2;
		style->ChildRounding = 2;
		style->ScrollbarSize = 16;
		style->ScrollbarRounding = 3;
		style->GrabRounding = 2;
		style->ItemSpacing.x = 10;
		style->ItemSpacing.y = 4;
		style->IndentSpacing = 22;
		style->FramePadding.x = 6;
		style->FramePadding.y = 4;
		style->Alpha = 1.0f;
		style->FrameRounding = 3.0f;

		colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		//colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
		colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
		break;
	case 2:
		// orange tyingy

		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		style->FramePadding = ImVec2(4, 2);
		style->ItemSpacing = ImVec2(10, 2);
		style->IndentSpacing = 12;
		style->ScrollbarSize = 10;

		style->WindowRounding = 4;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 6;
		style->GrabRounding = 4;
		style->TabRounding = 4;

		style->WindowTitleAlign = ImVec2(1.0f, 0.5f);
		style->WindowMenuButtonPosition = ImGuiDir_Right;

		style->DisplaySafeAreaPadding = ImVec2(4, 4);
		break;
	case 3:
		// red bois
		style->FrameRounding = 4.0f;
		style->WindowBorderSize = 0.0f;
		style->PopupBorderSize = 0.0f;
		style->GrabRounding = 4.0f;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.73f, 0.75f, 0.74f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.47f, 0.22f, 0.22f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.34f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.71f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.66f, 0.66f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.47f, 0.22f, 0.22f, 0.65f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.65f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
		colors[ImGuiCol_Header] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.65f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
		colors[ImGuiCol_Tab] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
		colors[ImGuiCol_TabActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		break;
	}
}
std::string notifTitle = "";
std::string notifText = "";

void triggerNotif(std::string title, std::string text)
{
	FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;

	notifTitle = title;
	notifText = text;



	editor->findWindow("Notification").shouldDraw = true;
}

note findNote(int lane, float beat)
{
	note struc;
	struc.beat = -1;
	for (note& n : selectedChart->meta.difficulties[currentDiff].notes)
		if (n.beat == beat && n.lane == lane)
			return n;
	return struc;
}

void createNote(int lane, float beat = currentBeat, noteType type = noteType::Note_Normal, float connectedBeat = -1, bool dontSave = false) {
	note n;
	n.beat = beat;
	n.lane = lane;
	n.type = type;
	n.connectedBeat = connectedBeat;
	selectedChart->meta.difficulties[currentDiff].notes.push_back(n);
	FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;
	editor->generateNoteObject(n, selectedChart->meta.difficulties[currentDiff], selectedChart, notes);
	std::cout << "created |" << type << " beat: " << n.beat << std::endl;
	if (!dontSave)
	{
		saved[n.lane] = findNote(lane, beat);
		startBeats[n.lane] = beat;
	}
}


void deleteVisualNote(float lane, float beat)
{
	FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;
	int ind = 0;
	for (NoteObject* obj : notes)
	{
		if (obj->lane == lane && obj->beat == beat)
		{
			std::cout << "deleted " << obj->beat << " " << obj->lane << std::endl;
			editor->gameplay->removeObj(obj);
			notes.erase(notes.begin() + ind);
			break;
		}
		ind++;
	}
}

void deleteNote(int lane, float beat)
{
	selectedChart->meta.difficulties[currentDiff].notes.erase(
		std::remove_if(selectedChart->meta.difficulties[currentDiff].notes.begin(), selectedChart->meta.difficulties[currentDiff].notes.end(), [&](note n) {
			return n.beat == beat && n.lane == lane;
			}),
		selectedChart->meta.difficulties[currentDiff].notes.end());
	deleteVisualNote(lane, beat);
}


void window_chartProperties() {
	if (ImGui::CollapsingHeader("Chart Metadata"))
	{
		if (selectedChart)
		{
			ImGui::Text("Difficulties:");
			int ind = 0;
			for (difficulty& diff : selectedChart->meta.difficulties)
			{
				char buff[24];
				strcpy_s(buff, diff.name.c_str());
				ImGui::Separator();
				ImGui::PushItemWidth(165);
				ImGui::Text("Name:");
				ImGui::InputText(("##NAME" + std::to_string(ind)).c_str(), buff, sizeof(buff));
				if (std::string(buff).size() != 0)
					diff.name = std::string(buff);


				if (ImGui::CollapsingHeader(diff.name.c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					char buf[24];
					strcpy_s(buf, diff.charter.c_str());
					ImGui::PushItemWidth(165);
					ImGui::InputText(("##Charter" + std::to_string(ind)).c_str(), buf, sizeof(buf));
					ImGui::Text(("Notes: " + std::to_string(diff.notes.size())).c_str());
					diff.charter = std::string(buf);
					if (ImGui::Button("Load Difficulty"))
					{
						FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;
						currentDiff = ind;
						editor->loadNotes(diff);
					}
					if (ImGui::Button("Delete Difficulty"))
					{
						// TODO: Delete diff
					}
				}
				ind++;
			}
			ImGui::Separator();
			ImGui::Text("Global Metadata:");
			char buf[32];
			strcpy_s(buf, selectedChart->meta.background.c_str());
			ImGui::PushItemWidth(180);
			ImGui::InputText("Background", buf, sizeof(buf));
			selectedChart->meta.background = std::string(buf);
			ImGui::Separator();
			if (ImGui::Button("Create Difficulty"))
			{
				difficulty diff;
				diff.name = "Untitled Diff";
				diff.charter = "";
				selectedChart->meta.difficulties.push_back(diff);
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
	}
	if (ImGui::CollapsingHeader("Tempo"))
	{
		if (selectedChart)
		{
			int ind = 0;
			for (bpmSegment& seg : selectedChart->meta.bpms)
			{
				ind++;
				if (ImGui::CollapsingHeader(("BPM Segment #" + std::to_string(ind)).c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					double beat = seg.startBeat;
					ImGui::Text("Beat:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##Beat" + std::to_string(ind)).c_str(), &beat, 1, 10);
					double d = seg.bpm;
					ImGui::Text("BPM:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##BPM" + std::to_string(ind)).c_str(), &d, 1, 10);
					if (d < 1)
						d = 1;
					if (beat < 0)
						beat = 0;
					double endBeat = seg.endBeat;
					if (endBeat >= (INT_MAX - 10))
						ImGui::Text("End Beat: INF");
					else
						ImGui::Text(("End Beat: " + std::to_string(endBeat)).c_str());
					seg.bpm = d;
					seg.startBeat = beat;
					if (ind - 2 >= 0)
						selectedChart->meta.bpms[ind - 2].endBeat = beat;
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Create BPM Segment"))
			{
				bpmSegment seg;
				seg.bpm = 120;
				seg.endBeat = INT_MAX;
				seg.startBeat = currentBeat;
				selectedChart->meta.bpms.push_back(seg);
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
	}
	if (ImGui::CollapsingHeader("Stops"))
	{
		if (selectedChart)
		{
			int ind = 0;
			for (stopSegment& seg : selectedChart->meta.stops)
			{
				ind++;
				if (ImGui::CollapsingHeader(("Stop Segment #" + std::to_string(ind)).c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					double beat = seg.beat;
					ImGui::Text("Beat:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##SBeat" + std::to_string(ind)).c_str(), &beat, 1, 10);
					double d = seg.length;
					ImGui::Text("Stop Length (in ms):");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##SLength" + std::to_string(ind)).c_str(), &d, 1, 10);
					if (d < 1)
						d = 1;
					if (beat < 0)
						beat = 0;
					seg.length = d;
					seg.beat = beat;
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Create Stop Segment"))
			{
				stopSegment seg;
				seg.beat = currentBeat;
				seg.length = 0;
				selectedChart->meta.stops.push_back(seg);
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
	}
	if (ImGui::CollapsingHeader("Extras"))
	{
		ImGui::Text("Themes:");
		if (ImGui::Button("Dark Theme"))
			changeTheme(0);
		ImGui::SameLine();
		if (ImGui::Button("Light Theme"))
			changeTheme(1);
		ImGui::SameLine();
		if (ImGui::Button("Orange Theme"))
			changeTheme(2);
		if (ImGui::Button("Red Theme"))
			changeTheme(3);
		if (ImGui::Button("Clear Recent Charts"))
		{
			Game::save->SetString("nonChange_chartHistory", "");
			Game::save->Save();
		}
	}
}

void window_help() {
	ImGui::Text("TODO: help lol");
	if (ImGui::CollapsingHeader("Credits"))
	{
		ImGui::Text("Thank you to all of these developers that created these amazing UI tools!");
		ImGui::Separator();
		ImGui::Text("ocornut - Dear ImGUI");
		ImGui::Text("aiekick - ImGuiFileDialog");
		ImGui::Separator();
		ImGui::Text("Thank you to all of these developers who published these themes!");
		ImGui::Separator();
		ImGui::Text("janekb04 - Dark Theme (Deep Dark)");
		ImGui::Text("ebachard - Light Theme");
		ImGui::Text("CookiePLMonster - Orange Theme (Gold & Black)");
		ImGui::Text("Sewer56  - Red Theme");
	}
}

void window_notif() {
	ImGui::Text(notifTitle.c_str());
	ImGui::Text(notifText.c_str());
}

void FuckinEditor::create()
{
	addCamera(Game::mainCamera);

	for (int i = 0; i < 4; i++)
	{
		note n;
		n.beat = -1;
		saved[i] = n;
		tails[i] = n;
	}
	snapSelect = 3;
	snapConvert = { {4,1}, {8,2},{12,3},{16,4}, {24,6}, {32,8}, {64,16} };

	Judge::initJudge();

	changeTheme(Game::save->GetDouble("chartTheme"));

	Rect laneUnderway;

	laneUnderway.x = ((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * 3) - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size") + 12);
	laneUnderway.h = 1280;

	AvgSprite* lunder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underway.png"));
	add(lunder);
	lunder->alpha = 0.8;

	lunder->colorR = 35;
	lunder->colorG = 35;
	lunder->colorB = 35;

	AvgSprite* lunderBorder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underwayBorder.png"));
	add(lunderBorder);
	lunderBorder->alpha = 1;

	lunderBorder->colorR = 255;
	lunderBorder->colorG = 255;
	lunderBorder->colorB = 255;

	lunder->w = laneUnderway.w;
	lunder->h = laneUnderway.h;
	lunderBorder->w = laneUnderway.w;
	lunderBorder->h = laneUnderway.h;

	top = new AvgGroup(0, 0, 1280, 720);
	gameplay = new AvgGroup(0, 0, 1280, 720);

	for (int i = 0; i < 4; i++)
	{
		float x = (lunder->x + 12) + (((64 * Game::save->GetDouble("Note Size") + 12)) * i);

		ReceptorObject* r = new ReceptorObject(x, 140, i);
		r->defAlpha = 0.8;
		top->add(r);
		switch (i)
		{
		case 0:
			r->angle = 90;
			break;
		case 2:
			r->angle = 180;
			break;
		case 3:
			r->angle = -90;
			break;
		}
		r->w = (64 * Game::save->GetDouble("Note Size"));
		r->h = (64 * Game::save->GetDouble("Note Size"));
		fuck.push_back(r);
	}

	add(gameplay);
	add(top);

	createWindow("Chart Properties", { 400,400 }, (drawCall)window_chartProperties, false);
	createWindow("Help", { 550,350 }, (drawCall)window_help, false);
	createWindow("Notification", { 500,100 }, (drawCall)window_notif, false);
	findWindow("Notification").xOff = 12;
	findWindow("Help").xOff = 10;
	created = true;
}

void FuckinEditor::update(Events::updateEvent event)
{
	if (!selectedChart)
		return;

	for (NoteObject* obj : notes)
	{
		obj->rTime = currentTime;
		float wh = selectedChart->getTimeFromBeat(obj->beat, selectedChart->getSegmentFromBeat(obj->beat));
		if (songPlaying)
		{
			bool removeLayer = false;
			if (wh - currentTime < (Judge::hitWindows[1] * 0.5) && wh - currentTime > -(Judge::hitWindows[1] * 0.2))
				fuck[obj->lane]->lightUpTimer = 195;

			for (holdTile tile : obj->heldTilings)
			{
				wh = selectedChart->getTimeFromBeat(tile.beat, selectedChart->getSegmentFromBeat(tile.beat));
				if (wh - currentTime < (Judge::hitWindows[1] * 0.5) && wh - currentTime > -(Judge::hitWindows[1] * 0.2))
				{
					fuck[obj->lane]->lightUpTimer = 195;
				}
			}
		}

	}

	for (line& l : beatLines)
	{
		float diff = l.time - currentTime;

		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		float noteOffset = (bps * (diff / 1000)) * (64 * Game::save->GetDouble("Note Size"));
		l.rect->y = fuck[0]->y + noteOffset;
		l.text->y = l.rect->y - (l.text->surfH / 2);
		l.text->x = l.rect->x - (l.text->surfW + 4);
	}



	if (songPlaying)
	{
		float songPos = song->getPos();

		if (std::abs(((currentTime) - songPos) >= 0.05))
			currentTime = songPos;
		else
			currentTime += Game::deltaTime;

		bpmSegment curSeg = selectedChart->getSegmentFromTime(currentTime);
		currentBeat = selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}

	for (note& n : saved)
	{
		if (n.beat == -1)
			continue;

		float beatDiff = currentBeat - n.beat;

		if (beatDiff > 0 && tails[n.lane].beat != currentBeat)
		{
			// create a note if it doesn't exist
			if (tails[n.lane].beat != -1)
			{
				deleteNote(n.lane, tails[n.lane].beat);
			}
			tails[n.lane].beat = currentBeat;
			tails[n.lane].type = noteType::Note_Tail;
			tails[n.lane].lane = n.lane;

			note find = findNote(n.lane, currentBeat);
			if (find.beat == -1)
				createNote(n.lane, currentBeat, noteType::Note_Tail, n.beat, true);
			else
			{
				deleteNote(n.lane, find.beat);
				createNote(n.lane, currentBeat, noteType::Note_Tail, n.beat, true);
			}

			deleteNote(n.lane, n.beat);
			createNote(n.lane, n.beat, noteType::Note_Head);

			for (note nn : selectedChart->meta.difficulties[currentDiff].notes)
			{
				if (nn.beat > n.beat && nn.beat < tails[n.lane].beat && nn.lane == n.lane)
					deleteNote(nn.lane, nn.beat);
			}
		}
		else if (beatDiff < 0 && n.beat != currentBeat)
		{
			if (tails[n.lane].beat != -1)
			{
				deleteNote(n.lane, tails[n.lane].beat);
			}
			tails[n.lane].beat = startBeats[n.lane];
			tails[n.lane].type = noteType::Note_Tail;
			tails[n.lane].lane = n.lane;

			note find = findNote(n.lane, startBeats[n.lane]);
			if (find.beat == -1)
				createNote(n.lane, startBeats[n.lane], noteType::Note_Tail, currentBeat, true);
			else
			{
				deleteNote(n.lane, find.beat);
				createNote(n.lane, startBeats[n.lane], noteType::Note_Tail, currentBeat, true);
			}

			std::cout << "reset head and created tail | " << currentBeat << " " << startBeats[n.lane] << std::endl;

			deleteNote(n.lane, n.beat);
			createNote(n.lane, currentBeat, noteType::Note_Head, -1, true);

			n.beat = currentBeat;
		}

	}
}

bool openingFile = false;

void openChart(std::string path, std::string folder) {
	FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;
	SMFile* file = new SMFile(path, folder, true);
	selectedChart = new Chart(file->meta);
	delete file;

	if (!selectedChart)
	{
		triggerNotif("Failed to parse chart!", "Couldn't get the chart data, try again maybe?");
		return;
	}

	if (selectedChart->meta.difficulties.size() == 0)
	{
		selectedChart = nullptr;
		triggerNotif("Failed to parse chart!", "There weren't any difficulties, does the chart exist?");
		return;
	}

	std::string pathj = selectedChart->meta.folder + "/" + selectedChart->meta.audio;

	editor->song = SoundManager::createChannel(pathj.c_str(), "editorSong");

	editor->song->createFXStream();

	editor->loadNotes(selectedChart->meta.difficulties[0]);
	currentDiff = 0;
}

void fileMenu() {
	if (ImGui::MenuItem("New Chart") && !openingFile) {
		// TODO create a chart
	}
	if (ImGui::MenuItem("Open") && !openingFile) {
		openingFile = true;
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".sm", ".");
	}
	if (ImGui::BeginMenu("Open Recent") && !openingFile)
	{
		int ind = 0;
		std::vector<std::string> history = Chart::split(Game::save->GetString("nonChange_chartHistory"), '$');
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		if (history.size() == 0)
			ImGui::TextColored(colors[ImGuiCol_TextDisabled], "Open a chart for it to appear here!");
		else
			for (std::string file : history)
			{
				if (ind > 10)
					break;
				std::string name = file.substr(file.find_last_of("\\") + 1, file.size());
				if (ImGui::MenuItem(name.c_str()))
					openChart(file, file.substr(0, file.find_last_of("\\")));
				ind++;
			}
		ImGui::EndMenu();
	}
}


void FuckinEditor::imguiUpdate(float elapsed)
{
	focused = false;
	for (editorWindow& wind : windows)
	{
		if (!wind.shouldDraw)
			continue;
		ImGui::Begin(wind.title.c_str(), NULL, (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize));
		{
			ImGui::SetWindowSize(wind.size);
			ImGui::BeginMenuBar();
			{
				if (ImGui::MenuItem("Close"))
					wind.shouldDraw = !wind.shouldDraw;
				float wOff = ImGui::CalcTextSize(wind.title.c_str()).x;
				ImGui::SameLine(ImGui::GetWindowWidth() - (wOff + (wOff / 5) + wind.xOff));
				ImGui::Text(wind.title.c_str());
				ImGui::EndMenuBar();
			}
			wind.draw();
			if (ImGui::IsWindowFocused())
				focused = true;
		}
		ImGui::End();
	}
	if (openingFile)
		focused = true;

	if (openingFile)
	{
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, { 640, 360 }, {800, 600}))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				if (filePathName.find_first_of(".sm") != -1)
				{
					std::string copy = filePathName;
					std::string folder = filePathName.substr(0, filePathName.find_last_of("\\"));
					if (selectedChart)
						selectedChart->destroy();
					openChart(filePathName, folder);
					Game::save->SetString("nonChange_chartHistory", copy + "$" + Game::save->GetString("nonChange_chartHistory"));
					Game::save->Save();
				}
			}
			openingFile = false;
			ImGuiFileDialog::Instance()->Close();
		}
	}

	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			fileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Chart Properties"))
		{
			editorWindow& wind = findWindow("Chart Properties");
			wind.shouldDraw = !wind.shouldDraw;
		}
		if (ImGui::MenuItem("Help"))
		{
			editorWindow& wind = findWindow("Help");
			wind.shouldDraw = !wind.shouldDraw;
		}
	}
	ImGui::EndMainMenuBar();
	if (selectedChart)
	{
		ImGui::Begin("BottomInfo", NULL, (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize));
		ImGui::SetWindowPos({ 15,660 });
		ImGui::SetWindowSize({ 350,50 });

		ImGui::Text("Time: %.2f", currentTime / 1000);
		ImGui::SameLine();  ImGui::Text("Beat: %.2f", currentBeat);
		ImGui::SameLine();  ImGui::Text("BPM: %.2f", selectedChart->getSegmentFromTime(currentTime).bpm);
		ImGui::SameLine();  ImGui::Text("Snap: %i", snap);
		ImGui::End();
	}
}


void FuckinEditor::keyUp(SDL_KeyboardEvent event)
{
	if (focused)
		return;
	note n;
	n.beat = -1;
	if (selectedChart)
		switch (event.keysym.sym)
		{
			case SDLK_1:
				saved[0] = n;
				break;
			case SDLK_2:
				saved[1] = n;
				break;
			case SDLK_3:
				saved[2] = n;
				break;
			case SDLK_4:
				saved[3] = n;
				break;
		}
}

void FuckinEditor::keyDown(SDL_KeyboardEvent event)
{
	if (focused)
		return;
	if (event.keysym.sym == SDLK_ESCAPE)
	{
		if (selectedChart)
			selectedChart->destroy();
		Game::instance->transitionToMenu(new MainMenu());

	}
	if (event.keysym.sym == SDLK_SPACE && selectedChart)
	{
		if (songPlaying)
			song->stop();
		else
		{
			song->setPos(currentTime);
			song->play();
		}
		songPlaying = !songPlaying;
	}

	if (event.keysym.sym == SDLK_UP && selectedChart)
	{
		currentTime -= 100;
		bpmSegment curSeg = selectedChart->getSegmentFromTime(currentTime);
		currentBeat = selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}

	if (event.keysym.sym == SDLK_DOWN && selectedChart)
	{
		currentTime += 100;
		bpmSegment curSeg = selectedChart->getSegmentFromTime(currentTime);
		currentBeat = selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}

	if (event.keysym.sym == SDLK_LEFT)
		snapSelect--;
	if (event.keysym.sym == SDLK_RIGHT)
		snapSelect++;

	if (snapSelect < 0)
		snapSelect = snapConvert.size() - 1;
	if (snapSelect > snapConvert.size() - 1)
		snapSelect = 0;

	auto it = snapConvert.begin();
	std::advance(it, snapSelect);
	snap = it->first;

	if (currentTime < 0)
	{
		currentBeat = 0;
		bpmSegment curSeg = selectedChart->getSegmentFromBeat(currentBeat);
		currentTime = selectedChart->getTimeFromBeat(currentBeat, curSeg);
	}

	if (selectedChart)
		switch (event.keysym.sym)
		{
		case SDLK_1:
			if (saved[0].beat != -1)
				return;
			if (findNote(0, currentBeat).beat != -1)
				deleteNote(0, currentBeat);
			else
				createNote(0);
			break;
		case SDLK_2:
			if (saved[1].beat != -1)
				return;
			if (findNote(1, currentBeat).beat != -1)
				deleteNote(1, currentBeat);
			else
				createNote(1);
			break;
		case SDLK_3:
			if (saved[2].beat != -1)
				return;
			if (findNote(2, currentBeat).beat != -1)
				deleteNote(2, currentBeat);
			else
				createNote(2);
			break;
		case SDLK_4:
			if (saved[3].beat != -1)
				return;
			if (findNote(3, currentBeat).beat != -1)
				deleteNote(3, currentBeat);
			else
				createNote(3);
			break;
		}
}

void FuckinEditor::mouseWheel(float wheel)
{
	if (focused)
		return;
	if (!selectedChart)
		return;
	float amount = wheel;
	float increase = 0;
	float beats = 0;
	if (amount < 0)
	{
		increase = (float)1 / (float)snapConvert[snap];
		beats = (floor((currentBeat * (float)snapConvert[snap]) + 0.001) / (float)snapConvert[snap]) + increase;
	}
	else
	{
		increase = (float)-1 / (float)snapConvert[snap];
		beats = ((ceil(currentBeat * (float)snapConvert[snap]) - 0.001) / (float)snapConvert[snap]) + increase;
	}
	if (beats < 0)
		beats = 0;
	currentBeat = beats;
	currentTime = selectedChart->getTimeFromBeat(beats, selectedChart->getSegmentFromBeat(beats));
	if (songPlaying)
	{
		song->setPos(currentTime);
	}
}


void FuckinEditor::loadNotes(difficulty diff)
{
	currentTime = 0;
	currentBeat = 0;
	if (songPlaying)
	{
		song->stop();
		songPlaying = false;
	}
	std::vector<Object*> objs = gameplay->children;
	for (Object* obj : objs)
	{
		gameplay->removeObj(obj);
	}
	beatLines.clear();
	notes.clear();
	bool downscroll = false;
	regenBeatLines(selectedChart);
	for (note n : diff.notes)
	{
		generateNoteObject(n, diff, selectedChart, notes);
	}
}
