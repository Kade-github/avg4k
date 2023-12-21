/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#pragma once

#include <AvgEngine/EventManager.h>
#include <AvgEngine/Debug/ConsoleCommandHandler.h>

namespace AvgEngine::Debug
{
	class Console
	{
	public:
		bool open = false;

		float fpsData[50];
		int drawData[50];

		static Console* instance;

		ConsoleCommandHandler* handler;

		bool showPerformance = false;

		char input[256];

		void registerEvents(Events::EventManager& e);

		void update();
	};
}

#endif // !CONSOLE_H