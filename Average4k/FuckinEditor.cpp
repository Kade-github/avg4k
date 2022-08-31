#include "FuckinEditor.h"
#include "ImGuiFileDialog.h"
#include "MainMenu.h"
#include "Judge.h"
#include "ModEditor.h"

// window draws

Chart* FuckinEditor::selectedChart = NULL;
bool FuckinEditor::dontDeleteChart = false;
float currentTime;
float currentBeat;
std::vector<NoteObject*> notes;

Color wavec = { 255,255,255 };
float waveformAlpha = 1;

std::vector<note> copiedNotes;
std::vector<note> pastedNotes;
std::vector<note> deletedNotes;

int snapSelect = 0;
float startBeats[4] = {};
note saved[4] = {};
note tails[4] = {};

bool downscroll = false;

bool topLayer = false;
int FuckinEditor::currentDiff = 0;
int snap = 16;

float speed = 1;

int lastBeatClicked = 0;

std::map<float, float> snapConvert;

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

bool compareByBeatBPM(const bpmSegment& a, const bpmSegment& b)
{
	return a.startBeat < b.startBeat;
}

bool compareByBeatStop(const stopSegment& a, const stopSegment& b)
{
	return a.beat < b.beat;
}

bool compareNoteByBeat(const NoteObject* a, const NoteObject* b)
{
	return a->beat < b->beat;
}


void resortBPMS()
{
	std::sort(FuckinEditor::selectedChart->meta.bpms.begin(), FuckinEditor::selectedChart->meta.bpms.end(), compareByBeatBPM);

	int ind = 0;
	for (bpmSegment& seg : FuckinEditor::selectedChart->meta.bpms)
	{
		seg.endBeat = INT_MAX;
		seg.length = INT_MAX;
		if (ind != 0)
		{
			bpmSegment& prev = FuckinEditor::selectedChart->meta.bpms[ind - 1];
			prev.endBeat = seg.startBeat;
			prev.length = ((prev.endBeat - prev.startBeat) / (prev.bpm / 60)) * 1000;
			seg.startTime = prev.startTime + prev.length;
		}
		ind++;
	}
}

void triggerNotif(std::string title, std::string text)
{
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;

	notifTitle = title;
	notifText = text;

	editor->findWindow("Notification").shouldDraw = true;
}

note findNote(int lane, float beat)
{
	note struc;
	struc.beat = -1;
	for (note& n : FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes)
		if (n.beat == beat && n.lane == lane)
			return n;
	return struc;
}

float beatMath(float amount, float increase, float startBeat)
{
	float beats = 0.0f;
	if (amount < 0)
	{
		float flor = roundf(startBeat * static_cast<float>(snapConvert[snap]));
		beats = (flor / static_cast<float>(snapConvert[snap])) + increase;
	}
	else
	{
		float ceal = roundf(startBeat * static_cast<float>(snapConvert[snap]));
		beats = (ceal / static_cast<float>(snapConvert[snap])) - increase;
	}
	return beats;
}

void deleteVisualNote(float lane, float beat)
{
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	int ind = 0;
	for (line l : editor->miniMapLines)
	{
		if (l.beat == beat && l.lane == lane)
		{
			editor->miniMap->removeObj(l.rect);
		}
	}
	editor->miniMapLines.erase(
		std::remove_if(editor->miniMapLines.begin(), editor->miniMapLines.end(), [&](line l) {
			return l.beat == beat && l.lane == lane;
			}),
		editor->miniMapLines.end());
	for (NoteObject* obj : notes)
	{
		if (obj->lane == lane && obj->beat == beat)
		{
			editor->gameplay->removeObj(obj);
			notes.erase(notes.begin() + ind);
			break;
		}
		ind++;
	}
}

void getShitBetween()
{
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	float minBeat = currentBeat - 4;
	if (minBeat < 0)
		minBeat = 0;
	editor->lastBeatMin = minBeat;
	float maxBeat = currentBeat + 4;
	editor->lastBeatMax = maxBeat;


	bpmSegment minSeg = FuckinEditor::selectedChart->getSegmentFromBeat(minBeat);
	bpmSegment maxSeg = FuckinEditor::selectedChart->getSegmentFromBeat(maxBeat);

	float minTime = FuckinEditor::selectedChart->getTimeFromBeatOffset(minBeat, minSeg);
	float maxTime = FuckinEditor::selectedChart->getTimeFromBeatOffset(maxBeat, maxSeg);


	editor->generateWaveForm(minTime, maxTime);
}

void deleteNote(int lane, float beat)
{
	FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.erase(
		std::remove_if(FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.begin(), FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.end(), [&](note n) {
			return n.beat == beat && n.lane == lane;
			}),
		FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.end());
	deleteVisualNote(lane, beat);
}


void createNote(int lane, float beat = currentBeat, noteType type = noteType::Note_Normal, float connectedBeat = -1, bool dontSave = false, bool findTail = true) {
	note n;
	n.beat = beat;
	n.lane = lane;
	n.type = type;
	n.connectedBeat = connectedBeat;
	FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.push_back(n);
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	editor->generateNoteObject(n, FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff], FuckinEditor::selectedChart, notes, findTail);
	std::sort(notes.begin(), notes.end(), compareNoteByBeat);
	for (NoteObject* obj : notes)
	{
		if (obj->type == noteType::Note_Head)
		{
			if (obj->tailBeat > n.beat && obj->beat < n.beat && obj->lane == lane)
			{
				float storeBeat = obj->beat;
				float endBeat = obj->endBeat;
				deleteNote(n.lane, storeBeat);

				float nEndBeat = endBeat - (endBeat - n.beat) - beatMath(-1, 1 / snapConvert[snap], 0);

				if (nEndBeat > 0)
					createNote(n.lane, storeBeat, noteType::Note_Head, nEndBeat, true, false);
				else
					createNote(n.lane, storeBeat, noteType::Note_Normal,-1,true);
			}
		}
	}

	if (!dontSave)
	{
		saved[n.lane] = findNote(lane, beat);
		startBeats[n.lane] = beat;
	}
}
void window_waveProperties() {
	bool shit = Game::save->GetBool("nonChange_chartWaveform");
	ImGui::Checkbox("Show Waveform", &shit);
	Game::save->SetBool("nonChange_chartWaveform", shit);

	if (Game::save->GetString("nonChange_colorShit") == "")
		Game::save->SetString("nonChange_colorShit", "128,128,255");

	std::string bruh = Game::save->GetString("nonChange_colorShit");

	std::vector<std::string> colorSaved = Chart::split(bruh, ',');

	float c[3] = {std::stof(colorSaved[0]) / 255,std::stof(colorSaved[1]) / 255,std::stof(colorSaved[2]) / 255 };
	ImGui::Text("Waveform Color:");
	ImGui::ColorEdit3("##WaveformColor",(float*)&c);
	Game::save->SetString("nonChange_colorShit", std::to_string(c[0] * 255) + "," + std::to_string(c[1] * 255) + "," + std::to_string(c[2] * 255));

	wavec = { static_cast<int>(c[0] * 255),static_cast<int>(c[1] * 255),static_cast<int>(c[2] * 255) };
}

