#pragma once
#include "includes.h"

struct setting {
	bool active;
	double value;
	double lowestValue;
	double highestValue;
	char name[128];
	bool takesActive;
	bool isKeybind;
	double increm;
	char stringValue[248];
	MSGPACK_DEFINE(active, value, name, takesActive, lowestValue, highestValue, isKeybind, increm, stringValue);
};

class SaveFile
{
	public:
		SaveFile();

		void CreateNewFile();

		void Save();


		void SetString(std::string setting, std::string value);
		void SetDouble(std::string setting, double value);
		void SetBool(std::string setting, bool value);

		std::string GetString(std::string setting);
		double GetDouble(std::string setting);
		bool GetBool(std::string setting);

		setting CreateSetting(bool defaultActive, double defaultValue, std::string defaultName, bool tA, double lowest, double highest, bool isKeybind, double increm, std::string defaultStringValue);

		std::vector<setting> settings;
		std::vector<setting> defaultSettings;
};

