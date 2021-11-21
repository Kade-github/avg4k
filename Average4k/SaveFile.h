#pragma once
#include <iostream>
#include <fstream>
#include "msgpack.hpp"
#include "includes.h"

struct setting {
	bool active;
	double value;
	char name[128];

	MSGPACK_DEFINE(active, value, name);
};

class SaveFile
{
	public:
		SaveFile();

		void CreateNewFile();

		void Save();

		void SetDouble(std::string setting, double value);
		void SetBool(std::string setting, bool value);

		double GetDouble(std::string setting);
		bool GetBool(std::string setting);

		setting CreateSetting(bool defaultActive, double defaultValue, std::string defaultName);

		std::vector<setting> settings;
};

