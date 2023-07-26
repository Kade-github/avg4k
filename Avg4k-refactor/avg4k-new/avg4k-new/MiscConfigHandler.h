#pragma once
#include "includes.h"
#include "ConfigReader.h"
namespace Average4k {
	class MiscConfigHandler {
		std::string _savePath = "";
	public:
		External::ConfigReader* reader;
		std::string getValue(std::string key)
		{
			return reader->Value(key);
		}

		void setValue(std::string key, std::string value)
		{
			for (External::ConfigValue& v : reader->values)
			{
				if (v.name == key)
				{
					v.value = value;
					reader->WriteToFile(_savePath);
					break;
				}
			}
		}

		MiscConfigHandler(External::ConfigReader* c, std::string savePath)
		{
			reader = c;
			_savePath = savePath;
		}
	};
}