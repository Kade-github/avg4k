#pragma once
#include "includes.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
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
			io = ImGui::GetIO(); (void)io;
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