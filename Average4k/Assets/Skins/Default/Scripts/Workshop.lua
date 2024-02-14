local logo = nil

function create()
    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())
end

function keyPress(data)

    -- escape

    if data == 256 then
        switchTo("Scripts/MainMenu.lua")
    end
end