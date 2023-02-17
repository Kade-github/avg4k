#pragma once
#include "includes.h"

#include <msgpack.hpp>

namespace Average4k
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
		int type = 0;

		int min = 0;
		int max = 0;

		bool operator==(const Setting& other) {
			return name == other.name;
		}

		MSGPACK_DEFINE(name, value, type, min, max);
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
		std::string _settingsVersion = "Average4KSettingsFile:ver-av.e.1";
		std::string _path;
	public:
		File f;

		File GetDefaults()
		{
			File nf;
			nf.settingsVersion = _settingsVersion;

			nf.settings.push_back({ "Music Volume", "0.45", "0.45", S_Float, 0,1 });
			nf.settings.push_back({ "Hitsound Volume", "0.8",  "0.8", S_Float, 0,1 });
			nf.settings.push_back({ "Scrollspeed", "800",  "800", S_Int, 200,1800 });
			nf.settings.push_back({ "Skin", "arrow",  "arrow", S_String, -1,-1 });
			nf.settings.push_back({ "Note Size", "1",  "1", S_Float, 0,2 });
			nf.settings.push_back({ "Resolution", "1280x720",  "1280x720", S_String, -1,-1 });
			return nf;
		}

		void SetDefaults()
		{
			f = GetDefaults();
		}

		void Save()
		{
			std::ofstream st;
			st.open(_path);
			
			std::stringstream s;

			msgpack::pack(s, f);

			st << s.str();
			st.close();
			#ifdef _DEBUG 
			AvgEngine::Logging::writeLog("[Settings] [Debug] Saved " + _path + " successfully.");
			#endif
		}

		void Validate()
		{
			File def = GetDefaults();

			bool checkForNew = (f.settingsVersion != _settingsVersion) || (def.settings.size() != f.settings.size());
			int index = 0;

			for (Setting& ns : f.settings)
			{
				if (checkForNew)
				{
					if (index < def.settings.size())
					{
						Setting tD = def.settings[index];
						if (tD.name != ns.name || tD.defaultSet != ns.defaultSet)
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
			#ifdef _DEBUG
			AvgEngine::Logging::writeLog("[Settings] [Debug] Validated " + _path + " successfully. " + (checkForNew ? "Updated old version." : "Version didn't need updating."));
			#endif

			Save();
		}


		Setting& Get(std::string name)
		{
			for (Setting& ns : f.settings)
				if (ns.name == name)
					return ns;
			Setting s;
			return s;
		}

		Settings()
		{
			SetDefaults();
			_path = "settings.ave";
		}

		Settings(std::string file)
		{
			_path = file;
			std::ifstream fl;
			fl.open(file);
			if (!fl.good())
			{
				AvgEngine::Logging::writeLog("[Settings] Failure to read " + file);
				SetDefaults();
				Save();
				return;
			}

			std::stringstream buffer;
			buffer << fl.rdbuf();

			fl.close();

			msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
			upd.get().convert(f);

			Validate();
		}
	};
}