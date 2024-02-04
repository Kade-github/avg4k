songs = nil
multiplayer = nil
settings = nil
exit = nil

texts = nil

selection = 1

function create()
    print("hello :)")
    local s = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(s)

    songs = Text.new(20, s.y + s.height + 20, "Arial.fnt", "> Songs", 42)

    currentMenu:addObject(songs)

    multiplayer = Text.new(20, songs.y + songs.height + 40, "Arial.fnt", "Multiplayer", 42)

    currentMenu:addObject(multiplayer)

    settings = Text.new(20, multiplayer.y + multiplayer.height + 40, "Arial.fnt", "Settings", 42)

    currentMenu:addObject(settings)

    exit = Text.new(20, settings.y + settings.height + 40, "Arial.fnt", "Exit", 42)

    currentMenu:addObject(exit)

    texts = {songs, multiplayer, settings, exit}
end

function keyPress(data)
    if data == 257 then -- enter
        if selection == 1 then -- songs
            switchTo("Scripts/SongSelect.lua")
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

            if selection > 4 then
                selection = 1
            end
        end

        texts[selection].text = "> " .. texts[selection].text;
    end
end

function draw()

end