/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <AvgEngine/Utils/Logging.h>

// obligatory out of class definition (or if you don't do this, this is what happens https://imgur.com/a/togqxR6)

std::ofstream AvgEngine::Logging::log;
std::vector<AvgEngine::ConsoleLog> AvgEngine::Logging::consoleLog = {};