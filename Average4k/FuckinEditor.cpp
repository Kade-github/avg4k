#include "FuckinEditor.h"
#include "ImGuiFileDialog.h"
#include "MainMenu.h"
#include "Judge.h"
// window draws

Chart* selectedChart;
float currentTime;
float currentBeat;

Channel* song;

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
					double d = seg.bpm;
					ImGui::Text("BPM:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##BPM" + std::to_string(ind)).c_str(), &d, 1, 10);
					if (d < 1)
						d = 1;
					double beat = seg.startBeat;
					ImGui::Text("Beat:");
					ImGui::PushItemWidth(100);
					ImGui::InputDouble(("##Beat" + std::to_string(ind)).c_str(), &beat, 1, 10);
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

void FuckinEditor::create()
{
	addCamera(Game::mainCamera);

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

	for (int i = 0; i < 4; i++)
	{
		float x = (lunder->x + 12) + (((64 * Game::save->GetDouble("Note Size") + 12)) * i);

		AvgSprite* r = new AvgSprite(x, 65, Game::noteskin->receptor);
		add(r);
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

	createWindow("Chart Properties", { 400,400 }, (drawCall)window_chartProperties, false);
	createWindow("Help", { 550,350 }, (drawCall)window_help, false);
	findWindow("Help").xOff = 10;
	created = true;
}

void FuckinEditor::update(Events::updateEvent event)
{
	for (NoteObject* obj : notes)
	{
		obj->rTime = currentTime;
	}

	if (songPlaying && selectedChart)
	{
		float songPos = song->getPos();

		if (std::abs(((currentTime) - songPos) >= 0.05))
			currentTime = songPos;
		else
			currentTime += Game::deltaTime;

		bpmSegment curSeg = selectedChart->getSegmentFromTime(currentTime);
		currentBeat = selectedChart->getBeatFromTimeOffset(currentTime, curSeg);
	}
}

bool openingFile = false;

void openChart(std::string path, std::string folder) {
	FuckinEditor* editor = (FuckinEditor*)Game::instance->currentMenu;
	SMFile* file = new SMFile(path, folder, true);
	selectedChart = new Chart(file->meta);
	delete file;
	std::string pathj = selectedChart->meta.folder + "/" + selectedChart->meta.audio;

	song = SoundManager::createChannel(pathj.c_str(), "editorSong");

	song->createFXStream();

	editor->loadNotes(selectedChart->meta.difficulties[0]);
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
		}
		ImGui::End();
	}

	if (openingFile)
	{
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
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
	ImGui::Begin("BottomInfo", NULL, (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize));
	ImGui::SetWindowPos({ 15,660 });
	ImGui::SetWindowSize({ 265,50 });
	ImGui::Text(("Time: " + std::to_string((double)(currentTime / 1000)) + "s | Beat: " + std::to_string(currentBeat)).c_str());
	ImGui::End();
}

void FuckinEditor::keyDown(SDL_KeyboardEvent event)
{
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
			song->play();
			song->setPos(currentTime);
		}
		songPlaying = !songPlaying;
	}

	if (event.keysym.sym == SDLK_UP && selectedChart)
	{
		currentTime -= 100;
	}

	if (event.keysym.sym == SDLK_DOWN && selectedChart)
	{
		currentTime += 100;
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
	for (NoteObject* obj : notes)
	{
		removeObj(obj);
		delete obj;
	}
	notes.clear();
	bool downscroll = false;
	for (note n : diff.notes)
	{
		NoteObject* object = new NoteObject();
		object->fboMode = false;
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
		object->connectedReceptor = fuck[n.lane];
		object->type = n.type;
		object->endTime = -1;
		object->endBeat = -1;

		bpmSegment noteSeg = selectedChart->getSegmentFromBeat(object->beat);

		object->time = selectedChart->getTimeFromBeatOffset(object->beat, noteSeg);
		rect.y = Game::gameHeight + 400;
		rect.x = 0;
		rect.w = 64 * Game::save->GetDouble("Note Size");
		rect.h = 64 * Game::save->GetDouble("Note Size");
		object->rect = rect;

		note tail;

		bpmSegment bruh = selectedChart->getSegmentFromBeat(object->beat);

		float wh = selectedChart->getTimeFromBeat(n.beat, bruh);

		float bps = (Game::save->GetDouble("scrollspeed") / 60);


		if (object->type == Note_Head)
		{
			for (int i = 0; i < diff.notes.size(); i++)
			{
				note& nn = diff.notes[i];
				if (nn.type != Note_Tail)
					continue;
				if (nn.lane != object->lane)
					continue;

				bpmSegment npreStopSeg = selectedChart->getSegmentFromBeat(nn.beat);

				float nstopOffset = selectedChart->getStopOffsetFromBeat(nn.beat);

				double nstopBeatOffset = (nstopOffset / 1000) * (npreStopSeg.bpm / 60);

				object->endBeat = nn.beat + nstopBeatOffset;

				object->endTime = selectedChart->getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
				tail = nn;
				break;
			}
		}

		float time = SDL_GetTicks();

		if (object->type == Note_Head)
		{

			for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
			{
				bpmSegment holdSeg = selectedChart->getSegmentFromTime(i);

				double beat = selectedChart->getBeatFromTimeOffset(i, holdSeg);

				float whHold = selectedChart->getTimeFromBeatOffset(beat, holdSeg);

				float diff = whHold - (object->time);

				float noteOffset = (bps * (diff / 1000)) * (64 * Game::save->GetDouble("Note Size"));

				float y = 0;
				float yDiff = 0;
				if (object->heldTilings.size() != 0)
				{
					if (downscroll)
						y = object->rect.y - noteOffset;
					else
						y = object->rect.y + noteOffset;
					yDiff = y - object->heldTilings.back().rect.y;
				}
				else
				{
					if (downscroll)
						y = object->rect.y - noteOffset;
					else
						y = object->rect.y + noteOffset;
					yDiff = y - object->rect.y;
				}

				bool otherOne = false;

				if (downscroll)
					otherOne = yDiff <= -(64 * Game::save->GetDouble("Note Size"));
				else
					otherOne = yDiff >= 64 * Game::save->GetDouble("Note Size");

				if (otherOne || object->heldTilings.size() == 0)
				{
					object->holdHeight += 64 * Game::save->GetDouble("Note Size");
					holdTile tile;
					SDL_FRect rect;
					tile.active = true;
					tile.fucked = false;
					rect.y = y;
					rect.x = 0;
					rect.w = 64 * Game::save->GetDouble("Note Size");
					rect.h = 68 * Game::save->GetDouble("Note Size");
					tile.rect = rect;
					tile.beat = beat;
					tile.time = i;
					object->heldTilings.push_back(tile);
				}
			}
		}
		std::sort(object->heldTilings.begin(), object->heldTilings.end());
		notes.push_back(object);
		object->create();
		add(object);
	}
}
