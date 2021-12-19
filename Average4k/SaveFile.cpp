#include "SaveFile.h"

template <typename T>
bool contains(std::vector<T> vec, const T& elem)
{
    bool result = false;
    if (find(vec.begin(), vec.end(), elem) != vec.end())
    {
        result = true;
    }
    return result;
}

SaveFile::SaveFile()
{
    // default settings
    defaultSettings.push_back(CreateSetting(false, 0, "downscroll", true, 0,1));
    defaultSettings.push_back(CreateSetting(true, 1100, "scrollspeed", false, 1,5000));
    defaultSettings.push_back(CreateSetting(true, 0, "offset", false,-1000,1000));
    defaultSettings.push_back(CreateSetting(false, 0, "hitsounds", true,0,1));

    std::ifstream ifs("settings.avg");
    if (!ifs.good())
    {
        CreateNewFile();
        return;
    }
   
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
    upd.get().convert(settings);

    // check for new settings
    if (settings.size() != defaultSettings.size())
    {
        for (int i = 0; i < defaultSettings.size(); i++)
        {
            if (i > settings.size() - 1)
            {
                settings.push_back(defaultSettings[i]);
                std::cout << "user didn't have " << defaultSettings[i].name << std::endl;
            }
        }
        Save();
    }
}

void SaveFile::CreateNewFile()
{
    for (setting& set : defaultSettings)
        settings.push_back(set);
    Save();
}

void SaveFile::Save()
{
    std::ofstream of("settings.avg");

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

setting SaveFile::CreateSetting(bool defaultActive, double defaultValue, std::string defaultName, bool tA, double lowest, double highest)
{
    setting set;
    set.active = defaultActive;
    set.takesActive = tA;
    set.value = defaultValue;
    set.highestValue = highest;
    set.lowestValue = highest;
    memcpy_s(set.name, 128, defaultName.c_str(), 128);
    return set;
}
