function create()
    local s = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(s)

    local songs = Text.new(20, s.y + s.height + 20, "Arial.fnt", "> Songs", 24)

    currentMenu:addObject(songs)
end