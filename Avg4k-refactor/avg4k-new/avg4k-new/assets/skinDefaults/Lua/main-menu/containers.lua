Containers = {}

function Containers.soloCreate(c)
    Containers.scontainer = {}
    Containers.scontainer.packs = helper.createSprite("Menu/MainMenu/Solo/packscontainer", 0, 0)
    create(Containers.scontainer.packs)
    c:add(Containers.scontainer.packs)
    Containers.scontainer.packs.tag = "packContainer"
    Containers.scontainer.packs.order = 2
    Containers.scontainer.packs.ratio = true

    Containers.scontainer.packs.transform.y = 0.12

    Containers.scontainer.packs.transform.h = 0.88
    Containers.scontainer.packs.transform.w = 0.2

    Containers.scontainer.packsText = text.new(0, 0, "ArialBold.fnt", "Packs")
    create(Containers.scontainer.packsText)
    c:add(Containers.scontainer.packsText)
    Containers.scontainer.packsText.tag = "packsText"

    Containers.scontainer.packsText.size = 24 * skin["upscale"]
    Containers.scontainer.packsText.characterSpacing = 3

    Containers.scontainer.packsText.order = 1
    Containers.scontainer.packsText.ratio = true

    Containers.scontainer.packsText.transform.x = 0.02
    Containers.scontainer.packsText.transform.y = 0.03

    Containers.scontainer.packsInfo = text.new(0, 0, "ArialItalic.fnt", "holy crap lois")
    create(Containers.scontainer.packsInfo)
    c:add(Containers.scontainer.packsInfo)
    Containers.scontainer.packsInfo.tag = "packsInfo"

    Containers.scontainer.packsInfo.size = 18 * skin["upscale"]

    Containers.scontainer.packsInfo.order = 1
    Containers.scontainer.packsInfo.ratio = true

    Containers.scontainer.packsInfo.transform.x = 0.02
    Containers.scontainer.packsInfo.transform.y = 0.07

    helper.initContainer(Containers.scontainer.packs, true)

    Containers.scontainer.songWheel = helper.createSprite("Menu/MainMenu/Solo/packscontainer", 0, 0)
    create(Containers.scontainer.songWheel)
    c:add(Containers.scontainer.songWheel)
    Containers.scontainer.songWheel.tag = "songWheel"
    Containers.scontainer.songWheel.order = 2
    Containers.scontainer.songWheel.ratio = true
    Containers.scontainer.songWheel.transform.x = Containers.scontainer.packs.transform.x +
        Containers.scontainer.packs.transform.w
    Containers.scontainer.songWheel.transform.y = 0.12

    Containers.scontainer.songWheel.transform.h = 0.88
    Containers.scontainer.songWheel.transform.w = 0.54
    Containers.scontainer.songWheel.transform.alpha = 0

    songWheel.init(Containers.scontainer.songWheel)
    helper.initContainer(Containers.scontainer.songWheel, false)

    Containers.scontainer.songInfo = helper.createSprite("Menu/MainMenu/Solo/songcontainer", 0, 0)
    create(Containers.scontainer.songInfo)
    c:add(Containers.scontainer.songInfo)
    Containers.scontainer.songInfo.tag = "songInfo"
    Containers.scontainer.songInfo.order = 2
    Containers.scontainer.songInfo.ratio = true
    Containers.scontainer.songInfo.transform.x = Containers.scontainer.songWheel.transform.x +
        Containers.scontainer.songWheel.transform.w
    Containers.scontainer.songInfo.transform.y = 0
    Containers.scontainer.songInfo.transform.h = 1
    Containers.scontainer.songInfo.transform.w = 0.26
    helper.initContainer(Containers.scontainer.songInfo, false)

    Containers.scontainer.title = text.new(0, 0, "ArialBold.fnt", "Song Title")
    create(Containers.scontainer.title)
    Containers.scontainer.title.size = 28 * skin["upscale"]
    Containers.scontainer.title.characterSpacing = 3
    Containers.scontainer.songInfo:add(Containers.scontainer.title)
    Containers.scontainer.title.ratio = true
    Containers.scontainer.title.transform.x = 0.06
    Containers.scontainer.title.transform.y = 0.04
    Containers.scontainer.title.tag = "songInfo_title"
    Containers.scontainer.title.order = 2
    Containers.scontainer.title.transform.alpha = 0

    Containers.scontainer.artist = text.new(0, 0, "Arial.fnt", "Song Artist")
    create(Containers.scontainer.artist)
    Containers.scontainer.artist.size = 20 * skin["upscale"]
    Containers.scontainer.artist.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.artist)
    Containers.scontainer.artist.ratio = true
    Containers.scontainer.artist.transform.x = 0.06
    Containers.scontainer.artist.transform.y = 0.08
    Containers.scontainer.artist.tag = "songInfo_artist"
    Containers.scontainer.artist.order = 2
    Containers.scontainer.artist.transform.alpha = 0

    Containers.scontainer.diffValue = text.new(0, 0, "ArialBold.fnt", "Song Diff")
    create(Containers.scontainer.diffValue)
    Containers.scontainer.diffValue.size = 28 * skin["upscale"]
    Containers.scontainer.diffValue.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.diffValue)
    Containers.scontainer.diffValue.ratio = true
    Containers.scontainer.diffValue.ratio = true
    Containers.scontainer.diffValue.center = true
    Containers.scontainer.diffValue.transform.x = 0.5
    Containers.scontainer.diffValue.transform.y = 0.55
    Containers.scontainer.diffValue.tag = "songInfo_difficulty"
    Containers.scontainer.diffValue.order = 2
    Containers.scontainer.diffValue.transform.alpha = 0

    Containers.scontainer.diffName = text.new(0, 0, "Arial.fnt", "Song Diff Name")
    create(Containers.scontainer.diffName)
    Containers.scontainer.diffName.size = 26 * skin["upscale"]
    Containers.scontainer.diffName.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.diffName)
    Containers.scontainer.diffName.ratio = true
    Containers.scontainer.diffName.center = true
    Containers.scontainer.diffName.transform.x = 0.5
    Containers.scontainer.diffName.transform.y = 0.5
    Containers.scontainer.diffName.tag = "songInfo_difficultyName"
    Containers.scontainer.diffName.order = 2
    Containers.scontainer.diffName.transform.alpha = 0

    Containers.scontainer.diffLeftArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow",
        0, 0)
    create(Containers.scontainer.diffLeftArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.diffLeftArrow)
    Containers.scontainer.diffLeftArrow.transform.alpha = 0
    Containers.scontainer.diffLeftArrow.ratio = true
    Containers.scontainer.diffLeftArrow.order = 2
    Containers.scontainer.diffLeftArrow.tag = "songInfo_difficultyLeftArrow"
    Containers.scontainer.diffLeftArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.diffRightArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow",
        0, 0)
    create(Containers.scontainer.diffRightArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.diffRightArrow)
    Containers.scontainer.diffRightArrow.transform.alpha = 0
    Containers.scontainer.diffRightArrow.ratio = true
    Containers.scontainer.diffRightArrow.transform.angle = 180
    Containers.scontainer.diffRightArrow.order = 2
    Containers.scontainer.diffRightArrow.tag = "songInfo_difficultyRightArrow"
    Containers.scontainer.diffRightArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.chartType = text.new(0, 0, "Arial.fnt", "Song Chart Type")
    create(Containers.scontainer.chartType)
    Containers.scontainer.chartType.size = 22 * skin["upscale"]
    Containers.scontainer.chartType.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.chartType)
    Containers.scontainer.chartType.ratio = true
    Containers.scontainer.chartType.transform.x = 0.02
    Containers.scontainer.chartType.transform.y = 0.96
    Containers.scontainer.chartType.tag = "songInfo_chartType"
    Containers.scontainer.chartType.order = 2
    Containers.scontainer.chartType.transform.alpha = 0

    Containers.scontainer.onlineText = text.new(0, 0, "Arial.fnt", "Song Online Text")
    create(Containers.scontainer.onlineText)
    Containers.scontainer.onlineText.size = 22 * skin["upscale"]
    Containers.scontainer.onlineText.ratio = true
    Containers.scontainer.onlineText.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.onlineText)
    Containers.scontainer.onlineText.tag = "songInfo_onlineText"
    Containers.scontainer.onlineText.order = 2
    Containers.scontainer.onlineText.transform.alpha = 0

    Containers.scontainer.bg = nil
