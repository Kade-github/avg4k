/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef IMGUIHELPER_H
#define IMGUIHELPER_H

#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/implot.h>
#include <GLFW/glfw3.h>

namespace AvgEngine::External
{
	class ImGuiHelper
	{
	public:
		static ImGuiIO io;

		static void Init(GLFWwindow* window)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImPlot::CreateContext();
			io = ImGui::GetIO();
			io.IniFilename = NULL;
			
			io.ConfigWindowsResizeFromEdges = false;
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 150");
		}

		static void Destroy()
		{
			ImPlot::DestroyContext();
			ImGui::DestroyContext();
		}

		static void RenderStart()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void RenderEnd(GLFWwindow* window)
		{
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	};
}

#endif // !IMGUIHELPER_H