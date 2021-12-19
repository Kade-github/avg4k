#pragma once
#include <iostream>
#include <fstream>
#include "msgpack.hpp"
#include "includes.h"

struct setting {
	bool active;
	double value;
	double lowestValue;
	double highestValue;
	char name[128];
	bool takesActive;

	MSGPACK_DEFINE(active, value, name, takesActive);
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

		setting CreateSetting(bool defaultActive, double defaultValue, std::string defaultName, bool tA, double lowest, double highest);

		std::vector<setting> settings;
		std::vector<setting> defaultSettings;
};