end

function containers_settingChanged(setting, newValue)
    if setting == "Use CMOD" then -- toggle xmod to the opposite
        setSetting("Use XMOD", tostring(not newValue))
    end
    if setting == "Scroll Speed" or setting == "XMOD XMOD" or setting == "Note Offset" or setting == "Start Offset" then
        if helper.isNumber(newValue) then
            local num = tonumber(newValue)
            if setting == "Scroll Speed" and num < 200 then
                return
            end
            if setting == "XMOD XMOD" and num < 0.1 then
                return
            end
            if num < 0 and setting ~= "Note Offset" then
                return
            end
            setSetting(setting, tostring(newValue))
        else
            cprint("[Textbox] Can't set " .. newValue .. ", because it isn't a number!")
        end
        return
    end
    setSetting(setting, tostring(newValue))

    if setting == "Resolution" then
        local res = helper.split(newValue, "x")
        if #res == 2 then
            local w = tonumber(res[1])
            local h = tonumber(res[2])
            setResolution(w, h)
        end
    end
end

function Containers.settings_createCheckbox(c, setting, tag, x, endFunc, infoText)
    Containers.settings_amount = Containers.settings_amount + 0.09
    checkbox.CreateCheckbox(c, setting, tag, { x, Containers.settings_amount },
        endFunc, infoText)
