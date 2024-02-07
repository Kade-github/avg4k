videoSettings = nil
gameplaySettings = nil
skinSettings = nil
audioSettings = nil

logo = nil
back = nil

currentView = 0
selection = 1

videoTable = {}
defRes = 2

audioTable = {}

view = {}


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
    
    local video = Text.new(20,math.floor(450 * getHeightScale()), "ArialUnicode.fnt", "Video", 42)

    currentMenu:addObject(video)

    local audio = Text.new(20, video.y + video.height + 20, "ArialUnicode.fnt", "Audio", 42)

    currentMenu:addObject(audio)

    local gameplay = Text.new(20, audio.y + audio.height + 20, "ArialUnicode.fnt", "Gameplay", 42)

    currentMenu:addObject(gameplay)

    local skin = Text.new(20, gameplay.y + gameplay.height + 20, "ArialUnicode.fnt", "Skin", 42)

    currentMenu:addObject(skin)


    view = {video, audio, gameplay, skin}

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

function keyPress(data)
    -- escape

    if data == 256 then
        if currentView == 0 then
            switchTo("Scripts/MainMenu.lua")
        else
            currentView = 0
            create_main()
        end
    end

    -- enter

    if data == 257 then
        if currentView == 0 then
            if selection == 1 then
                currentView = 1
                create_video()
            end
            if selection == 2 then
                currentView = 2
                selection = 1
                create_audio()
            end
            if selection == 3 then
                currentView = 3
            end
            if selection == 4 then
                currentView = 4
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
    end

    view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text));

    if data == 265 then -- up

        selection = selection - 1

        if selection < 1 then
            selection = #view
        end
    end

    if data == 264 then -- down

        selection = selection + 1

        if selection > #view then
            selection = 1
        end
    end

    view[selection].text = "> " .. view[selection].text;
end
function draw()
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
    end
end