#pragma once
#include "VerticesTest.h"
#include "Game.h"
#include "imgui.h"

std::vector<GL_Vertex> vertices;

void VerticesTest::create()
{
	Game::useImGUI = true;
}




void VerticesTest::imguiUpdate(float elapsed)
{
	ImGui::Begin("Vertices Test");
	{
		if (ImGui::Button("Create Vertex"))
		{
			vertices.push_back({ 0,0,0,0,255,255,255,1 });
		}
		int ind = 0;
		int toDelete = -1;

		for (GL_Vertex& vert : vertices)
		{
			if (ImGui::CollapsingHeader(("Vertex numba " + std::to_string(ind)).c_str()))
			{
				ImGui::Text("Delete");
				if (ImGui::Button("Delete Vertex ##Delete" + ind))
				{
					toDelete = ind;
				}
				ImGui::Text("X");
				ImGui::InputFloat(("##VertexX" + std::to_string(ind)).c_str(), &vert.x);
				ImGui::Text("Y");
				ImGui::InputFloat(("##VertexY" + std::to_string(ind)).c_str(), &vert.y);
				ImGui::Text("U");
				ImGui::InputFloat(("##VertexU" + std::to_string(ind)).c_str(), &vert.u);
				ImGui::Text("V");
				ImGui::InputFloat(("##VertexV" + std::to_string(ind)).c_str(), &vert.v);
			}
			ind++;
		}

		if (toDelete != -1)
			vertices.erase(vertices.begin() + toDelete);
	}
	ImGui::End();
}

void VerticesTest::update(Events::updateEvent event)
{
	Rendering::PushQuad(vertices, NULL, GL::genShader);
	Rendering::drawBatch();
}
