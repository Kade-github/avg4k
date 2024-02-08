logo = nil

currentView = 0

packs = {}
currentPack = 1

currentCharts = {}

currentSong = nil

packSelection = 1
selection = 1

reloadFrameCounter = 0
justReloaded = false

packObjects = {}
chartObjects = {}

selectedChart = {}
selectedDiff = 1
chartObject = {}

waitingForSong = false

function sortPack(a,b)
  return a["name"] < b["name"]
end

function sortChart(a,b)
    return a["title"] < b["title"]
end

function resetAll()
    currentMenu:removeAll()
    clearAsync()
    packObjects = {}
    chartObjects = {}
    chartObject = {}
end

function createPacks()
    resetAll()
    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    local goBack = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "ArialUnicode.fnt", "Press escape to go back", math.floor(42 * getHeightScale()))

    currentMenu:addObject(goBack)

    for i = 1, #packs do
        local p = packs[i]
        local pack = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()) + (i * math.floor(40 * getHeightScale())), "ArialUnicode.fnt", packs[i].name, math.floor(42 * getHeightScale()))

        local bannerId = -1

        if p["banner"] ~= "" then
            bannerId = loadAsyncTexture(p["path"] .. "/" .. p["banner"])
        end

        packObjects[i] = { text = pack, name = packs[i].name, banner = nil, bId = bannerId, id = i }

        currentMenu:addObject(pack)
    end
end

function createCharts()
    if #currentCharts == 0 then
        currentView = 0
        return
    end
    resetAll()

    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    local goBack = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "ArialUnicode.fnt", "Press escape to go back", math.floor(42 * getHeightScale()))

    currentMenu:addObject(goBack)

    for i = 1, #currentCharts do
        local p = currentCharts[i]

        local pack = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()) + (i * math.floor(40 * getHeightScale())), "ArialUnicode.fnt", "whatever", math.floor(42 * getHeightScale()))

        chartObjects[i] = { text = pack, name = p["title"], banner = nil, bId = -1, id = i, background = nil, backgroundId =
        -1, path = p["path"], showBG = false, folder = p["folder"] }

        currentMenu:addObject(pack)
    end
end

function createChart()
    resetAll()

    selectedDiff = 1

    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    local goBack = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "ArialUnicode.fnt", "Press escape to go back", math.floor(42 * getHeightScale()))

    currentMenu:addObject(goBack)

    local rp = selectedChart

    local t = rp["title"]
    if rp["titleTranslit"] ~= "" then
        t = rp["titleTranslit"]
    end
    if t == "" then
        t = "Unknown Title"
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

    local chart = Text.new(20, logo.y + logo.height + math.floor(62 * getHeightScale()), "ArialUnicode.fnt", t, math.floor(42 * getHeightScale()))

    currentMenu:addObject(chart)

    local bId = -1

    if rp["background"] ~= "" then
        bId = loadAsyncTexture(rp["folder"] .. "/" .. rp["background"])
    end

    selectedChart["loaded"] = false

    loadChart(selectedChart["path"]) -- load diffs and stuff

    chartObject = { text = chart, name = t, id = 1, background = nil, backgroundId = bId, diffObjects = {}}
end

function create()
    packs = getPacks()
    table.sort(packs, sortPack)

    createPacks()
end

function findSong()
    local rp = currentCharts[selection]

    if currentSong ~= nil then
        if currentSong.isValid then
            currentSong:Stop()
            currentSong:Destruct()
        end
    else
        clearAudio()
    end

    loadSong(rp["file"], rp["folder"] .. "/" .. rp["file"])

    print("queued song " .. rp["folder"] .. "/" .. rp["file"])

    waitingForSong = true
end

