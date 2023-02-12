#pragma once
#include "includes.h"
namespace Average4k::External
{
	struct ConfigValue
	{
		std::string name;
		std::string value;

		bool operator==(const ConfigValue& other) {
			return name == other.name;
		}
	};

	class ConfigReader
	{
	public:
		std::vector<ConfigValue> values{};
		bool good = false;
		ConfigReader() = default;
		ConfigReader(std::string path)
		{
			std::ifstream f;
			f.open(path);
			good = f.good();
			using namespace AvgEngine;
			if (!f.good())
			{
				Logging::writeLog("[Config] Failed to read " + path);
				return;
			}

			std::string line;

			using namespace Utils;
			while (std::getline(f, line)) {
				if (!StringTools::Contains(line, ":"))
					continue;
				std::vector<std::string> s = StringTools::Split(line, ":");
				if (s.size() != 2)
					continue;
				ConfigValue v;

				v.name = StringTools::Trim(s[0]);
				v.value = StringTools::Trim(s[1]);
				values.push_back(v);
			}
			f.close();

			#ifdef _DEBUG
			Logging::writeLog("[Config] [Debug] Loaded " + path + " with " + std::to_string(values.size()) + " values.");
			#endif
		}

		bool Contains(std::string name)
		{
			for (ConfigValue& v : values)
				if (v.name == name)
					return true;
			return false;
		}

		float Float(std::string name)
		{
			if (!Contains(name))
				return 0.0f;
			for (ConfigValue v : values)
				if (v.name == name)
					return std::stof(v.value);
			return 0.0f;
		}

		int Int(std::string name)
		{
			if (!Contains(name))
				return 0;
			for (ConfigValue v : values)
				if (v.name == name)
					return std::stoi(v.value);
			return 0;
		}

		bool Bool(std::string name)
		{
			if (!Contains(name))
				return false;
			for (ConfigValue v : values)
				if (v.name == name)
					return v.value == "true";
			return false;
		}

		std::string Value(std::string name)
		{
			if (!Contains(name))
				return "";
			for (ConfigValue v : values)
				if (v.name == name)
					return v.value;
			return "";
		}
	};
}