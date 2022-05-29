#include "SaveFile.h"
#include "Helpers.h"
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

    // {takesActive, takesString, takesDouble, defaultActive, defaultString, defaultDouble, defaultMin, defaultMax, defaultIncrm, unique, suffix, isDropdown}

    settingHeader defaultHeader;
    defaultHeader.settingsVersion = "v2.1";

    defaultSettings.push_back(CreateSetting("Downscroll",{true}));
    defaultSettings.push_back(CreateSetting("Scrollspeed",{false,false,true,false,"",800,200,1900}));
    defaultSettings.push_back(CreateSetting("Offset",{false,false,true,false,"",0,-1000,1000}));
    defaultSettings.push_back(CreateSetting("Hitsounds", {true}));
    defaultSettings.push_back(CreateSetting("Note Size",{false,false,true,false,"",1,0.8,1.8,0.1,false,"x"}));
    defaultSettings.push_back(CreateSetting("Keybinds", {false,true,false,false,"DFJK", 0, 0, 4, 0, true}));
    defaultSettings.push_back(CreateSetting("Noteskin", {false,true,false,false, "arrow",0,0,0,0,false,"",true}));
    defaultSettings.push_back(CreateSetting("Resolution", { false,true,false,false, "1280x720",0,0,0,0,false,"",true }));
    defaultSettings.push_back(CreateSetting("Fullscreen", { false,true,false,false, "Windowed",0,0,0,0,false,"",true }));
    defaultSettings.push_back(CreateSetting("nonChange_chartTheme", {}));
    defaultSettings.push_back(CreateSetting("nonChange_chartHistory",{}));
    defaultSettings.push_back(CreateSetting("nonChange_chartWaveform", {true}));
    defaultSettings.push_back(CreateSetting("nonChange_beatLines", { true }));
    defaultSettings.push_back(CreateSetting("nonChange_infoPanel", { true }));
    defaultSettings.push_back(CreateSetting("nonChange_noteTick", { false }));
    defaultSettings.push_back(CreateSetting("nonChange_colorShit", { false,true,false,false,"128,128,255"}));
    defaultSettings.push_back(CreateSetting("nonChange_minimap", { false }));
    defaultHeader.settings = defaultSettings;
    std::ifstream ifs("settings.avg2");
    if (!ifs.good())
    {
        currentHeader = defaultHeader;
        Save();
        return;
    }
   
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
    upd.get().convert(currentHeader);

    // check for new settings
    if (currentHeader.settings.size() != defaultHeader.settings.size())
    {
        for (int i = 0; i < defaultSettings.size(); i++)
        {
            if (i > currentHeader.settings.size() - 1)
            {
                currentHeader.settings.push_back(defaultSettings[i]);
                std::cout << "user didn't have " << defaultSettings[i].name << std::endl;
            }
        }
        Save();
    }

}

void SaveFile::Save()
{
    std::ofstream of("settings.avg2");

    std::stringstream bitch;

    msgpack::pack(bitch, currentHeader);

    of << bitch.str();

    of.close();
}

void SaveFile::SetString(std::string sett, std::string value)
{
    int size = value.size();
    if (size > 248)
        size = 248;
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
        {
            set.defaultString = value;
        }
    }
}

void SaveFile::SetDouble(std::string sett, double value)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            set.defaultDouble = value;
    }
}

void SaveFile::SetBool(std::string sett, bool value)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            set.defaultActive = value;
    }
}

setting& SaveFile::getSetting(std::string sett)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            return set;
    }
}

std::vector<std::string> SaveFile::ObtainDropDownSettingList(std::string set)
{
    if (set == "Noteskin")
        return {"arrow"}; // TODO: collect noteskins
    if (set == "Resolution")
    {
        // current monitor resolution

        int mw, mh;

        std::vector<std::string> resStrings;

        Helpers::GetDesktopResolution(mw, mh);

        // lowest to highest
        std::vector<std::vector<int>> resolutions;
        resolutions.push_back({ 640,480 });
        resolutions.push_back({ 1280,720 });
        resolutions.push_back({ 1920,1080 });
        resolutions.push_back({ 2048,1080 });
        resolutions.push_back({ 2560,1440 });
        resolutions.push_back({ 3840,2160 });
        resolutions.push_back({ 7680,4320 });

        for (int i = 0; i < resolutions.size(); i++)
        {
            int w, h;
            w = resolutions[i][0];
            h = resolutions[i][1];
            if (w < mw && h < mh)
            {
                resStrings.push_back(std::to_string(w) + "x" + std::to_string(h));
            }
        }
        resStrings.push_back(std::to_string(mw) + "x" + std::to_string(mh));

        return resStrings;
    }
    if (set == "Fullscreen")
    {
        return {"Fullscreen", "Windowed", "Borderless"};
    }
    return std::vector<std::string>();
}

std::string SaveFile::GetString(std::string sett)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            return set.defaultString;
    }
    return "";
}

double SaveFile::GetDouble(std::string sett)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            return set.defaultDouble;
    }
    return 0;
}

bool SaveFile::GetBool(std::string sett)
{
    for (setting& set : currentHeader.settings)
    {
        std::string s = set.name;
        std::transform(s.begin(), s.end(), s.begin(), Helpers::asciitolower);
        std::transform(sett.begin(), sett.end(), sett.begin(), Helpers::asciitolower);
        if (s == sett)
            return set.defaultActive;
    }
    return false;
}

setting SaveFile::CreateSetting(std::string defaultName, settingConstruct cons)
{
    setting set;

    set.name = defaultName;

    set.defaultActive = cons.defaultActive;
    set.defaultDouble = cons.defaultDouble;
    set.defaultMax = cons.defaultMax;
    set.defaultMin = cons.defaultMin;
    set.takesActive = cons.takesActive;
    set.takesDouble = cons.takesDouble;
    set.takesString = cons.takesString;
    set.defaultString = cons.defaultString;
    set.defaultIncrm = cons.defaultIncrm;
    set.unique = cons.unique;
    set.settingSuffix = cons.settingSuffix;
    set.isDropdown = cons.isDropdown;
    return set;
}