end

function Containers.settings_sameLine()
    Containers.settings_amount = Containers.settings_amount - 0.09
end

function Containers.settings_createTextbox(c, setting, max, tag, x, endFunc, inc, _keyInput, blacklisted, infoText)
    Containers.settings_amount = Containers.settings_amount + 0.09
    textbox.CreateTextbox(c, setting, max, tag, { x, Containers.settings_amount }, endFunc, inc, _keyInput, blacklisted,
        infoText)
end

function Containers.settings_createHeader(c, x, tag, headerText)
    Containers.settings_amount = Containers.settings_amount + 0.09
    local header = text.new(0, 0, "ArialBold.fnt", headerText)
    create(header)
    c:add(header)
    header.size = 28 * skin["upscale"]
    header.ratio = true
    header.transform.x = x
    header.transform.y = Containers.settings_amount
    header.tag = "settings_" .. tag .. "_header"
    header.order = 2
end

function Containers.settingsCreate(c)
    -- basically a y value lol
    Containers.settings_amount = -0.045

    local x = 0.02

    Containers.settings_createHeader(c, x, "gameplay", "Gameplay")

    Containers.settings_createCheckbox(c, "Use CMOD", "settings_useCMOD",
        x,
        containers_settingChanged, "Toggle between time based scrolling, and beat based scrolling.")

    -- all blacklisted characters for numbersOnly
    local numbersOnly = {}
    for i = 65, 90 do
        table.insert(numbersOnly, string.char(i))
    end
    -- add special characters (`,space,etc)
    table.insert(numbersOnly, "`")
    table.insert(numbersOnly, " ")
    table.insert(numbersOnly, "=")
    table.insert(numbersOnly, "[")
    table.insert(numbersOnly, "]")
    table.insert(numbersOnly, "\\")
    table.insert(numbersOnly, ";")
    table.insert(numbersOnly, "'")
    table.insert(numbersOnly, ",")
    table.insert(numbersOnly, "/")


    Containers.settings_createTextbox(c, "Scroll Speed", 4, "settings_scrollSpeed",
        x,
        containers_settingChanged, 15, false, numbersOnly, "The time based scroll speed. In BPM.")

    Containers.settings_createTextbox(c, "XMOD Multiplier", 3, "settings_xmodMultiplier",
        x,
        containers_settingChanged, 0.1, false, numbersOnly,
        "The multiplier to be used when scaling how big beats are in XMOD.")

    Containers.settings_createTextbox(c, "Note Offset", 5, "settings_noteOffset",
        x,
        containers_settingChanged, 15, false, numbersOnly, "The offset of the notes. In seconds, negative is later.")

    Containers.settings_createTextbox(c, "Start Offset", 5, "settings_noteOffset",
        x,
        containers_settingChanged, 15, false, numbersOnly,
        "How long the game should wait before starting the song. In seconds.")

    Containers.settings_createTextbox(c, "Underlane Transparency", 5, "settings_underlaneTransparency",
        x,
        containers_settingChanged, 15, false, numbersOnly,
        "How transparent the under background of the note should be. 1 is fully transparent, 0 is fully opaque.")

    Containers.settings_createTextbox(c, "Left Keybind", 1, "settings_leftKeybind",
        x,
        containers_settingChanged, 15, true, {},
        "")

    Containers.settings_createTextbox(c, "Down Keybind", 1, "settings_downKeybind", x,
        containers_settingChanged, 15, true, {}, "")

    Containers.settings_createTextbox(c, "Up Keybind", 1, "settings_upKeybind", x,
        containers_settingChanged, 15, true, {}, "")

    Containers.settings_createTextbox(c, "Right Keybind", 1, "settings_rightKeybind", x,
        containers_settingChanged, 15, true, {}, "")

    Containers.settings_createHeader(c, x, "audio", "Audio")

    Containers.settings_createTextbox(c, "Music Volume", 5, "settings_musicVolume",
        x,
        containers_settingChanged, 15, false, numbersOnly, "The volume of the music. 1 is the maximum.")

    Containers.settings_createTextbox(c, "Hitsound Volume", 5, "settings_hitsoundVolume",
        x,
        containers_settingChanged, 15, false, numbersOnly, "The volume of the hitsounds. 1 is the maximum.")

    Containers.settings_createHeader(c, x, "graphics", "Graphics")



    Containers.settings_createHeader(c, x, "misc", "Misc")
end

function Containers.create()
    Containers.solo = helper.createContainer("container", 0, 0)
    add(Containers.solo)
    Containers.solo.tag = "solo"
    Containers.solo.ratio = true
    Containers.solo.transform.x = 0.5
    Containers.solo.center = true
    Containers.solo.transform.y = -0.5
    Containers.solo.transform.scale = skin["upscale"]

    Containers.endRect = copyRect(Containers.solo.transform)
    Containers.endRect.y = 0.5

    Containers.soloCreate(Containers.solo)

    Containers.multiplayer = helper.createContainer("container", 0, 0)
    add(Containers.multiplayer)
    Containers.multiplayer.tag = "multiplayer"
    Containers.multiplayer.ratio = true
    Containers.multiplayer.transform.x = 1.5
    Containers.multiplayer.center = true
    Containers.multiplayer.transform.y = 0.5
    Containers.multiplayer.transform.scale = skin["upscale"]

    Containers.settings = helper.createContainer("container", 0, 0)
    add(Containers.settings)
    Containers.settings.tag = "settings"
    Containers.settings.ratio = true
    Containers.settings.transform.x = 1.5
    Containers.settings.center = true
    Containers.settings.transform.y = 0.5
    Containers.settings.transform.scale = skin["upscale"]

    Containers.settings_amount = 0

    Containers.settingsCreate(Containers.settings)

    -- Tweens

    tween(Containers.solo, Containers.endRect, 1, "outcubic", "")

    helper.initContainer(Containers.solo, false)
    helper.initContainer(Containers.multiplayer, false)
    helper.initContainer(Containers.settings, true)
end
