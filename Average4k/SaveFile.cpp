#include "SaveFile.h"


SaveFile::SaveFile()
{
    std::ifstream ifs("settings.pack");
    if (!ifs.good())
    {
        CreateNewFile();
        return;
    }
   
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
    upd.get().convert(settings);
}

void SaveFile::CreateNewFile()
{
	// default settings
    settings.push_back(CreateSetting(false,0,"downscroll"));
    settings.push_back(CreateSetting(true,2.4,"scrollspeed"));
    
    Save();
}

void SaveFile::Save()
{
    std::ofstream of("settings.pack");

    std::stringstream bitch;

    msgpack::pack(bitch, settings);

    of << bitch.str();

    of.close();
}

void SaveFile::SetDouble(std::string sett, double value)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            set.value = value;
    }
}

void SaveFile::SetBool(std::string sett, bool value)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            set.active = value;
    }
}

double SaveFile::GetDouble(std::string sett)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            return set.value;
    }
    return 0;
}

bool SaveFile::GetBool(std::string sett)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            return set.active;
    }
    return false;
}

setting SaveFile::CreateSetting(bool defaultActive, double defaultValue, std::string defaultName)
{
    setting set;
    set.active = defaultActive;
    set.value = defaultValue;
    memcpy_s(set.name, 128, defaultName.c_str(), 128);
    return set;
}
