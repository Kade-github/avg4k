videoSettings = nil
gameplaySettings = nil
skinSettings = nil
audioSettings = nil

logo = nil
back = nil

currentView = 0
initialSelc = 1
selection = 1

videoTable = {}
defRes = 2

audioTable = {}

keybindTable = {}
gameplayTable = {}
noteskins = {}

view = {}
skins = {}

-- keyboard codes

local keys = {
    [32] = "SPACE",
    [39] = "APOSTROPHE",
    [44] = "COMMA",
    [45] = "MINUS",
    [46] = "PERIOD",
    [47] = "SLASH",
    [48] = "0",
    [49] = "1",
    [50] = "2",
    [51] = "3",
    [52] = "4",
    [53] = "5",
    [54] = "6",
    [55] = "7",
    [56] = "8",
    [57] = "9",
    [59] = "SEMICOLON",
    [61] = "EQUAL",
    [91] = "LEFT_BRACKET",
    [92] = "BACKSLASH",
    [93] = "RIGHT_BRACKET",
    [96] = "GRAVE_ACCENT",
    [161] = "WORLD_1",
    [162] = "WORLD_2",
    [256] = "ESCAPE",
    [257] = "ENTER",
    [258] = "TAB",
    [259] = "BACKSPACE",
    [260] = "INSERT",
    [261] = "DELETE",
    [262] = "RIGHT",
    [263] = "LEFT",
    [264] = "DOWN",
    [265] = "UP",
    [266] = "PAGE_UP",
    [267] = "PAGE_DOWN",
    [268] = "HOME",
    [269] = "END",
    [280] = "CAPS_LOCK",
    [281] = "SCROLL_LOCK",
    [282] = "NUM_LOCK",
    [283] = "PRINT_SCREEN",
    [284] = "PAUSE",
    [290] = "F1",
    [291] = "F2",
    [292] = "F3",
    [293] = "F4",
    [294] = "F5",
    [295] = "F6",
    [296] = "F7",
    [297] = "F8",
    [298] = "F9",
    [299] = "F10",
    [300] = "F11",
    [301] = "F12",
    [302] = "F13",
    [303] = "F14",
    [304] = "F15",
    [305] = "F16",
    [306] = "F17",
    [307] = "F18",
    [308] = "F19",
    [309] = "F20",
    [310] = "F21",
    [311] = "F22",
    [312] = "F23",
    [313] = "F24",
    [314] = "F25",
    [320] = "KP_0",
    [321] = "KP_1",
    [322] = "KP_2",
    [323] = "KP_3",
    [324] = "KP_4",
    [325] = "KP_5",
    [326] = "KP_6",
    [327] = "KP_7",
    [328] = "KP_8",
    [329] = "KP_9",
    [330] = "KP_DECIMAL",
    [331] = "KP_DIVIDE",
    [332] = "KP_MULTIPLY",
    [333] = "KP_SUBTRACT",
    [334] = "KP_ADD",
    [335] = "KP_ENTER",
    [336] = "KP_EQUAL",
    [340] = "LEFT_SHIFT",
    [341] = "LEFT_CONTROL",
    [342] = "LEFT_ALT",
    [343] = "LEFT_SUPER",
    [344] = "RIGHT_SHIFT",
    [345] = "RIGHT_CONTROL",
    [346] = "RIGHT_ALT",
    [347] = "RIGHT_SUPER",
    [348] = "MENU",
    [364] = "LAST",
    -- alphabet
    [65] = "A",
    [66] = "B",
    [67] = "C",
    [68] = "D",
    [69] = "E",
    [70] = "F",
    [71] = "G",
    [72] = "H",
    [73] = "I",
    [74] = "J",
    [75] = "K",
    [76] = "L",
    [77] = "M",
    [78] = "N",
    [79] = "O",
    [80] = "P",
    [81] = "Q",
    [82] = "R",
    [83] = "S",
    [84] = "T",
    [85] = "U",
    [86] = "V",
    [87] = "W",
    [88] = "X",
    [89] = "Y",
    [90] = "Z"
}

function getNameFromCode(code)
    return keys[code]
end

noteskinIndex = 1

function getIndexFromValue(table, value)
    for i = 1, #table do
        if table[i] == value then
            return i
        end
    end
    return 1
end


