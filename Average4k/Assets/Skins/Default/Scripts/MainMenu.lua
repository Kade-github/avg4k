songs = nil
multiplayer = nil
workshop = nil
settings = nil
exit = nil

texts = nil

selection = 1

function create()
    print("hello :)")
    local s = Sprite.new(20,20, "Images/Logo.png")

    s.width = math.floor(700 * getWidthScale())
    s.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(s)

    songs = Text.new(20, s.y + s.height + math.floor(20 * getHeightScale()), "Arial.fnt", "> Songs", math.floor(42 * getHeightScale()))

    currentMenu:addObject(songs)

    multiplayer = Text.new(20, songs.y + songs.height + math.floor(40 * getHeightScale()), "Arial.fnt", "Multiplayer", math.floor(42 * getHeightScale()))

    currentMenu:addObject(multiplayer)

    workshop = Text.new(20, multiplayer.y + multiplayer.height + math.floor(40 * getHeightScale()), "Arial.fnt", "Workshop", math.floor(42 * getHeightScale()))

    currentMenu:addObject(workshop)

    settings = Text.new(20, workshop.y + workshop.height + math.floor(40 * getHeightScale()), "Arial.fnt", "Settings", math.floor(42 * getHeightScale()))

    currentMenu:addObject(settings)

    exit = Text.new(20, settings.y + settings.height + math.floor(40 * getHeightScale()), "Arial.fnt", "Exit", math.floor(42 * getHeightScale()))

    currentMenu:addObject(exit)

    texts = {songs, multiplayer, workshop, settings, exit}
end

function keyPress(data)
    if data == 257 then -- enter
        if selection == 1 then -- songs
            switchTo("Scripts/SongSelect.lua")
        end
        if selection == 2 and isSteamRunning() then -- multiplayer
            
        end
        if selection == 3 and isSteamRunning() then -- workshop
            switchTo("Scripts/Workshop.lua")
        end
        if selection == 4 then -- settings
            switchTo("Scripts/Settings.lua")
        end
        if selection == 5 then -- exit
            quitGame()
        end
    else
        texts[selection].text = string.sub(texts[selection].text, 3, string.len(texts[selection].text));

        if data == 265 then -- up

            selection = selection - 1

            if selection < 1 then
                selection = 4
            end

            
        end

        if data == 264 then -- down
            selection = selection + 1

            if selection > 5 then
                selection = 1
            end
        end

        texts[selection].text = "> " .. texts[selection].text;
    end
end

function draw()

end