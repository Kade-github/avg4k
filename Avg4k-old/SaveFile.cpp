#include "SaveFile.h"
#include "Helpers.h"
#include "Chart.h"
#include <shlobj.h>
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
    VM_START
    // default settings

    // {takesActive, takesString, takesDouble, defaultActive, defaultString, defaultDouble, defaultMin, defaultMax, defaultIncrm, unique, suffix, isDropdown}

    settingHeader defaultHeader;
    defaultHeader.settingsVersion = "v3.4"; // KADE PLEASE FUCKING CHANGE THIS GOD DAMN IT - kade from the past

    defaultSettings.push_back(CreateSetting("Downscroll",{true}));
    defaultSettings.push_back(CreateSetting("Scrollspeed",{false,false,true,false,"",800,200,1900}));
    defaultSettings.push_back(CreateSetting("Offset",{false,false,true,false,"",0,-1000,1000}));
    defaultSettings.push_back(CreateSetting("Hitsounds", {true}));
    defaultSettings.push_back(CreateSetting("Note Size",{false,false,true,false,"",1,0.8,1.8,0.1,false,"x"}));
    defaultSettings.push_back(CreateSetting("Keybinds ", {false,true,false,false,"D-F-J-K", 0, 0, 4, 0, true}));
    defaultSettings.push_back(CreateSetting("Noteskin", {false,true,false,false, "arrow",0,0,0,0,false,"",true}));
    defaultSettings.push_back(CreateSetting("Resolution", { false,true,false,false, "1280x720",0,0,0,0,false,"",true }));
    defaultSettings.push_back(CreateSetting("Fullscreen", { false,true,false,false, "Windowed",0,0,0,0,false,"",true }));
    defaultSettings.push_back(CreateSetting("Music Volume", { false,false,true,false,"",0.2,0.1,1,0.1,false}));
    defaultSettings.push_back(CreateSetting("Hitsounds Volume", { false,false,true,false,"",0.4,0.1,1,0.1,false }));
    defaultSettings.push_back(CreateSetting("Auto Accent Colors", { true, false, false, true }));
    defaultSettings.push_back(CreateSetting("Accent Color R", { false,false,true,false,"",0,0,255,1,false }));
    defaultSettings.push_back(CreateSetting("Accent Color G", { false,false,true,false,"",0,0,255,1,false }));
    defaultSettings.push_back(CreateSetting("Accent Color B", { false,false,true,false,"",0,0,255,1,false }));
    defaultSettings.push_back(CreateSetting("Lane Underway Transparency", { false,false,true,false,"",0.8,0,1,0.1,false }));
    defaultSettings.push_back(CreateSetting("Background Transparency", { false,false,true,false,"",1,0,1,0.1,false }));
    defaultSettings.push_back(CreateSetting("FPS Limit", { false,false,true,false,"",240,10,4000 }));
    defaultSettings.push_back(CreateSetting("Start Delay", { false,false,true,false,"",5,0,10,1,false,"s"}));
    defaultSettings.push_back(CreateSetting("Submit Scores", { true, false, false, true }));
    defaultSettings.push_back(CreateSetting("Show Judgement Count", { true }));
    defaultSettings.push_back(CreateSetting("nonChange_chartTheme", {}));
    defaultSettings.push_back(CreateSetting("nonChange_chartHistory",{}));
    defaultSettings.push_back(CreateSetting("nonChange_chartWaveform", {true}));
    defaultSettings.push_back(CreateSetting("nonChange_beatLines", { true }));
    defaultSettings.push_back(CreateSetting("nonChange_infoPanel", { true }));
    defaultSettings.push_back(CreateSetting("nonChange_noteTick", { false }));
    defaultSettings.push_back(CreateSetting("nonChange_beatTick", { false }));
    defaultSettings.push_back(CreateSetting("nonChange_colorShit", { false,true,false,false,"128,128,255"}));
    defaultSettings.push_back(CreateSetting("nonChange_minimap", { false }));
    defaultSettings.push_back(CreateSetting("Ignore mod noteskin", { true}));
    defaultSettings.push_back(CreateSetting("Ignore mod scrollspeed", { true }));
    defaultSettings.push_back(CreateSetting("Ignore mod starting scroll", { true }));
    defaultSettings.push_back(CreateSetting("Show Song Position", { true, false, false, true }));
    defaultSettings.push_back(CreateSetting("Use XMOD Scroll", { true, false, false, false }));
    defaultHeader.settings = defaultSettings;

    std::string bang = getPath();

    std::ifstream ifs(bang + "settings.avg2", std::ios::binary | std::ios::in | std::ios::out);
    if (!ifs.good())
    {
        currentHeader = defaultHeader;
        Save();
        return;
    }
   
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    ifs.close();
    msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
    upd.get().convert(currentHeader);

    // check for new settings
    if (currentHeader.settingsVersion != defaultHeader.settingsVersion)
    {
        currentHeader.settingsVersion = defaultHeader.settingsVersion;
        for (int i = 0; i < defaultSettings.size(); i++)
        {
            if (i < currentHeader.settings.size())
            {
                if (defaultHeader.settings[i].name != currentHeader.settings[i].name)
                {
                    currentHeader.settings[i] = defaultHeader.settings[i];
                }
            }
            else
            {
                currentHeader.settings.push_back(defaultHeader.settings[i]);
            }
        }
        Save();
    }
    VM_END
}