supportedWidths = {640, 1280, 1920, 2560, 3840}
supportedHeights = {480, 720, 1080, 1440, 2160}

function Round(to_round)
    local divided = to_round / 5
    local rounded = 5 * math.floor(divided)
    return rounded
end

function getIndexFromVideoData()
    for i = 1, #supportedWidths do
        if supportedWidths[i] == videoTable["width"] then
            return i
        end
    end
    return 2
end


function removeStuff()
    for i = 1, #view do
        currentMenu:removeObject(view[i])
    end

    view = {}
end

function create_main()
    removeStuff()
    selection = 1

    back.text = "Press escape to go back"
    
    local video = Text.new(20,math.floor(450 * getHeightScale()), "ArialUnicode.fnt", "Video", 42)

    currentMenu:addObject(video)

    local audio = Text.new(20, video.y + video.height + 20, "ArialUnicode.fnt", "Audio", 42)

    currentMenu:addObject(audio)

    local gameplay = Text.new(20, audio.y + audio.height + 20, "ArialUnicode.fnt", "Gameplay", 42)

    currentMenu:addObject(gameplay)

    local skin = Text.new(20, gameplay.y + gameplay.height + 20, "ArialUnicode.fnt", "Skin", 42)

    currentMenu:addObject(skin)

    view = {video, audio, gameplay, skin}

    view[initialSelc].text = "> " .. view[initialSelc].text;

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    back.size = math.floor(42 * getHeightScale())
    back.y = logo.y + logo.height + math.floor(20 * getHeightScale()) 

    for i = 1, #view do
        view[i].size = math.floor(42 * getHeightScale())
        view[i].y = back.y + math.floor(60 * getHeightScale()) + (i - 1) * math.floor(60 * getHeightScale())
    end
    
end

function create_video()
    removeStuff()

    videoTable = getVideoData()
    defRes = getIndexFromVideoData()

    local resolution = Text.new(20,math.floor(450 * getHeightScale()), "ArialUnicode.fnt", "Resolution (" .. tostring(videoTable["width"]) .. "x" .. tostring(videoTable["height"]) .. ")", 42)

    currentMenu:addObject(resolution)

    local fullscreen = Text.new(20, resolution.y + resolution.height + 20, "ArialUnicode.fnt", "Fullscreen (" .. tostring(videoTable["fullscreen"]) .. ")", 42)

    currentMenu:addObject(fullscreen)

    local borderless = Text.new(20, fullscreen.y + fullscreen.height + 20, "ArialUnicode.fnt", "Borderless (" .. tostring(videoTable["borderless"]) .. ")", 42)

    currentMenu:addObject(borderless)

    view = {resolution, fullscreen, borderless}

    view[selection].text = "> " .. view[selection].text;

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    back.size = math.floor(42 * getHeightScale())
    back.y = logo.y + logo.height + math.floor(20 * getHeightScale()) 

    for i = 1, #view do
        view[i].size = math.floor(42 * getHeightScale())
        view[i].y = back.y + math.floor(60 * getHeightScale()) + (i - 1) * math.floor(60 * getHeightScale())
    end
end

function create_audio()
    removeStuff()

    audioTable = getAudioData()

    local volume = Text.new(20,math.floor(450 * getHeightScale()), "ArialUnicode.fnt", "Volume (" .. tostring(math.floor(audioTable["volume"] * 100)) .. "%)", 42)

    currentMenu:addObject(volume)

    local sfxvolume = Text.new(20,volume.y + volume.height + math.floor(20 * getHeightScale()), "ArialUnicode.fnt", "Sfx Volume (" .. tostring(math.floor(audioTable["sfxVolume"] * 100)) .. "%)", 42)

    currentMenu:addObject(sfxvolume)

    view = {volume, sfxvolume}

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    back.size = math.floor(42 * getHeightScale())
    back.y = logo.y + logo.height + math.floor(20 * getHeightScale()) 

    for i = 1, #view do
        view[i].size = math.floor(42 * getHeightScale())
        view[i].y = back.y + math.floor(60 * getHeightScale()) + (i - 1) * math.floor(60 * getHeightScale())
    end

    view[selection].text = "> " .. view[selection].text;
end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

