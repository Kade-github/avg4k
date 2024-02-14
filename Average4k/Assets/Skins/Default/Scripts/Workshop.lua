local logo = nil
local back = nil
local workshop = nil

local view = {}

local selection = 1

function createView(list)
    currentMenu:removeAll()

    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    back = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "Arial.fnt", "Press escape to go back", math.floor(42 * getHeightScale()))

    currentMenu:addObject(back)

    workshop = Text.new(20, back.y + back.height + math.floor(42 * getHeightScale()), "Arial.fnt", "Average4K Workshop", math.floor(52 * getHeightScale()))

    currentMenu:addObject(workshop)

    view = {}
    for i = 1, #list do
        local t = Text.new(20, (workshop.y + workshop.height + (40 * getHeightScale())) + (i - 1) * (60 * getHeightScale()), "Arial.fnt", list[i], (42 * getHeightScale()))

        table.insert(view, t)

        currentMenu:addObject(t)
    end

    selection = 1

    view[selection].text = "> " .. view[selection].text
end

function create()
    createView({"Upload a Pack", "Upload a Noteskin", "Upload a Theme"})
end

function up()
    view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text))

    selection = selection - 1

    if selection < 1 then
        selection = #view
    end

    view[selection].text = "> " .. view[selection].text
end

function down()
    view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text))

    selection = selection + 1

    if selection > #view then
        selection = 1
    end

    view[selection].text = "> " .. view[selection].text
end

function keyPress(data)
    -- escape

    if data == 256 then
        switchTo("Scripts/MainMenu.lua")
    end
    
    -- up

    if data == 265 then
        up()
    end

    -- down

    if data == 264 then
        down()
    end
end