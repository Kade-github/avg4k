#pragma once
#include <chrono>
#include <iostream>
#include <fstream>

class Logging {
public:
	static std::ofstream log;

	static void openLog()
	{
		log = std::ofstream("log.txt");
	}

	static void writeLog(std::string l)
	{
		auto start = std::chrono::system_clock::now();
		auto legacyStart = std::chrono::system_clock::to_time_t(start);
		char tmBuff[30];
		ctime_s(tmBuff, sizeof(tmBuff), &legacyStart);

		std::string date = std::string(tmBuff);

		// substr -1 because it appends a \n. it just hates me man

		log << "[" + date.substr(0,date.length() - 1) + "] " + l + "\n";
	}

	static void closeLog()
	{
		log.close();
	}
};