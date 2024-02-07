videoSettings = nil
gameplaySettings = nil
skinSettings = nil
audioSettings = nil

currentView = 0

function create()
    local s = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(s)

    local goBack = Text.new(20, logo.y + logo.height + 20, "ArialUnicode.fnt", "Press escape to go back", 42)

    currentMenu:addObject(goBack)
    
end

function keyPress(data)
    -- escape

    if data == 256 then
        if currentView == 0 then
            switchTo("Scripts/MainMenu.lua")
        end
    end
end