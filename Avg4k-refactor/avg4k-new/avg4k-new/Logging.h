#pragma once
#include <chrono>
#include <iostream>
#include <fstream>

namespace AvgEngine
{

	class Logging {
	public:
		static std::ofstream log;

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

			// substr -1 because it appends a \n. it just hates me man
#ifdef _DEBUG
			std::cout << "[" + date.substr(0, date.length() - 1) + "] " + l + "\n";
#else
			log << "[" + date.substr(0, date.length() - 1) + "] " + l + "\n";
#endif

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