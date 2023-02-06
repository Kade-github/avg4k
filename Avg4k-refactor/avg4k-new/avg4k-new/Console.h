#pragma once
#include "includes.h"
#include "imgui.h"
#include "EventManager.h"
namespace AvgEngine::Debug
{
	class Console
	{
	public:
		bool open = false;

		float fpsData[50];
		int drawData[50];


		bool showPerformance = false;

		char input[256];

		void registerEvents(Events::EventManager& e);

		void update();
	};
}