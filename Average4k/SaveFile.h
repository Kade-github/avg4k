#pragma once
#include "includes.h"

struct settingConstruct {
	bool takesActive = false;
	bool takesString = false;
	bool takesDouble = false;

	bool defaultActive = false;
	std::string defaultString = "";
	double defaultDouble = 0;
	double defaultMin = 0;
	double defaultMax = 0;
	double defaultIncrm = 1;

	// other special stuff
	bool unique = false;
	std::string settingSuffix = "";
	bool isDropdown = false;
};

struct setting {
	bool takesActive;
	bool takesString;
	bool takesDouble;

	bool defaultActive;
	std::string defaultString;
	double defaultDouble;
	double defaultMin;
	double defaultMax;
	double defaultIncrm;

	std::string name;

	// other special stuff
	bool unique;
	std::string settingSuffix;
	bool isDropdown;

	MSGPACK_DEFINE(takesActive, takesString, takesDouble, defaultActive, defaultString, defaultDouble, defaultMin, defaultMax, defaultIncrm, name, unique, settingSuffix, isDropdown);
};

struct settingHeader {
	std::string settingsVersion;

	std::vector<setting> settings;

	MSGPACK_DEFINE(settingsVersion, settings);
};

class SaveFile
{
	public:
		settingHeader currentHeader;

		SaveFile();

		void Save();


		void SetString(std::string setting, std::string value);
		void SetDouble(std::string setting, double value);
		void SetBool(std::string setting, bool value);

		setting& getSetting(std::string setting);

		static std::vector<std::string> ObtainDropDownSettingList(std::string settting);

		std::string GetString(std::string setting);
		double GetDouble(std::string setting);
		bool GetBool(std::string setting);

		setting CreateSetting(std::string defaultName, settingConstruct cons);

		std::vector<setting> defaultSettings;
};

