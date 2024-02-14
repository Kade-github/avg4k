local logo = nil
local back = nil

function create()
    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    back = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "Arial.fnt", "Press escape to go back", math.floor(42 * getHeightScale()))

    currentMenu:addObject(back)
end

function keyPress(data)

    -- escape

    if data == 256 then
        switchTo("Scripts/MainMenu.lua")
    end
end