void SaveFile::Save()
{
    VM_START

    std::string bath = getPath();

    std::ofstream of(bath + "settings.avg2", std::ios::binary | std::ios::out);

    std::stringstream bitch;

    msgpack::pack(bitch, currentHeader);

    of << bitch.str();

    of.close();
    VM_END
}

std::string SaveFile::getPath()
{
    PWSTR   docs;

    HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &docs);


    std::string bangerPath = "";


    if (SUCCEEDED(hr)) {
        std::wstring str = std::wstring(docs);

        bangerPath = std::string(str.begin(), str.end()) + "/Average4K/";

        CreateDirectory(Helpers::s2ws((bangerPath)).c_str(), NULL);

        if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_SUCCESS)
        {
            bangerPath = "/data/";
        }
    }

    return bangerPath;
}

void SaveFile::saveScore(std::map<float, float> noteTimings, float acc, int combo, std::string name, std::string artist, long id, long chartIndex, long diffIndex)
{
    scoreHeader h;
    h.t = noteTimings;
    h.a = acc;
    h.c = combo;
    h.pId = id;
    h.cId = chartIndex;
    h.dId = diffIndex;
    h.ar = artist;
    h.sn = name;

    std::string p = getPath();

    CreateDirectory(Helpers::s2ws((p + "scores/")).c_str(), NULL); // this will create a directory and if it already exists, it will do nothing.

    std::ofstream of(p + "scores/" + name + "-" + artist + "-" + std::to_string(std::time(0)) + ".avgScore", std::ios::binary | std::ios::out);

    std::stringstream bitch;

    msgpack::pack(bitch, h);

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

std::vector<scoreHeader> SaveFile::getScores(std::string name, std::string artist, long id, long chartIndex, long diffIndex)
{
    std::vector<scoreHeader> scores;

    std::string p = getPath();

    CreateDirectory(Helpers::s2ws((p + "scores/")).c_str(), NULL); // this will create a directory and if it already exists, it will do nothing.

    for (const auto& entry : std::filesystem::directory_iterator(p + "scores"))
    {
        std::string s = entry.path().string();

        if (s.find(name) != std::string::npos && s.find(artist) != std::string::npos)
        {
            scoreHeader h;
            h.t = {};
            std::ifstream sP(s, std::ios::binary | std::ios::in | std::ios::out);

            std::stringstream buffer;
            buffer << sP.rdbuf();

            sP.close();
            msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
            upd.get().convert(h);

            if (h.t.size() > 0)
                if (h.ar == artist && h.sn == name && id == h.pId && chartIndex == h.cId && diffIndex == h.dId)
                    scores.push_back(h);   
        }
    }

    return scores;
}

std::vector<std::string> SaveFile::ObtainDropDownSettingList(std::string set)
{
    VM_START
    if (set == "Noteskin")
    {
        std::vector<std::string> noteskins;
        for (const auto& entry : std::filesystem::directory_iterator("assets/noteskin/"))
        {
            if (entry.is_directory())
            {
                std::vector<std::string> spl = Chart::split(entry.path().string(), '/');
                noteskins.push_back(spl[spl.size() - 1].c_str());
            }
        }

        return noteskins;
    }
    if (set == "Resolution")
    {
        // current monitor resolution

        int mw, mh;

        std::vector<std::string> resStrings;

        Helpers::GetDesktopResolution(mw, mh);

        // lowest to highest
        std::vector<std::vector<int>> resolutions;
        resolutions.push_back({ 1280,720 });
        resolutions.push_back({ 1920,1080 });
        resolutions.push_back({ 2048,1080 });
        resolutions.push_back({ 2560,1440 });
        resolutions.push_back({ mw,mh });


        for (int i = 0; i < resolutions.size(); i++)
        {
            int w, h;
            w = resolutions[i][0];
            h = resolutions[i][1];
            std::string s = std::to_string(w) + "x" + std::to_string(h);
            if (std::find(resStrings.begin(), resStrings.end(), s) == resStrings.end())
                resStrings.push_back(s);
        }

        return resStrings;
    }
    if (set == "Fullscreen")
    {
        return {"Fullscreen", "Windowed", "Borderless"};
    }
    VM_END
    return std::vector<std::string>();
}

std::vector<int> SaveFile::ObtainResolution()
{
    STR_ENCRYPT_START
    std::string resText = GetString("Resolution");
    if (GetString("Fullscreen") == "Borderless")
    {
        int mw, mh;

        Helpers::GetDesktopResolution(mw, mh);
        resText = std::to_string(mw) + "x" + std::to_string(mh);
    }


    STR_ENCRYPT_END
    if (resText == "640x480")
        resText = "1280x720";
    return { std::stoi(resText.substr(0,resText.find("x"))), std::stoi(resText.substr(resText.find("x") + 1,resText.length()))};
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
