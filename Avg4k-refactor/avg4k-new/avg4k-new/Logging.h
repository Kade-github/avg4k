#pragma once
#include <chrono>
#include <iostream>
#include <fstream>
#include "StringTools.h"
#include "imgui.h"

namespace AvgEngine
{
	struct ConsoleLog
	{
		ImColor color;
		std::string text;
	};
	class Logging {
	public:
		static std::ofstream log;

		static std::vector<ConsoleLog> consoleLog;

		/**
		 * \brief Create the log file
		 */
		static void openLog()
		{
			log = std::ofstream("log.txt");
		}

		/**
		 * \brief Write to the log file
		 * \param l The log
		 */
		static void writeLog(std::string l)
		{
			auto start = std::chrono::system_clock::now();
			auto legacyStart = std::chrono::system_clock::to_time_t(start);
			char tmBuff[30];
			ctime_s(tmBuff, sizeof(tmBuff), &legacyStart);

			std::string date = std::string(tmBuff);

			std::string logs = "[" + date.substr(0, date.length() - 1) + "] " + l;

			// substr -1 because it appends a \n. it just hates me man
#ifdef _DEBUG
			std::cout << log << "\n";
#else
			log << logs << "\n";
#endif

			int type = 0;

			if (Utils::StringTools::Contains(logs, "[User]"))
				type = 3;
			else if (Utils::StringTools::Contains(logs, "[Error]"))
				type = 2;
			else if (Utils::StringTools::Contains(logs, "[Warning]"))
				type = 1;

			ConsoleLog cLog;
			cLog.text = logs;

			switch (type)
			{
			default: // normal
				cLog.color = { ImColor(255,255,255,255) };
				break;
			case 1: // warning
				cLog.color = { ImColor(245, 218, 86, 255) };
				break;
			case 2: // error
				cLog.color = { ImColor(245, 86, 86, 255) };
				break;
			case 3: // other
				cLog.color = { ImColor(95, 141, 250, 255) };
				break;
			}
			consoleLog.push_back(cLog);

		}

		/**
		 * \brief Save the log file
		 */
		static void closeLog()
		{
			log.close();
		}
	};
}