function keyPress(data)

    -- pressed f1

    if data == 290 then -- reload charts
        justReloaded = true
        selection = 1
        packSelection = 1
        currentView = 0
        selectedDiff = 1
        packs = {}
        resetAll()
        scanCharts()
    end

    if data == 265 then -- up

        if currentView == 0 then
            packSelection = packSelection - 1
            if packSelection < 1 then
                packSelection = #packObjects
            end
        elseif currentView == 1 then
            selection = selection - 1
            if selection < 1 then
                selection = #chartObjects
            end
            
            findSong()
        else
            selectedDiff = selectedDiff - 1
            if selectedDiff < 1 then
                selectedDiff = #chartObject.diffObjects
            end
        end
    end

    if data == 264 then -- down

        if currentView == 0 then
            packSelection = packSelection + 1
            if packSelection > #packObjects then
                packSelection = 1
            end
        elseif currentView == 1 then
            selection = selection + 1
            if selection > #chartObjects then
                selection = 1
            end

            findSong()
        else
            selectedDiff = selectedDiff + 1
            if selectedDiff > #chartObject.diffObjects then
                selectedDiff = 1
            end
        end
    end

    -- escape

    if data == 256 then
        if currentView == 0 then
            switchTo("Scripts/MainMenu.lua")
        elseif currentView == 1 then
            currentView = 0
            createPacks()
            selection = 1
        else
            currentView = 1
            table.sort(currentCharts, sortChart)
            createCharts()
        end
    end

    -- enter

    if data == 257 then
        if currentView == 0 then
            currentCharts = getCharts(packs[packSelection]["name"])
            table.sort(currentCharts, sortChart)
            if #currentCharts ~= 0 then
                currentView = 1
                createCharts()
                findSong()
                selection = 1
            end
        elseif currentView == 1 then
            selectedChart = currentCharts[selection]
            currentView = 2
            createChart()
        elseif currentView == 2 then
            fadeOutSong(true)
            playChart(selectedChart["path"], selectedDiff)
        end
    end
end

function view_packs()
    local min = packSelection - 14
    local max = packSelection + 14

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

        local away = i - packSelection

        if away == 0 then
            p.text.text = "> " .. p.name .. " <"
        else
            p.text.text = p.name
        end

        p.text.y = logo.y + logo.height + math.floor(300 * getHeightScale()) + (away * math.floor(40 * getHeightScale()))

        if p.text.y < logo.y + logo.height + math.floor(64 * getHeightScale()) then
            p.text.y = -1000
        end

        if p.text.y > math.floor(900 * getHeightScale()) then
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
        local rp = currentCharts[i]

        if i >= min and i <= max then
            if p.bId == -1 then
                p.bId = loadAsyncTexture(p.folder .. "/" .. rp["banner"])
            end
        else
            p.text.text = p.name
            p.text.y = -1000
            if p.banner ~= nil then
                p.banner.y = -1000
            end

            if p.banner ~= nil then
                currentMenu:removeObject(p.banner)
            end
            p.banner = nil
            p.bId = -1

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
        if t == "" then
            t = "Unknown Title"
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

        p.text.y = logo.y + logo.height + math.floor(300 * getHeightScale()) + (away * math.floor(40 * getHeightScale()))

        if p.text.y < logo.y + logo.height + math.floor(64 * getHeightScale()) then
            p.text.y = -1000
        end

        if p.text.y > math.floor(900 * getHeightScale()) then
            p.text.y = -1000
        end

        if p.banner ~= nil then
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)
            p.banner.x = p.text.x + p.text.width + 20
        end
    end
end

function view_chart()

    local min = selectedDiff - 5
    local max = selectedDiff + 5

    if min < 1 then
        min = 1
    end

    if max > #chartObject.diffObjects then
        max = #chartObject.diffObjects
    end

    -- set view
    for i = 1, #chartObject.diffObjects do
        local p = chartObject.diffObjects[i]

        if i >= min and i <= max then
            -- nada
        else
            p.text.text = p.name
            p.text.y = -1000
        end
    end

    for i = min, max do
        local p = chartObject.diffObjects[i]

        local away = i - selectedDiff

        if away == 0 then
            p.text.text = "> " .. p.name .. " <"
        else
            p.text.text = p.name
        end

        p.text.y = logo.y + logo.height + math.floor(300 * getHeightScale()) + (away * math.floor(40 * getHeightScale()))

        if p.text.y < logo.y + logo.height + math.floor(64 * getHeightScale()) then
            p.text.y = -1000
        end

        if p.text.y > math.floor(900 * getHeightScale()) then
            p.text.y = -1000
        end
    end

