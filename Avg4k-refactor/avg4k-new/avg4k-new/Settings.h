#pragma once
#include "includes.h"

namespace Average4k::Settings
{
	enum SettingType
	{
		S_Int = 0,
		S_String = 1,
		S_Float = 2,
		S_Bool = 3
	};

	struct Setting {
		std::string name = "";
		std::string value;
		std::string defaultSet = "";
		SettingType type = static_cast<SettingType>(0);

		int min = 0;
		int max = 0;

		bool operator==(const Setting& other) {
			return name == other.name;
		}

		MSGPACK_DEFINE(value, type, min, max);
	};

	struct File
	{
		std::string settingsVersion = "n/a";

		std::vector<Setting> settings{};

		MSGPACK_DEFINE(settingsVersion, settings);
	};

	class Settings
	{
		// Average Engine 1
		std::string settingsVersion = "av.e.1";
	public:
		File f;

		File GetDefaults()
		{
			File nf;
			nf.settingsVersion = settingsVersion;

			nf.settings.push_back({ "Music Volume", "0.45", "0.45", S_Float, 0,1 });
			nf.settings.push_back({ "Hitsound Volume", "0.8",  "0.8", S_Float, 0,1 });
			nf.settings.push_back({ "Scrollspeed", "800",  "800", S_Int, 200,1800 });
			nf.settings.push_back({ "Hitsound Volume", "0.8",  "0.8", S_Float, 0,1 });
		}

		void SetDefaults()
		{
			f = GetDefaults();
		}

		void Validate()
		{
			bool checkForNew = f.settingsVersion != settingsVersion;

			File def = GetDefaults();

			int index = 0;

			for (Setting& ns : f.settings)
			{
				if (checkForNew)
				{
					if (index < def.settings.size())
					{
						Setting tD = def.settings[index];
						if (tD != ns || tD.defaultSet != ns.defaultSet)
						{
							ns = tD;
							AvgEngine::Logging::writeLog("[Settings] " + ns.name + " is old.");
						}
					}
				}

				if (ns.type == S_Int || ns.type == S_Float)
				{
					if (!AvgEngine::Utils::StringTools::isNumber(ns.value))
					{
						ns.value = ns.defaultSet;
						AvgEngine::Logging::writeLog("[Settings] Reverting " + ns.name + " due to it not being a number.");
					}
				}

				if (ns.type == S_Bool && (ns.value != "false" && ns.value != "true"))
				{
					ns.value = ns.defaultSet;
					AvgEngine::Logging::writeLog("[Settings] Reverting " + ns.name + " due to it not being a bool.");
				}
				index++;
			}

			if (checkForNew)
			{
				if (f.settings.size() > def.settings.size())
					f.settings.resize(index);
				else if (f.settings.size() < def.settings.size())
				{
					for(int i = f.settings.size(); i < def.settings.size(); i++)
					{
						f.settings.push_back(def.settings[i]);
					}
				}
			}
		}

		void Set(Setting s)
		{
			for(Setting& ns : f.settings)
				if (ns == s)
				{
					switch(ns.type)
					{
					case S_Float:
					case S_Int:
						if (!AvgEngine::Utils::StringTools::isNumber(ns.value))
						{
							AvgEngine::Logging::writeLog("[Settings] Cannot set " + s.name + " because you're trying to set it to a non-number.");
							return;
						}
						// end up converting it to an int since min/max are ints lol
						int v = std::stoi(ns.value);
						break;
					case S_String:
						break;
					case S_Bool:
						break;
					}
				}
		}

		Setting Get(std::string name)
		{
			for (Setting& ns : f.settings)
				if (ns.name == name)
					return ns;
			return {};
		}

		Settings(std::string file)
		{
			std::ifstream fl;
			fl.open(file);
			if (fl.bad())
			{
				AvgEngine::Logging::writeLog("[Settings] Failure to read " + file);
				SetDefaults();
				return;
			}
		}
	};
}