function create_gameplay()
    removeStuff()

    keybindTable = getKeybindData()
    gameplayTable = getGameplayData()
    noteskins = getNoteskins()

    noteskinIndex = getIndexFromValue(noteskins, gameplayTable["noteskin"])
    
    local key0 = Text.new(20,math.floor(450 * getHeightScale()), "ArialUnicode.fnt", "Key 0 : " .. getNameFromCode(keybindTable["key0"]), 42)

    currentMenu:addObject(key0)

    local key1 = Text.new(20,key0.y + key0.height + 20, "ArialUnicode.fnt", "Key 1 : " .. getNameFromCode(keybindTable["key1"]), 42)

    currentMenu:addObject(key1)

    local key2 = Text.new(20,key1.y + key1.height + 20, "ArialUnicode.fnt", "Key 2 : " .. getNameFromCode(keybindTable["key2"]), 42)

    currentMenu:addObject(key2)

    local key3 = Text.new(20,key2.y + key2.height + 20, "ArialUnicode.fnt", "Key 3 : " .. getNameFromCode(keybindTable["key3"]), 42)

    currentMenu:addObject(key3)

    local keyPause = Text.new(20,key3.y + key3.height + 20, "ArialUnicode.fnt", "Key Pause : " .. getNameFromCode(keybindTable["keyPause"]), 42)

    currentMenu:addObject(keyPause)

    local keyRestart = Text.new(20,keyPause.y + keyPause.height + 20, "ArialUnicode.fnt", "Key Restart : " .. getNameFromCode(keybindTable["keyRestart"]), 42)

    currentMenu:addObject(keyRestart)

    local useCmod = Text.new(20,keyPause.y + keyPause.height + 20, "ArialUnicode.fnt", "Use Constant Scrollspeed : " .. tostring(gameplayTable["useCmod"]), 42)

    currentMenu:addObject(useCmod)

    local cmod = Text.new(20,useCmod.y + useCmod.height + 20, "ArialUnicode.fnt", "Constant Scrollspeed : " .. tostring(gameplayTable["cmod"]), 42)

    currentMenu:addObject(cmod)

    local xmod = Text.new(20,cmod.y + cmod.height + 20, "ArialUnicode.fnt", "Multiplicative Scrollspeed : " .. string.format("%.2f", tostring(gameplayTable["xmod"])) .. "x", 42)

    currentMenu:addObject(xmod)

    local noteskin = Text.new(20,xmod.y + xmod.height + 20, "ArialUnicode.fnt", "Noteskin : " .. gameplayTable["noteskin"], 42)

    currentMenu:addObject(noteskin)

    local backgroundDim = Text.new(20,noteskin.y + noteskin.height + 20, "ArialUnicode.fnt", "Background Opacity : " .. tostring(math.floor(gameplayTable["backgroundDim"] * 100)) .. "%", 42)

    currentMenu:addObject(backgroundDim)

    local noteSpace = Text.new(20,backgroundDim.y + backgroundDim.height + 20, "ArialUnicode.fnt", "Note Space : " .. string.format("%.2f", gameplayTable["noteSpace"]), 42)

    currentMenu:addObject(noteSpace)

    local startTime = Text.new(20,noteSpace.y + noteSpace.height + 20, "ArialUnicode.fnt", "Start Time : " .. string.format("%.2f", gameplayTable["startTime"]) .. " seconds.", 42)

    currentMenu:addObject(startTime)

    local downscroll = Text.new(20,startTime.y + startTime.height + 20, "ArialUnicode.fnt", "Downscroll : " .. tostring(gameplayTable["downscroll"]), 42)

    currentMenu:addObject(downscroll)

    view = {key0, key1, key2, key3, keyPause, keyRestart, useCmod, cmod, xmod, downscroll, noteskin, backgroundDim, noteSpace, startTime}

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    back.size = math.floor(42 * getHeightScale())
    back.y = logo.y + logo.height + math.floor(20 * getHeightScale())

    for i = 1, #view do
        view[i].size = math.floor(42 * getHeightScale())
        view[i].y = back.y + math.floor(60 * getHeightScale()) + (i - 1) * math.floor(60 * getHeightScale())
    end

    view[selection].text = "> " .. view[selection].text;
end