end

function lerp(a,b,t) 
    return a * (1-t) + b * t 
end

function draw()
    if justReloaded then
        reloadFrameCounter = reloadFrameCounter + 1
        if reloadFrameCounter > 30 then -- wait 30ish frames, probably bad but whatever
            reloadFrameCounter = 0
            print("Reloaded!")
            justReloaded = false
            packs = getPacks()
            table.sort(packs, sortPack)

            createPacks()
        end
        return
    end


    if currentView == 0 then
        view_packs()
    elseif currentView == 1 then
        view_charts()
    else
        view_chart()
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
            p.banner.height = math.floor(38 * getHeightScale())
            p.banner.width = math.floor(200 * getWidthScale())
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)

            currentMenu:addObject(p.banner)
        end
    end


    -- async callbacks

    for i = 1, #chartObjects do
        local p = chartObjects[i]
        if p.banner == nil and p.bId ~= -1 then
            local spr = getAsyncTexture(p.bId)

            if not (spr.width == 0) and not (spr.height == 0) then -- doesn't exist yet
                p.banner = spr
                p.banner.x = p.text.x + p.text.width + 20
                p.banner.height = math.floor(38 * getHeightScale())
                p.banner.width = math.floor(200 * getWidthScale())
                p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)

                currentMenu:addObject(p.banner)
            end
        end
    end

    if waitingForSong then
        currentSong = getAsyncSong()

        if currentSong.isValid then
            local rp = currentCharts[selection]
            waitingForSong = false
            currentSong.volume = 0
            currentSong.time = rp["previewStart"]
            currentSong:Play(false)
            currentSong:Repeat(true)
        end
    end

    if currentView == 2 then
        if chartObject.background == nil and chartObject.backgroundId ~= -1 then
            local spr = getAsyncTexture(chartObject.backgroundId)

            if not (spr.width == 0) and not (spr.height == 0) then -- doesn't exist yet
                chartObject.background = spr
                chartObject.background.zIndex = -1
                chartObject.background.x = 0
                chartObject.background.y = 0
                chartObject.background.width = math.floor(1920 * getWidthScale())
                chartObject.background.height = math.floor(1080 * getHeightScale())
                chartObject.background.alpha = 0.0
                currentMenu:addObject(chartObject.background)
            end
        end

        if chartObject.background ~= nil then
            chartObject.background.alpha = lerp(chartObject.background.alpha, 0.5, 0.1)
        end

        if not selectedChart["loaded"] then
            local c = checkChart()

            local t = c["title"]

            if t == "" then
                t = "Unknown Title"
            end

            local ot = selectedChart["title"]

            if ot == "" then
                ot = "Unknown Title"
            end

            if t ~= selectedChart["title"] then
                return
            end

            selectedChart = c

            if selectedChart["difficulties"] == nil then
                selectedChart["loaded"] = true
                return
            end

            if #selectedChart["difficulties"] == 0 then
                selectedChart["loaded"] = true
                return
            end

            selectedChart["loaded"] = true

            print("Loaded diffs " .. tostring(#selectedChart["difficulties"]) .. " for " .. selectedChart["title"])

            -- create difficulties

            for i = 1, #selectedChart["difficulties"] do
                local d = selectedChart["difficulties"][i]

                local t = d["name"]

                if d["charter"] ~= "" then
                    t = t .. " charted by " .. d["charter"]
                end
                
                t = t .. " (Rating: " .. d["rating"] .. ")"

                local diff = Text.new(20, 500 + (i * 40), "ArialUnicode.fnt", t, math.floor(42 * getHeightScale()))

                chartObject.diffObjects[i] = { text = diff, name = t, id = i }

                currentMenu:addObject(diff)
            end

            
        end
    end

    -- song fade in

    if currentSong ~= nil then
        if currentSong.isValid then
            currentSong.volume = lerp(currentSong.volume, 0.6, 0.08)
        end
    end
end