void window_chartProperties() {
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	ImGui::BeginTabBar("##Chart Metadata");
	if (ImGui::BeginTabItem("Editor"))
	{
		ImGui::Text("Bass Offset (debug):");
		ImGui::InputFloat("##BassOffset", &FuckinEditor::selectedChart->BASS_OFFSET, 0.001, 0.01);
	}
	if (ImGui::BeginTabItem("Metadata"))
	{
		if (FuckinEditor::selectedChart)
		{
			float offset = FuckinEditor::selectedChart->meta.chartOffset;
			ImGui::Text("Chart Offset:");
			ImGui::InputFloat("##Offset", &offset, .001, .01, "%.3f");
			if (FuckinEditor::selectedChart->meta.chartOffset != offset)
				FuckinEditor::selectedChart->meta.chartOffset = offset;

			char buf[32];
			strcpy_s(buf, FuckinEditor::selectedChart->meta.background.c_str());
			ImGui::PushItemWidth(180);
			ImGui::Text("Chart Background:");
			ImGui::InputText("##Background", buf, sizeof(buf));
			FuckinEditor::selectedChart->meta.background = std::string(buf);

			char buffff[32];
			strcpy_s(buffff, FuckinEditor::selectedChart->meta.banner.c_str());
			ImGui::PushItemWidth(180);
			ImGui::Text("Chart Banner:");
			ImGui::InputText("##Banner", buffff, sizeof(buffff));
			FuckinEditor::selectedChart->meta.banner = std::string(buffff);

			char buff[32];
			strcpy_s(buff, FuckinEditor::selectedChart->meta.songName.c_str());
			ImGui::PushItemWidth(180);
			ImGui::Text("Song Title:");
			ImGui::InputText("##Title", buff, sizeof(buff));
			FuckinEditor::selectedChart->meta.songName = std::string(buff);


			char bufff[32];
			strcpy_s(bufff, FuckinEditor::selectedChart->meta.audio.c_str());
			ImGui::PushItemWidth(180);
			ImGui::Text("Song Audio:");
			ImGui::InputText("##Audio", bufff, sizeof(bufff));
			if (FuckinEditor::selectedChart->meta.audio != std::string(bufff))
			{
				FuckinEditor::selectedChart->meta.audio = std::string(bufff);
				std::string pathj = FuckinEditor::selectedChart->meta.folder + "/" + FuckinEditor::selectedChart->meta.audio;

				if (editor->song)
				{
					editor->song->stop();
					editor->song->free();
				}
				editor->song = SoundManager::createChannel(pathj.c_str(), "editorSong");

				editor->song->createFXStream();
				editor->generateWaveForm(0, editor->song->length);
				editor->song->setVolume(0.4);
			}

		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Diffs"))
	{
		if (FuckinEditor::selectedChart)
		{
			int ind = 0;
			for (difficulty& diff : FuckinEditor::selectedChart->meta.difficulties)
			{
				char buff[24];
				strcpy_s(buff, diff.name.c_str());
				ImGui::Separator();
				ImGui::PushItemWidth(165);
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
					if (ImGui::Button(("Load Difficulty##DiffLoad" + std::to_string(ind)).c_str()))
					{
						FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
						FuckinEditor::currentDiff = ind;
						editor->loadNotes(diff);
					}
					if (ImGui::Button(("Delete Difficulty##DiffDelete" + std::to_string(ind)).c_str()))
					{
						// TODO: Delete diff
					}
				}
				ind++;
			}
			ImGui::Separator();
			if (ImGui::Button("Create Difficulty"))
			{
				difficulty diff;
				diff.name = "Untitled Diff";
				diff.charter = "";
				FuckinEditor::selectedChart->meta.difficulties.push_back(diff);
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Tempo"))
	{
		if (FuckinEditor::selectedChart)
		{

			int ind = 0;
			for (bpmSegment& seg : FuckinEditor::selectedChart->meta.bpms)
			{
				ind++;
				if (ImGui::CollapsingHeader(("BPM Segment #" + std::to_string(ind)).c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					double beat = seg.startBeat;
					ImGui::Text("Beat:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##Beat" + std::to_string(ind)).c_str(), &beat, 0.01, 0.1, "%.2f");
					double d = seg.bpm;
					ImGui::Text("BPM:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##BPM" + std::to_string(ind)).c_str(), &d, 0.01, 0.1, "%.2f");
					if (d < 1)
						d = 1;
					if (beat < 0)
						beat = 0;
					double endBeat = seg.endBeat;
					if (endBeat >= (INT_MAX - 10))
						ImGui::Text("End Beat: INF");
					else
						ImGui::Text(("End Beat: " + std::to_string(endBeat)).c_str());

					if (seg.startBeat != beat)
					{
						seg.startBeat = beat;
						resortBPMS();
						editor->generateSnapLines(FuckinEditor::selectedChart, snapConvert[snap]);
						editor->regenBeatLines(FuckinEditor::selectedChart);
						editor->regenThings(FuckinEditor::selectedChart);
					}
					if (seg.bpm != d)
					{
						seg.bpm = d;
						resortBPMS();
						editor->generateSnapLines(FuckinEditor::selectedChart, snapConvert[snap]);
						editor->regenBeatLines(FuckinEditor::selectedChart);
						editor->regenThings(FuckinEditor::selectedChart);
					}

					if (ind - 2 >= 0)
						FuckinEditor::selectedChart->meta.bpms[ind - 2].endBeat = beat;
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Create BPM Segment"))
			{
				bpmSegment seg;
				seg.bpm = 120;
				seg.endBeat = INT_MAX;
				seg.startBeat = currentBeat;
				seg.length = INT_MAX;
				FuckinEditor::selectedChart->meta.bpms.push_back(seg);
				resortBPMS();
				editor->generateSnapLines(FuckinEditor::selectedChart, snapConvert[snap]);
				editor->regenBeatLines(FuckinEditor::selectedChart);
				editor->regenThings(FuckinEditor::selectedChart);
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Stops"))
	{
		if (FuckinEditor::selectedChart)
		{
			FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
			int ind = 0;
			for (stopSegment& seg : FuckinEditor::selectedChart->meta.stops)
			{
				ind++;
				if (ImGui::CollapsingHeader(("Stop Segment #" + std::to_string(ind)).c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					double beat = seg.beat;
					ImGui::Text("Beat:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##SBeat" + std::to_string(ind)).c_str(), &beat, 0.01, 0.1, "%.2f");
					double d = seg.length / 1000;
					ImGui::Text("Stop Length (in seconds):");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##SLength" + std::to_string(ind)).c_str(), &d, 0.01, 0.1, "%.2f");
					if (d < 1)
						d = 1;
					if (beat < 0)
						beat = 0;
					if (seg.length / 1000 != d)
					{
						seg.length = d * 1000;
						FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
						for (thingy t : editor->sideStuff)
						{
							editor->gameplay->removeObj(t.background);
							editor->gameplay->removeObj(t.text);
						}
						editor->sideStuff.clear();
						editor->regenThings(FuckinEditor::selectedChart);
						float prevTime = currentTime;
						editor->loadNotes(FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff]);
						currentTime = prevTime;
					}
					if (seg.beat != beat)
					{
						seg.beat = beat;
						FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
						for (thingy t : editor->sideStuff)
						{
							editor->gameplay->removeObj(t.background);
							editor->gameplay->removeObj(t.text);
						}
						editor->sideStuff.clear();
						editor->regenThings(FuckinEditor::selectedChart);

						float prevTime = currentTime;
						editor->loadNotes(FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff]);
						currentTime = prevTime;
					}
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Create Stop Segment"))
			{
				stopSegment seg;
				seg.beat = currentBeat;
				seg.length = 0.01;
				FuckinEditor::selectedChart->meta.stops.push_back(seg);
				float prevTime = currentTime;
				editor->loadNotes(FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff]);
				currentTime = prevTime;
			}
		}
		else
		{
			ImGui::Text("Please select a chart first");
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("BPM Detect"))
	{
		ImGui::Text("Tempo Detection Results:");
		int ind = 0;
		if (editor->song->bpmCan.size() == 0 && editor->triedBPM)
		{
			ImGui::Text("Seems like I failed to detect the bpm, uh. woops, idfk.");
		}
		for (double d : editor->song->bpmCan)
		{
			ImGui::PushItemWidth(100);
			double dd = d;
			ImGui::InputDouble(("##Beat" + std::to_string(d)).c_str(), &dd, 0, 0, "%.2f");
			ind++;
			if (ind > 8)
				break;
		}
		if (ImGui::Button("Detect Tempo"))
		{
			editor->triedBPM = true;
			float sampl = (editor->song->returnSampleRate() * (editor->song->length / 1000)) / 2;
			float* samples = editor->song->returnSamples(sampl, NULL);
			editor->song->bpmDetect(samples, sampl, true);
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Extras"))
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
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
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

void window_debug() {
	ImGui::Text(("beat: " + std::to_string(currentBeat)).c_str());
	ImGui::Text(("time: " + std::to_string(currentTime)).c_str());
	ImGui::Text(("snap: " + std::to_string(snap)).c_str());
	ImGui::Text(("snapDivBeat: " + std::to_string((float)1 / (float)snapConvert[snap])).c_str());
	ImGui::Text(("Ceil: " + std::to_string(ceil((float)(currentBeat * snapConvert[snap])))).c_str());
	ImGui::Text(("CeilDiv: " + std::to_string(ceil((currentBeat * (float)snapConvert[snap])) / (float)snapConvert[snap])).c_str());
	ImGui::Text(("Floor: " + std::to_string(floor((float)(currentBeat * snapConvert[snap])))).c_str());
	ImGui::Text(("FloorDiv: " + std::to_string(floor((currentBeat * (float)snapConvert[snap])) / (float)snapConvert[snap])).c_str());
}


void window_notif() {
	ImGui::Text(notifTitle.c_str());
	ImGui::Text(notifText.c_str());
}

bool openingFile = false;

void openChart(std::string path, std::string folder) {
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;

	editor->currentFile = path;

	SMFile* file = new SMFile(path, folder, true);
	if (FuckinEditor::selectedChart)
	{
		delete FuckinEditor::selectedChart;
	}
	FuckinEditor::selectedChart = new Chart(file->meta);
	delete file;
	editor->triedBPM = false;



	if (!FuckinEditor::selectedChart)
	{
		triggerNotif("Failed to parse chart!", "Couldn't get the chart data, try again maybe?");
		return;
	}

	if (FuckinEditor::selectedChart->meta.difficulties.size() == 0)
	{
		FuckinEditor::selectedChart = nullptr;
		triggerNotif("Failed to parse chart!", "There weren't any difficulties, does the chart exist?");
		return;
	}

	std::string pathj = FuckinEditor::selectedChart->meta.folder + "/" + FuckinEditor::selectedChart->meta.audio;

	if (editor->song)
		editor->song->free();

	editor->song = SoundManager::createChannel(pathj.c_str(), "editorSong");

	editor->song->createFXStream();
	editor->generateWaveForm(0, editor->song->length);
	editor->loadNotes(FuckinEditor::selectedChart->meta.difficulties[0]);
	editor->song->setVolume(0.4);
	FuckinEditor::currentDiff = 0;
}


void FuckinEditor::create()
{

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
		Channel* c = SoundManager::getChannelByName("prevSong");
		c->stop();
	}
	Game::useImGUI = true;
	//Game::showErrorWindow("Warning", "editor is super wip, your game is fucked.", false);
	noteZoom = 1;
	
	downscroll = Game::save->GetBool("downscroll");

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

	laneUnderway.x = ((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) + ((64 * noteZoom + 12) * 3) - laneUnderway.x) + (68 * noteZoom + 12);
	laneUnderway.h = 1280;

	lunder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underway.png"));
	add(lunder);
	lunder->alpha = 0.8;

	lunder->colorR = 35;
	lunder->colorG = 35;
	lunder->colorB = 35;
	float w = 42;
	float x = Game::gameWidth - (w + 6);
	float y = 24;
	float h = 690;

	miniMapBorder = new AvgRect(x, y, w, h);
	miniMapBorder->c = { 255,255,255 };
	add(miniMapBorder);

	AvgRect* rectShitBitch = new AvgRect(x + 2, y + 2, w - 4, h - 4);
	add(rectShitBitch);


	miniMapCursor = new AvgRect(x, y, 42, 4);
	miniMapCursor->c = { 255,255,255 };;

	lunderBorder = new AvgSprite(laneUnderway.x, laneUnderway.y, Noteskin::getGameplayElement(Game::noteskin, "underwayBorder.png"));
	add(lunderBorder);
	lunderBorder->alpha = 1;

	lunderBorder->colorR = 255;
	lunderBorder->colorG = 255;
	lunderBorder->colorB = 255;
	selectionRect = new AvgRect(0, 0, 0, 0);

	lunder->w = laneUnderway.w;
	lunder->h = laneUnderway.h;
	lunderBorder->w = laneUnderway.w;
	lunderBorder->h = laneUnderway.h;
	lines = new AvgGroup(0, 0, 1280, 720);
	wave = new AvgGroup(0, 0, 1280, 720);
	top = new AvgGroup(0, 0, 1280, 720);
	gameplay = new AvgGroup(0, 0, 1280, 720);
	miniMap = new AvgGroup(rectShitBitch->x, rectShitBitch->y, rectShitBitch->w, rectShitBitch->h);


	for (int i = 0; i < 4; i++)
	{
		float x = (lunder->x + 12) + ((((64 * noteZoom) + 12)) * i);

		float y = 140;
		if (downscroll)
			y = 580;

		ReceptorObject* r = new ReceptorObject(x, y, i);
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
		r->w = (64 * noteZoom);
		r->h = (64 * noteZoom);
		fuck.push_back(r);
	}
	add(wave);
	add(lines);
	add(gameplay);
	add(top);
	add(miniMap);
	add(miniMapCursor);
	top->add(selectionRect);
	selectionRect->alpha = 0.6;
	selectionRect->c = { 255,255,255 };

	std::vector<std::string> colorSaved = Chart::split(Game::save->GetString("nonChange_colorShit"), ',');
	wavec = { std::atoi(colorSaved[0].c_str()),std::atoi(colorSaved[1].c_str()),std::atoi(colorSaved[2].c_str()) };

	createWindow("Chart Properties", { 400,400 }, (drawCall)window_chartProperties, false);
	createWindow("Waveform Properties", { 400,400 }, (drawCall)window_waveProperties, false);
	createWindow("Help", { 550,350 }, (drawCall)window_help, false);
	createWindow("Notification", { 500,100 }, (drawCall)window_notif, false);
	findWindow("Notification").xOff = 12;
	findWindow("Help").xOff = 10;
	createWindow("Debug Window", { 500,500 }, (drawCall)window_debug, false);

	if (!dontDeleteChart)
		FuckinEditor::selectedChart = NULL;
	else
	{
		std::string folder = selectedChart->meta.folder;
		std::vector<std::string> history = Chart::split(Game::save->GetString("nonChange_chartHistory"), '$');
		std::string path = history[0];
		FuckinEditor::selectedChart = NULL;
		openChart(path, folder);
	}
	dontDeleteChart = false;

	created = true;
}

bool mousePressed = false;

int initalSelectionX, initalSelectionY;


void FuckinEditor::leftMouseDown()
{
	int x, y;
	Game::GetMousePos(&x, &y);

	initalSelectionX = x;
	initalSelectionY = y;

	selectionRect->x = x;
	selectionRect->y = y;


	mousePressed = true;
}

void FuckinEditor::leftMouseUp()
{
	for (NoteObject* n : notes)
	{
		if ((n->y >= selectionRect->y && n->y + n->h <= selectionRect->y + selectionRect->h) &&
			(n->x >= selectionRect->x && n->x + n->w <= selectionRect->x + selectionRect->w))
			n->selected = true;
		else
			n->selected = false;
	}

	selectionRect->w = 0;
	selectionRect->h = 0;
	mousePressed = false;
}

void FuckinEditor::update(Events::updateEvent event)
{
	if (!FuckinEditor::selectedChart)
		return;
	if (notes.size() > 0)
	{
		float perc = (currentTime - (FuckinEditor::selectedChart->meta.chartOffset * 1000)) / song->length;
		if (perc > 1)
			perc = 1;
		miniMapCursor->y = (miniMap->y + (miniMap->h * (perc))) + (miniMapCursor->h / 2);

		int x, y;
		Game::GetMousePos(&x, &y);

		if (FuckinEditor::selectedChart && mousePressed)
		{
			if ((x > miniMap->x && x < miniMap->x + miniMap->w) &&
				(y > miniMap->y && y < miniMap->y + miniMap->h))
			{
				float relativeY = y - miniMap->y;
				int time = (song->length * (relativeY / miniMap->h)) - (FuckinEditor::selectedChart->meta.chartOffset * 1000);
				currentTime = time;
				bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);
				currentBeat = FuckinEditor::selectedChart->getBeatFromTime(currentTime, curSeg);
			}
			else
			{
				selectionRect->y = min(y, static_cast<int>(initalSelectionY));
				selectionRect->x = min(x, static_cast<int>(initalSelectionX));
				selectionRect->w = abs(x - initalSelectionX);
				selectionRect->h = abs(y - initalSelectionY);
			}
		}

		if (Game::save->GetBool("nonChange_minimap"))
		{

			for (line& l : miniMapLines)
			{
				l.rect->alpha = 1;
				l.rect->drawCall = true;
				
				float tim = l.time;

				float notePerc = tim / song->length;

				l.rect->y = (miniMap->y + (miniMap->h * (notePerc))) + (l.rect->h / 2);
			}
		}
		else
		{
			for (line& l : miniMapLines)
			{
				l.rect->alpha = 0;
				l.rect->drawCall = false;
			}
		}
	}

	for (NoteObject* obj : notes)
	{
		if (obj->y > 720 && obj->y + 64 < 0)
		{
			obj->drawCall = false;
			continue;
		}
		else
			obj->drawCall = true;
		obj->rTime = currentTime;
		float wh = FuckinEditor::selectedChart->getTimeFromBeat(obj->beat, FuckinEditor::selectedChart->getSegmentFromBeat(obj->beat));
		if (songPlaying)
		{
			bool removeLayer = false;
			if (wh - abs(currentTime) < (Judge::hitWindows[1] * 0.5) && wh - abs(currentTime) > -(Judge::hitWindows[1] * 0.2))
			{
				fuck[obj->lane]->lightUpTimer = 195;
				if (!findClapped(obj->beat) && Game::save->GetBool("nonChange_noteTick"))
				{
					beatsClapped.push_back(obj->beat);
					Channel* c = SoundManager::createChannel("assets/sounds/hitSound.wav", "clap_" + std::to_string(obj->beat), true);
					c->play();
					c->setVolume(1);
					clapChannels.push_back(c);
				}
			}

			for (holdTile tile : obj->heldTilings)
			{
				wh = FuckinEditor::selectedChart->getTimeFromBeat(tile.beat, FuckinEditor::selectedChart->getSegmentFromBeat(tile.beat));
				if (wh - abs(currentTime) < (Judge::hitWindows[1] * 0.5) && wh - abs(currentTime) > -(Judge::hitWindows[1] * 0.2))
				{
					fuck[obj->lane]->lightUpTimer = 195;
				}
			}
		}

	}


	int showBeatLines = Game::save->GetBool("nonChange_beatLines") ? 1 : 0;

	for (line& l : snapBeat)
	{
		l.rect->alpha = showBeatLines;

		float noteOffset = Helpers::calculateCMODY(Game::save->GetDouble("scrollspeed") / 60, FuckinEditor::selectedChart->getTimeFromBeat(l.beat, FuckinEditor::selectedChart->getSegmentFromBeat(l.beat)), currentTime, 64 * noteZoom);
		if (downscroll)
			l.rect->y = (fuck[0]->y) - noteOffset + (32 * noteZoom);
		else
			l.rect->y = (fuck[0]->y) + noteOffset + (32 * noteZoom);

		l.rect->x = lunder->x;
		l.rect->w = lunder->w;
		if (showBeatLines == 1)
		{
			if (l.rect->y > 720 || l.rect->y + l.rect->h < 0)
				l.rect->drawCall = false;
			else
				l.rect->drawCall = true;
		}
		else
			l.rect->drawCall = false;
		if (Game::save->GetBool("nonChange_beatTick"))
		{
			bool clicked = false;
			for (line ll : clappedLines)
			{
				if (ll.beat == l.beat)
					clicked = true;
			}
			if (!clicked)
			{
				float diff = l.beat - currentBeat;

				if (fabs(diff) < 0.1)
				{
					clappedLines.push_back(l);
					Channel* c = SoundManager::createChannel("assets/sounds/beatTick.wav", "beat_click_" + std::to_string(currentBeat), true);
					c->play();
					c->setVolume(1);
					clapChannels.push_back(c);
				}
			}
		}
	}


	for (line& l : beatLines)
	{
		l.rect->alpha = showBeatLines;

		l.text->alpha = showBeatLines;

		float noteOffset = Helpers::calculateCMODY(Game::save->GetDouble("scrollspeed") / 60, FuckinEditor::selectedChart->getTimeFromBeat(l.beat, FuckinEditor::selectedChart->getSegmentFromBeat(l.beat)) + (FuckinEditor::selectedChart->getStopOffsetFromBeat(l.beat)), currentTime, 64 * noteZoom);
		if (downscroll)
			l.rect->y = (fuck[0]->y) - noteOffset + (32 * noteZoom);
		else
			l.rect->y = (fuck[0]->y) + noteOffset + (32 * noteZoom);
		l.rect->x = lunder->x;
		l.rect->w = lunder->w;
		l.text->y = l.rect->y - (l.text->surfH / 2);
		l.text->x = l.rect->x - (l.text->surfW + 4);
		if (showBeatLines == 1)
		{
			if (l.rect->y > 720 || l.rect->y + l.rect->h < 0)
				l.rect->drawCall = false;
			else
				l.rect->drawCall = true;
		}
		else
			l.rect->drawCall = false;

		if (Game::save->GetBool("nonChange_beatTick"))
		{
			bool clicked = false;
			for (line ll : clappedLines)
			{
				if (ll.beat == l.beat)
					clicked = true;
			}
			if (!clicked)
			{
				float diff = l.beat - currentBeat;

				if (fabs(diff) < 0.1)
				{
					clappedLines.push_back(l);
					Channel* c = SoundManager::createChannel("assets/sounds/beatTick.wav", "beat_click_" + std::to_string(currentBeat), true);
					c->play();
					c->setVolume(1);
					clapChannels.push_back(c);
				}
			}
		}
	}

	for (thingy& l : sideStuff)
	{
		l.text->drawCall = true;
		float noteOffset = Helpers::calculateCMODY(Game::save->GetDouble("scrollspeed") / 60, FuckinEditor::selectedChart->getTimeFromBeat(l.beat, FuckinEditor::selectedChart->getSegmentFromBeat(l.beat)) + (FuckinEditor::selectedChart->getStopOffsetFromBeat(l.beat)), currentTime, 64 * noteZoom);
		if (downscroll)
			l.background->y = (fuck[0]->y - noteOffset + (32 * noteZoom)) - 25;
		else
			l.background->y = (fuck[0]->y + noteOffset + (32 * noteZoom)) - 25;
		l.background->x = (lunder->x + lunder->w) + 25;
		l.text->x = l.background->x + 4;
		l.text->y = l.background->y + 2;
		if (l.background->y > 720 || l.background->y + l.background->h < 0)
			l.background->drawCall = false;
		else
			l.background->drawCall = true;
	}

	int waveInd = 0;
	for (waveformSeg& seg : waveform)
	{
		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		float noteOffset = Helpers::calculateCMODY(Game::save->GetDouble("scrollspeed") / 60, seg.time, currentTime, 64 * noteZoom);

		seg.sprite->alpha = waveformAlpha;
		if (!Game::save->GetBool("nonChange_chartWaveform"))
			seg.sprite->alpha = 0;
		seg.sprite->colorR = wavec.r;
		seg.sprite->colorG = wavec.g;
		seg.sprite->colorB = wavec.b;

		if (downscroll)
			seg.sprite->y = ((fuck[0]->y + (64 * noteZoom)) - 715) - (noteOffset);
		else
			seg.sprite->y = (fuck[0]->y) + noteOffset;
		seg.sprite->x = lunder->x;
		seg.sprite->w = lunder->w;
		seg.sprite->h = seg.length * noteZoom;

		if (waveformAlpha != 0)
		{
			if (seg.sprite->y > 720 || seg.sprite->y + seg.sprite->h < 0)
				seg.sprite->drawCall = false;
			else
				seg.sprite->drawCall = true;
		}
		else
			seg.sprite->drawCall = false;

		waveInd++;
	}


	if (songPlaying)
	{
		float songPos = song->getPos();

		currentTime = songPos;
		bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);

		currentBeat = FuckinEditor::selectedChart->getBeatFromTime(currentTime, curSeg);
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

			for (note nn : FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes)
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

			deleteNote(n.lane, n.beat);
			createNote(n.lane, currentBeat, noteType::Note_Head, -1, true);

			n.beat = currentBeat;
		}

	}
}
void fileMenu() {
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
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
				if (name.size() == 0)
					continue;
				if (ImGui::MenuItem(name.c_str()))
					openChart(file, file.substr(0, file.find_last_of("\\")));
				ind++;
			}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save"))
	{
		SMFile::SaveSM(FuckinEditor::selectedChart->meta, editor->currentFile);
	}
}

void deleteNotes()
{
	std::vector<NoteObject*> delNote;
	for (NoteObject* ne : notes)
	{
		if (!ne->selected)
			continue;
		delNote.push_back(ne);
	}

	for (NoteObject* obj : delNote)
	{
		note nn;
		nn.beat = obj->beat;
		nn.lane = obj->lane;
		nn.type = obj->type;
		deletedNotes.push_back(nn);
		deleteNote(obj->lane, obj->beat);
	}
	delNote.clear();
}

void copy()
{
	copiedNotes.clear();
	float firstBeat = -1;
	for (NoteObject* ne : notes)
	{
		if (!ne->selected)
			continue;
		note n;
		if (firstBeat == -1)
			firstBeat = ne->beat;
		n.beat = ne->beat - firstBeat;
		n.lane = ne->lane;
		n.type = ne->type;

		copiedNotes.push_back(n);
	}
	std::cout << "copied " << copiedNotes.size() << std::endl;
}

void paste()
{
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	if (copiedNotes.size() == 0)
		return;
	for (NoteObject* ne : notes)
	{
		ne->selected = false;
	}
	pastedNotes.clear();
	deletedNotes.clear();
	float startBeat = copiedNotes[0].beat + currentBeat;
	float endBeat = copiedNotes[copiedNotes.size() - 1].beat + currentBeat;
	for (NoteObject* n : notes)
	{
		if (n->beat >= startBeat && n->beat <= endBeat)
		{
			note nn;
			nn.beat = n->beat;
			nn.lane = n->lane;
			nn.type = n->type;
			deletedNotes.push_back(nn);
			deleteNote(n->lane, n->beat);
		}
	}
	for (note n : copiedNotes)
	{
		note copiedN = n;
		copiedN.beat = currentBeat + copiedN.beat;
		FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.push_back(copiedN);
		pastedNotes.push_back(copiedN);
		editor->generateNoteObject(copiedN, FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff], FuckinEditor::selectedChart, notes);
	}
	std::sort(notes.begin(), notes.end(), compareNoteByBeat);
	std::cout << "pasted " << pastedNotes.size() << " deleted " << deletedNotes.size() << std::endl;
}

void undo()
{
	FuckinEditor* editor = (FuckinEditor*)Game::currentMenu;
	if (pastedNotes.size() == 0 && deletedNotes.size() == 0)
		return;
	for (NoteObject* ne : notes)
	{
		ne->selected = false;
	}
	for (note n : pastedNotes)
	{
		deleteNote(n.lane, n.beat);
	}
	for (note n : deletedNotes)
	{
		note copiedN = n;
		copiedN.beat = copiedN.beat;
		FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff].notes.push_back(copiedN);
		editor->generateNoteObject(copiedN, FuckinEditor::selectedChart->meta.difficulties[FuckinEditor::currentDiff], FuckinEditor::selectedChart, notes);
	}
	std::sort(notes.begin(), notes.end(), compareNoteByBeat);
	std::cout << "deleted " << pastedNotes.size() << " pasted " << deletedNotes.size() << std::endl;

	pastedNotes.clear();
	deletedNotes.clear();
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
					if (FuckinEditor::selectedChart)
						FuckinEditor::selectedChart->destroy();
					openChart(filePathName, folder);
					currentFile = filePathName;
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
			if (FuckinEditor::selectedChart != NULL)
			{
				if (FuckinEditor::selectedChart->isModFile)
				{
					if (ImGui::MenuItem("Mod Viewer") && !openingFile) {
						currentTime = 0;
						currentBeat = 0;
						if (songPlaying)
						{
							song->stop();
							songPlaying = false;
						}

						notes.clear();
						sideStuff.clear();
						Game::instance->transitionToMenu(new ModEditor());
						return;
					}
					ImGui::Separator();
				}
			}
			fileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo") && !openingFile) {
				undo();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Copy") && !openingFile) {
				copy();
			}
			if (ImGui::MenuItem("Paste") && !openingFile) {
				paste();
			}
			if (ImGui::MenuItem("Delete") && !openingFile) {
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Select All") && !openingFile) {
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Mirror") && !openingFile) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Waveform") && !openingFile) {
				findWindow("Waveform Properties").shouldDraw = true;
			}

			bool shit = Game::save->GetBool("nonChange_beatLines");
			ImGui::Checkbox("Lines", &shit);
			if (shit != Game::save->GetBool("nonChange_beatLines"))
			{
				Game::save->SetBool("nonChange_beatLines", shit);
				Game::save->Save();
			}

			shit = Game::save->GetBool("nonChange_infoPanel");
			ImGui::Checkbox("Info Panel", &shit);
			if (shit != Game::save->GetBool("nonChange_infoPanel"))
			{
				Game::save->SetBool("nonChange_infoPanel", shit);
				Game::save->Save();
			}

			shit = Game::save->GetBool("nonChange_minimap");
			ImGui::Checkbox("Mini Map", &shit);
			if (shit != Game::save->GetBool("nonChange_minimap"))
			{
				Game::save->SetBool("nonChange_minimap", shit);
				Game::save->Save();
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Ticks"))
		{

			bool shit = Game::save->GetBool("nonChange_noteTick");
			ImGui::Checkbox("Note Tick", &shit);
			if (shit != Game::save->GetBool("nonChange_noteTick"))
			{
				Game::save->SetBool("nonChange_noteTick", shit);
				Game::save->Save();
			}

			shit = Game::save->GetBool("nonChange_beatTick");
			ImGui::Checkbox("Beat Tick", &shit);
			if (shit != Game::save->GetBool("nonChange_beatTick"))
			{
				Game::save->SetBool("nonChange_beatTick", shit);
				Game::save->Save();
			}
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
	if (FuckinEditor::selectedChart && Game::save->GetBool("nonChange_infoPanel"))
	{
		ImGui::Begin("BottomInfo", NULL, (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize));
		ImGui::SetWindowPos({ 15,660 });
		ImGui::SetWindowSize({ 350,50 });

		ImGui::Text("Time: %.2fs", currentTime / 1000);
		ImGui::SameLine();  ImGui::Text("Beat: %.2f", currentBeat);
		ImGui::SameLine();  ImGui::Text("BPM: %.2f", FuckinEditor::selectedChart->getSegmentFromTime(currentTime).bpm);
		ImGui::SameLine();  ImGui::Text("Snap: %i", snap);
		ImGui::End();
	}
}

bool controlHeld = false;

void FuckinEditor::keyUp(SDL_KeyboardEvent event)
{
	if (focused)
		return;
	note n;
	n.beat = -1;
	if (FuckinEditor::selectedChart)
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
			case SDLK_LCTRL:
				controlHeld = false;
				break;
		}
}

void FuckinEditor::keyDown(SDL_KeyboardEvent event)
{
	if (focused)
		return;
	if (event.keysym.sym == SDLK_ESCAPE)
	{
		if (FuckinEditor::selectedChart)
			FuckinEditor::selectedChart->destroy();
		Game::useImGUI = false;
		Game::instance->transitionToMenu(new MainMenu());
		return;
	}

	if (event.keysym.sym == SDLK_F1)
	{
		findWindow("Debug Window").shouldDraw = !findWindow("Debug Window").shouldDraw;

	}

	if (event.keysym.sym == SDLK_SPACE && FuckinEditor::selectedChart)
	{
		if (songPlaying)
		{
			song->stop();

			beatsClapped.clear();
			for (Channel* c : clapChannels)
			{
				c->stop();
				c->free();
			}
			clappedLines.clear();
			clapChannels.clear();
		}
		else
		{
			if (currentTime < 0)
				currentTime = 0;
			song->setPos(currentTime);
			song->play();
		}

		songPlaying = !songPlaying;
	}

	if (event.keysym.sym == SDLK_UP && FuckinEditor::selectedChart)
	{
		currentTime -= 100;
		bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);
		currentBeat = FuckinEditor::selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}

	if (event.keysym.sym == SDLK_DOWN && FuckinEditor::selectedChart)
	{
		currentTime += 100;
		bpmSegment curSeg = FuckinEditor::selectedChart->getSegmentFromTime(currentTime);
		currentBeat = FuckinEditor::selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}

	if (event.keysym.sym == SDLK_LEFT && (event.keysym.mod & KMOD_SHIFT))
	{
		speed -= 0.1;
		if (speed < 0.1)
			speed = 0.1;
		song->rateChange(speed);
	}
	else if (event.keysym.sym == SDLK_RIGHT && (event.keysym.mod & KMOD_SHIFT))
	{
		speed += 0.1;
		if (speed > 3)
			speed = 3;
		song->rateChange(speed);
	}
	else
	{
		if (event.keysym.sym == SDLK_LEFT)
		{
			snapSelect--;
			if (snapSelect < 0)
				snapSelect = snapConvert.size() - 1;
		}
		if (event.keysym.sym == SDLK_RIGHT)
		{
			snapSelect++;
			if (snapSelect > snapConvert.size() - 1)
				snapSelect = 0;
		}
	}

	if (event.keysym.sym == SDLK_DELETE)
	{
		deleteNotes();
	}

	if (event.keysym.sym == SDLK_c && (event.keysym.mod & KMOD_CTRL))
	{
		copy();
	}

	if (event.keysym.sym == SDLK_z && (event.keysym.mod & KMOD_CTRL))
	{
		undo();
	}

	if (event.keysym.sym == SDLK_v && (event.keysym.mod & KMOD_CTRL))
	{
		paste();
	}

	auto it = snapConvert.begin();
	std::advance(it, snapSelect);
	snap = it->first;


	if (FuckinEditor::selectedChart)
	{
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
		case SDLK_LCTRL:
			controlHeld = true;
			break;
		}
		if (currentTime < FuckinEditor::selectedChart->meta.chartOffset)
			currentTime = FuckinEditor::selectedChart->meta.chartOffset;
	}
}


void FuckinEditor::mouseWheel(float wheel)
{
	if (focused)
		return;
	if (!FuckinEditor::selectedChart)
		return;

	beatsClapped.clear();
	for (Channel* c : clapChannels)
	{
		c->stop();
		c->free();
	}
	clapChannels.clear();

	float amount = wheel;
	if (controlHeld)
	{
		if (amount < 0)
			noteZoom -= 0.01;
		else
			noteZoom += 0.01;

		if (noteZoom < 0.2)
			noteZoom = 0.2;
		else if (noteZoom > 2)
			noteZoom = 2;


		Rect laneUnderway;

		laneUnderway.x = ((Game::gameWidth / 2) - (((64 * noteZoom) + 12) * 2)) - 4;
		laneUnderway.y = -200;
		laneUnderway.w = (((Game::gameWidth / 2) - (((64 * noteZoom) + 12) * 2)) + (((64 * noteZoom) + 12) * 3) - laneUnderway.x) + ((68 * noteZoom) + 12);
		laneUnderway.h = 1280;

		lunder->x = laneUnderway.x;
		lunder->y = laneUnderway.y;
		lunder->w = laneUnderway.w;
		lunder->h = laneUnderway.h;

		lunderBorder->x = laneUnderway.x;
		lunderBorder->y = laneUnderway.y;
		lunderBorder->w = laneUnderway.w;
		lunderBorder->h = laneUnderway.h;

		for (int i = 0; i < 4; i++)
		{
			ReceptorObject* obj = fuck[i];
			float x = (laneUnderway.x + 12) + ((((64 * noteZoom) + 12)) * i);
			obj->x = x;
			obj->w = 64 * noteZoom;
			obj->h = 64 * noteZoom;
		}

		for (NoteObject* obj : notes)
		{
			obj->size = noteZoom;
			obj->rect.w = 64 * noteZoom;
			obj->rect.h = 64 * noteZoom;
			obj->fboX = fuck[obj->lane]->x;
		}
		return;
	}

	float increase = 0.0f;
	float beats = 0.0f;
	increase = 1.0f / static_cast<float>(snapConvert[snap]);
	
	beats = beatMath(amount, increase, currentBeat);

	if (beats < 0.00)
		beats = 0.00;

	bpmSegment seg = FuckinEditor::selectedChart->getSegmentFromBeat(beats);

	float stopOffset = (FuckinEditor::selectedChart->getStopOffsetFromBeat(beats) / 1000.0f) * (seg.bpm / 60.0f);

	float offsetBeats = beatMath(amount, stopOffset, beats);

	if (stopOffset != 0)
		beats = offsetBeats;

	std::cout << "setting beat to " << beats << " with increase/decrease of " << increase << " from beat " << currentBeat << std::endl;
	currentBeat = beats;
	float time = FuckinEditor::selectedChart->getTimeFromBeat(beats, seg);
	song->setPos(time);
	currentTime = song->getPos();
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
	for (line l : miniMapLines)
	{
		miniMap->removeObj(l.rect);
	}
	miniMapLines.clear();
	for (NoteObject* obj : notes)
	{
		gameplay->removeObj(obj);
	}

	for (line obj : snapBeat)
	{
		gameplay->removeObj(obj.rect);
	}

	for (line obj : beatLines)
	{
		gameplay->removeObj(obj.rect);
		gameplay->removeObj(obj.text);
	}

	for (thingy obj : sideStuff)
	{
		gameplay->removeObj(obj.background);
		gameplay->removeObj(obj.text);
	}

	notes.clear();
	sideStuff.clear();
	bool downscroll = false;
	generateSnapLines(FuckinEditor::selectedChart, snapConvert[snap]);
	regenBeatLines(FuckinEditor::selectedChart);
	regenThings(FuckinEditor::selectedChart);
	for (note n : diff.notes)
	{
		generateNoteObject(n, diff, FuckinEditor::selectedChart, notes);
	}
}

void FuckinEditor::generateWaveForm(int start, int end)
{
	for (waveformSeg seg : waveform)
	{
		wave->removeObj(seg.sprite);
	}

	waveform.clear();

	float rate = song->returnSampleRate();
	float leng = 0;
	float* samples = song->returnSongSample(&leng);
	int lastInterp = 0;
	float lastFreqR = 0;
	float savedInd = 0;
	float nextFreq = 0;
	float lastNext = 0;
	int nextIndex = 0;

	int plength = getWaveLength(1, FuckinEditor::selectedChart);
	float pixels = 0;

	AvgGroup* waveSeg = new AvgGroup(0, 0, lunder->w, 715);
	waveSeg->renderOnce = true;
	float currentSegTime = start;
	float offset = 0;
	float lastY = 0;
	double startTime = start / 1000;
	int indexx = 0;
	bool startNext = false;
	for (int i = start; i < end; i++)
	{
		int index = BASS_ChannelSeconds2Bytes(song->id, std::max(0.0,(double)i / 1000.0)) / 2;

		if (index > leng)
			break;

		double time = (double)i / 1000.0;

		float rightFreq = samples[index + 1] * 128;
		rightFreq = abs(rightFreq);

		float leftFreq = samples[index] * 128;
		leftFreq = abs(leftFreq);

		if (rightFreq != leftFreq)
			rightFreq = leftFreq;

		float x = (lunder->w / 2);

		float w = (rightFreq * 2);

		if (rightFreq == 0)
			continue;


		if (w / 2 + (lunder->w / 2) > lunder->w)
			w = (lunder->w - (lunder->w / 2)) * 2;

		float noteOffTime = i - (startTime * 1000);
		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		float noteOffset = Helpers::calculateCMODY(bps, noteOffTime, 0, 64 * noteZoom);

		float yy = noteOffset;
		if (downscroll)
			yy = 715 - noteOffset;

		AvgRect* rect = new AvgRect(x - rightFreq, yy, w, 2);
		rect->c = { 255,255,255 };
		line l;
		l.freqR = rightFreq;
		l.rect = rect;
		l.time = i;
		pixels += 2;
		//l.text = new Text(rect->x - 10, rect->y, std::to_string((int)snappedBeat), 16, "Futura Bold");
		waveSeg->add(rect);
		if (pixels >= 715)
		{
			indexx++;
			waveformSeg wSeg;
			wSeg.time = startTime * 1000;
			startTime = time;
			waveSeg->forceDraw();
			wSeg.sprite = new AvgSprite(0, 0, waveSeg->ctb);
			wSeg.sprite->flip = true;
			wSeg.offset = offset;
			wave->add(wSeg.sprite);
			wSeg.length = 715;
			waveform.push_back(wSeg);
			delete waveSeg;
			pixels = 0;
			waveSeg = new AvgGroup(0, 0, lunder->w, 715);
			waveSeg->renderOnce = true;
			currentSegTime = i;
		}
		//gameplay->add(l.text);
	}
	if (waveSeg)
		delete waveSeg;
	delete samples;
}