function create_skin()
    removeStuff()

    skins = getSkins()

    back.text = "Press escape to go back; Select a skin :"
    
    for i = 1, #skins do
        local skinText = Text.new(20,back.y + back.height + 20 + (i - 1) * math.floor(60 * getHeightScale()), "ArialUnicode.fnt", skins[i], 42)

        currentMenu:addObject(skinText)

        table.insert(view, skinText)
    end

    view[selection].text = "> " .. view[selection].text;

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    back.size = math.floor(42 * getHeightScale())
    back.y = logo.y + logo.height + math.floor(20 * getHeightScale()) 

    for i = 1, #view do
        view[i].size = math.floor(42 * getHeightScale())
        view[i].y = back.y + math.floor(60 * getHeightScale()) + (i - 1) * math.floor(60 * getHeightScale())
    end
end

function create()
    logo = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(logo)

    back = Text.new(20, 20, "ArialUnicode.fnt", "Press escape to go back", 42)

    currentMenu:addObject(back)

    create_main()

end

pressLeft = false
pressRight = false

frameSkip = 0


function keyRelease(data)
    if data == 263 then
        pressLeft = false
    end
    if data == 262 then
        pressRight = false
    end
    frameSkip = 0
end

keybinding = false

function keyPress(data)
    -- f3

    if data == 292 then
        return
    end
    -- enter

    if data == 257 then
        if currentView == 0 then
            if initialSelc == 1 then
                currentView = 1
                create_video()
            end
            if initialSelc == 2 then
                currentView = 2
                selection = 1
                create_audio()
            end
            if initialSelc == 3 then
                currentView = 3
                selection = 1
                create_gameplay()
            end
            if initialSelc == 4 then
                currentView = 4
                create_skin()
            end
        elseif currentView == 1 then
            if selection == 1 and not videoTable["borderless"] then
                defRes = defRes + 1
                if defRes > #supportedWidths then
                    defRes = 1
                end
                videoTable["width"] = supportedWidths[defRes]
                videoTable["height"] = supportedHeights[defRes]

                setVideoData(videoTable)

                create_video()
            end
            if selection == 2 then
                videoTable["fullscreen"] = not videoTable["fullscreen"]

                if videoTable["borderless"] then
                    videoTable["borderless"] = false
                end

                setVideoData(videoTable)
                create_video()
            end
            if selection == 3 then
                videoTable["borderless"] = not videoTable["borderless"]

                if videoTable["fullscreen"] then
                    videoTable["fullscreen"] = false
                end

                -- set to max otherwise it looks weird lol

                videoTable["width"] = -1
                videoTable["height"] = -1

                setVideoData(videoTable)
                create_video()
            end
        elseif currentView == 2 then
            if selection == 1 then
                audioTable["volume"] = Round((audioTable["volume"] + 0.25) * 100) / 100
                if audioTable["volume"] > 1 then
                    audioTable["volume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
            if selection == 2 then
                audioTable["sfxVolume"] = Round((audioTable["sfxVolume"] + 0.25) * 100) / 100
                if audioTable["sfxVolume"] > 1 then
                    audioTable["sfxVolume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
        elseif currentView == 3 then
            if selection < 7 then -- keybinds
                if not keybinding then
                    keybinding = true
                    view[selection].text = view[selection].text .. " <";
                    return
                else
                    keybinding = false
                    create_gameplay()
                end
            else -- everything else
                if selection == 7 then
                    gameplayTable["useCmod"] = not gameplayTable["useCmod"]
                    setGameplayData(gameplayTable)
                    create_gameplay()
                end

                -- downscroll

                if selection == 10 then
                    gameplayTable["downscroll"] = not gameplayTable["downscroll"]
                    print(tostring(gameplayTable["downscroll"]))
                    setGameplayData(gameplayTable)
                    create_gameplay()
                end
            end
        elseif currentView == 4 then
            setSkin(skins[selection])
        end
    end

    if keybinding then
        print(tostring(selection))
        if selection < 5 then
            keybindTable["key" .. tostring(selection - 1)] = data
            setKeybindData(keybindTable)
            create_gameplay()
        else
            if selection == 5 then
                keybindTable["keyPause"] = data
                setKeybindData(keybindTable)
                create_gameplay()
            elseif selection == 6 then
                keybindTable["keyRestart"] = data
                setKeybindData(keybindTable)
                create_gameplay()
            end
        end
        keybinding = false
        return
    end

    -- escape

    if data == 256 then
        if currentView == 0 then
            switchTo("Scripts/MainMenu.lua")
        else
            currentView = 0
            create_main()
        end
    end

    -- left 

    if data == 263 then
        pressLeft = true
        frameSkip = 0
        if currentView == 1 then
            if selection == 1 and not videoTable["borderless"] then
                defRes = defRes - 1
                if defRes < 1 then
                    defRes = 1
                end
                videoTable["width"] = supportedWidths[defRes]
                videoTable["height"] = supportedHeights[defRes]
                setVideoData(videoTable)
                create_video()
            end
        end
        if currentView == 2 then
            if selection == 1 then
                audioTable["volume"] = audioTable["volume"] - 0.01
                if audioTable["volume"] < 0 then
                    audioTable["volume"] = 1
                end
                setAudioData(audioTable)
                create_audio()
            end
            if selection == 2 then
                audioTable["sfxVolume"] = audioTable["sfxVolume"] - 0.01
                if audioTable["sfxVolume"] < 0 then
                    audioTable["sfxVolume"] = 1
                end
                setAudioData(audioTable)
                create_audio()
            end
        end

        if currentView == 3 then
            if selection == 8 then
                gameplayTable["cmod"] = gameplayTable["cmod"] - 1
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 9 then
                gameplayTable["xmod"] = gameplayTable["xmod"] - 0.01

                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 11 then
                noteskinIndex = noteskinIndex - 1
                if noteskinIndex < 1 then
                    noteskinIndex = #noteskins
                end
                gameplayTable["noteskin"] = noteskins[noteskinIndex]
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 12 then
                gameplayTable["backgroundDim"] = gameplayTable["backgroundDim"] - 0.01
                if gameplayTable["backgroundDim"] < 0 then
                    gameplayTable["backgroundDim"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 13 then
                gameplayTable["noteSpace"] = gameplayTable["noteSpace"] - 0.01
                if gameplayTable["noteSpace"] < 1 then
                    gameplayTable["noteSpace"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 14 then
                gameplayTable["startTime"] = gameplayTable["startTime"] - 0.01
                if gameplayTable["startTime"] < 1 then
                    gameplayTable["startTime"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end
        end
    end

    -- right

    if data == 262 then
        pressRight = true
        frameSkip = 0
        if currentView == 1 then
            if selection == 1 then
                defRes = defRes + 1
                if defRes > #supportedWidths then
                    defRes = #supportedWidths
                end
                videoTable["width"] = supportedWidths[defRes]
                videoTable["height"] = supportedHeights[defRes]
                setVideoData(videoTable)
                create_video()
            end
        end
        if currentView == 2 then
            if selection == 1 then
                audioTable["volume"] = audioTable["volume"] + 0.01
                if audioTable["volume"] > 1 then
                    audioTable["volume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
            if selection == 2 then
                audioTable["sfxVolume"] = audioTable["sfxVolume"] + 0.01
                if audioTable["sfxVolume"] > 1 then
                    audioTable["sfxVolume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
        end

        if currentView == 3 then
            if selection == 8 then
                gameplayTable["cmod"] = gameplayTable["cmod"] + 1
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 9 then
                gameplayTable["xmod"] = gameplayTable["xmod"] + 0.01
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 11 then
                noteskinIndex = noteskinIndex + 1
                if noteskinIndex > #noteskins then
                    noteskinIndex = 1
                end
                gameplayTable["noteskin"] = noteskins[noteskinIndex]
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 12 then
                gameplayTable["backgroundDim"] = gameplayTable["backgroundDim"] + 0.01
                if gameplayTable["backgroundDim"] > 1 then
                    gameplayTable["backgroundDim"] = 0
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 13 then
                gameplayTable["noteSpace"] = gameplayTable["noteSpace"] + 0.01
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 14 then
                gameplayTable["startTime"] = gameplayTable["startTime"] + 0.01
                setGameplayData(gameplayTable)
                create_gameplay()
            end
        end
    end

    if currentView == 0 then
        view[initialSelc].text = string.sub(view[initialSelc].text, 3, string.len(view[initialSelc].text));
    else
        view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text));
    end

    if data == 265 then -- up

        if currentView == 0 then
            initialSelc = initialSelc - 1

            if initialSelc < 1 then
                initialSelc = #view
            end
        else

            selection = selection - 1

            if selection < 1 then
                selection = #view
            end
        end
    end

    if data == 264 then -- down

        if currentView == 0 then
            initialSelc = initialSelc + 1

            if initialSelc > #view then
                initialSelc = 1
            end
        else
            selection = selection + 1

            if selection > #view then
                selection = 1
            end
        end
    end

    if currentView == 0 then
        view[initialSelc].text = "> " .. view[initialSelc].text;
    else
        view[selection].text = "> " .. view[selection].text;
    end
end
function draw()
    if currentView == 4 then
        for i = 1, #skins do
            local away = i - selection

            local p = view[i]

            p.y = logo.y + logo.height + math.floor(300 * getHeightScale()) + (away * math.floor(40 * getHeightScale()))

            if p.y < logo.y + logo.height + math.floor(64 * getHeightScale()) then
                p.y = -1000
            end
    
            if p.y > math.floor(900 * getHeightScale()) then
                p.y = -1000
            end    
        end
    elseif currentView == 3 then
        for i = 1, #view do
            local away = i - selection

            local p = view[i]

            p.y = logo.y + logo.height + math.floor(300 * getHeightScale()) + (away * math.floor(40 * getHeightScale()))

            if p.y < logo.y + logo.height + math.floor(64 * getHeightScale()) then
                p.y = -1000
            end
    
            if p.y > math.floor(900 * getHeightScale()) then
                p.y = -1000
            end    
        end
    end

    if frameSkip < 60  then
        frameSkip = frameSkip + 1
        return
    end
    
    if pressLeft then
        if currentView == 2 then
            if selection == 1 then
                audioTable["volume"] = audioTable["volume"] - 0.005
                if audioTable["volume"] < 0 then
                    audioTable["volume"] = 1
                end
                setAudioData(audioTable)
                create_audio()
            end
            if selection == 2 then
                audioTable["sfxVolume"] = audioTable["sfxVolume"] - 0.005
                if audioTable["sfxVolume"] < 0 then
                    audioTable["sfxVolume"] = 1
                end
                setAudioData(audioTable)
                create_audio()
            end
        end
        if currentView == 3 then
            if selection == 8 then
                gameplayTable["cmod"] = gameplayTable["cmod"] - 1
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 9 then
                gameplayTable["xmod"] = gameplayTable["xmod"] - 0.005
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 11 then
                gameplayTable["backgroundDim"] = gameplayTable["backgroundDim"] - 0.005
                if gameplayTable["backgroundDim"] < 0 then
                    gameplayTable["backgroundDim"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 13 then
                gameplayTable["noteSpace"] = gameplayTable["noteSpace"] - 0.01
                if gameplayTable["noteSpace"] < 1 then
                    gameplayTable["noteSpace"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 14 then
                gameplayTable["startTime"] = gameplayTable["startTime"] - 0.01
                if gameplayTable["startTime"] < 1 then
                    gameplayTable["startTime"] = 1
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end
        end
    end

    if pressRight then
        if currentView == 2 then
            if selection == 1 then
                audioTable["volume"] = audioTable["volume"] + 0.005
                if audioTable["volume"] > 1 then
                    audioTable["volume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
            if selection == 2 then
                audioTable["sfxVolume"] = audioTable["sfxVolume"] + 0.005
                if audioTable["sfxVolume"] > 1 then
                    audioTable["sfxVolume"] = 0
                end
                setAudioData(audioTable)
                create_audio()
            end
        end
        if currentView == 3 then
            if selection == 8 then
                gameplayTable["cmod"] = gameplayTable["cmod"] + 1
                setGameplayData(gameplayTable)
                create_gameplay()
            end
            if selection == 9 then
                gameplayTable["xmod"] = gameplayTable["xmod"] + 0.005
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 11 then
                gameplayTable["backgroundDim"] = gameplayTable["backgroundDim"] + 0.005
                if gameplayTable["backgroundDim"] > 1 then
                    gameplayTable["backgroundDim"] = 0
                end
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 13 then
                gameplayTable["noteSpace"] = gameplayTable["noteSpace"] + 0.1
                setGameplayData(gameplayTable)
                create_gameplay()
            end

            if selection == 14 then
                gameplayTable["startTime"] = gameplayTable["startTime"] + 0.1
                setGameplayData(gameplayTable)
                create_gameplay()
            end
        end
    end
end