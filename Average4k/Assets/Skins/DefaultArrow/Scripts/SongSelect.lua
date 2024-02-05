logo = nil

function dump(o)
    if type(o) == 'table' then
        local s = '{ '
        for k, v in pairs(o) do
            if type(k) ~= 'number' then k = '"' .. k .. '"' end
            s = s .. '[' .. k .. '] = ' .. dump(v) .. ','
        end
        return s .. '} '
    else
        return tostring(o)
    end
end

currentView = 0

packs = {}
currentPack = 1

currentCharts = {}

selection = 1

packObjects = {}
chartObjects = {}

function createPacks()
    currentMenu:removeAll()
    packObjects = {}
    chartObjects = {}

    logo = Sprite.new(20, 20, "Images/Logo.png")

    currentMenu:addObject(logo)

    local goBack = Text.new(20, logo.y + logo.height + 20, "ArialUnicode.fnt", "Press escape to go back", 42)

    currentMenu:addObject(goBack)

    for i = 1, #packs do
        local p = packs[i]
        local pack = Text.new(20, logo.y + logo.height + 20 + (i * 40), "ArialUnicode.fnt", packs[i].name, 42)

        local bannerId = -1

        if p["banner"] ~= "" then
            bannerId = loadAsyncTexture(p["path"] .. "/" .. p["banner"])
        end

        packObjects[i] = { text = pack, name = packs[i].name, banner = nil, bId = bannerId, id = i }

        currentMenu:addObject(pack)
    end
end

function createCharts()
    currentMenu:removeAll()
    packObjects = {}
    chartObjects = {}

    logo = Sprite.new(20, 20, "Images/Logo.png")

    currentMenu:addObject(logo)

    local goBack = Text.new(20, logo.y + logo.height + 20, "ArialUnicode.fnt", "Press escape to go back", 42)

    currentMenu:addObject(goBack)

    for i = 1, #currentCharts do
        local p = currentCharts[i]

        local pack = Text.new(20, logo.y + logo.height + 20 + (i * 40), "ArialUnicode.fnt", "whatvever", 42)

        chartObjects[i] = { text = pack, name = p["title"], banner = nil, bId = -1, id = i, background = nil, backgroundId =
        -1, path = p["path"], showBG = false, folder = p["folder"] }

        currentMenu:addObject(pack)
    end
end

function create()
    packs = getPacks()

    createPacks()
end

function keyPress(data)
    if data == 265 then -- up
        selection = selection - 1

        if currentView == 0 then
            if selection < 1 then
                selection = #packObjects
            end
        else
            if selection < 1 then
                selection = #chartObjects
            end
        end
    end

    if data == 264 then -- down
        selection = selection + 1

        if currentView == 0 then
            if selection > #packObjects then
                selection = 1
            end
        else
            if selection > #chartObjects then
                selection = 1
            end
        end
    end

    -- escape

    if data == 256 then
        if currentView == 0 then
            switchTo("Scripts/MainMenu.lua")
        else
            currentView = 0
            createPacks()
        end
    end

    -- enter

    if data == 257 then
        if currentView == 0 then
            currentView = 1
            currentCharts = getCharts(packs[selection]["name"])
            createCharts()
        else
            -- load chart
        end
    end
end

function view_packs()
    local min = selection - 14
    local max = selection + 14

    if min < 1 then
        min = 1
    end

    if max > #packObjects then
        max = #packObjects
    end

    -- set view
    for i = 1, #packObjects do
        local p = packObjects[i]

        if i >= min and i <= max then
            -- nada
        else
            p.text.text = p.name
            p.text.y = -1000
            if p.banner ~= nil then
                p.banner.y = -1000
            end
        end
    end

    for i = min, max do
        local p = packObjects[i]

        local away = i - selection

        if away == 0 then
            p.text.text = "> " .. p.name .. " <"
        else
            p.text.text = p.name
        end

        p.text.y = 500 + (away * 40)

        if p.text.y < logo.y + logo.height + 64 then
            p.text.y = -1000
        end

        if p.text.y > 900 then
            p.text.y = -1000
        end

        if p.banner ~= nil then
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)
            p.banner.x = p.text.x + p.text.width + 20
        end
    end
end

function view_charts()
    local min = selection - 20
    local max = selection + 20

    if min < 1 then
        min = 1
    end

    if max > #chartObjects then
        max = #chartObjects
    end

    -- set view
    for i = 1, #chartObjects do
        local p = chartObjects[i]

        if i >= min and i <= max then
            if p.bId == -1 then
                print("loading " .. p.folder .. "/banner.png")
                p.bId = loadAsyncTexture(p.folder .. "/banner.png")
            end
        else
            p.text.text = p.name
            p.text.y = -1000
            if p.banner ~= nil then
                p.banner.y = -1000
            end

            if p.banner ~= nil then
                currentMenu:removeObject(p.banner)
                p.bId = -1
            end

            if p.background ~= nil then
                currentMenu:removeObject(p.background)
                p.backgroundId = -1
            end
        end
    end

    for i = min, max do
        local p = chartObjects[i]
        local rp = currentCharts[i]
        local away = i - selection

        local t = rp["title"]
        if rp["titleTranslit"] ~= "" then
            t = rp["titleTranslit"]
        end
        if rp["artistTranslit"] ~= "" then
            t = t .. " by " .. rp["artist"]
        elseif rp["artist"] ~= "" then
            t = t .. " by " .. rp["artist"]
        end
        if rp["subtitleTranslit"] ~= "" then
            t = t .. " (" .. rp["subtitleTranslit"] .. ")"
        elseif rp["subtitle"] ~= "" then
            t = t .. " (" .. rp["subtitle"] .. ")"
        end

        if away == 0 then
            p.text.text = "> " .. t .. " <"
        else
            p.text.text = t
        end

        p.text.y = 500 + (away * 40)

        if p.text.y < logo.y + logo.height + 64 then
            p.text.y = -1000
        end

        if p.text.y > 900 then
            p.text.y = -1000
        end

        if p.banner ~= nil then
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)
            p.banner.x = p.text.x + p.text.width + 20
        end
    end
end

function draw()
    if currentView == 0 then
        view_packs()
    else
        view_charts()
    end

    for i = 1, #packObjects do
        local p = packObjects[i]
        if p.banner == nil and p.bId ~= -1 then
            local spr = getAsyncTexture(p.bId)

            if spr.width == 0 and spr.height == 0 then -- doesn't exist yet
                return
            end

            p.banner = spr
            p.banner.x = p.text.x + p.text.width + 20
            p.banner.height = 38
            p.banner.width = 200
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)

            currentMenu:addObject(p.banner)
        end
    end

    for i = 1, #chartObjects do
        local p = chartObjects[i]
        if p.banner == nil and p.bId ~= -1 then
            local spr = getAsyncTexture(p.bId)

            if spr.width == 0 and spr.height == 0 then -- doesn't exist yet
                return
            end

            p.banner = spr
            p.banner.x = p.text.x + p.text.width + 20
            p.banner.height = 38
            p.banner.width = 200
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)

            currentMenu:addObject(p.banner)
        end

        if p.background == nil and p.backgroundId ~= -1 and p.showBG then
            local spr = getAsyncTexture(p.backgroundId)

            if spr.width == 0 and spr.height == 0 then -- doesn't exist yet
                return
            end

            p.background = spr
            p.background.x = 0
            p.background.y = 0
            p.background.width = 1920
            p.background.height = 1080

            currentMenu:addObject(p.background)
        end
    end
end
