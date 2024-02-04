charts = {}
chartTexts = {}
chartBanners = {}

logo = nil

selection = 1

function loadCharts()
    -- clear the charts table
    charts = {}
    for i = 1, #chartTexts do
        currentMenu:removeObject(chartTexts[i])
        if chartBanners[i] ~= nil then
            currentMenu:removeObject(chartBanners[i])
        end
    end

    chartTexts = {}
    chartBanners = {}

    collectgarbage("collect")

    scanCharts();

end

function create()
    logo = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(logo)

    loadCharts()
end

function keyPress(data)
    chartTexts[selection].text = string.sub(chartTexts[selection].text, 3, string.len(chartTexts[selection].text));

    if data == 265 then -- up

        selection = selection - 1

        if selection < 1 then
            selection = #chartTexts
        end

        
    end

    if data == 264 then -- down
        selection = selection + 1

        if selection > #chartTexts then
            selection = 1
        end
    end

    chartTexts[selection].text = "> " .. chartTexts[selection].text;
end


function draw()
    local min = selection - 6
    local max = selection + 6

    if min < 1 then
        min = 1
    end

    if max > #chartTexts then
        max = #chartTexts
    end

    for i = 1, #chartTexts do
        local text = chartTexts[i]
        local banner = chartBanners[i]

        if i < min or i > max then
            text.x = -100
            text.y = -100
            if banner ~= nil then
                banner.x = -100
                banner.y = -100
            end
        else

            local diff = math.abs(i - selection)

            local behind = i < selection

            local e = 100 / (diff)

            if e > 100 then
                e = 100
            end

            if e < -1000 then
                e = -1000
            end

            e = math.floor(e)

            text.x = e
            text.x = text.x
            text.y = logo.y + logo.height + 20
            text.y = 500 + math.floor((text.y * diff) * 0.2)
            if behind then
                text.y = 550 - math.floor((text.y * diff) * 0.2)
            end
            if banner ~= nil then
                banner.width = 200
                banner.height = 50
                banner.x = text.x + text.width + 6
                banner.y = text.y + (text.height / 2) - (banner.height / 2)
            end
        end
    end
end

function chartFound(c)
    local text = c["title"] .. " by " .. c["artist"]
    if c["artist"] == "" then
        text = c["title"]
    end

    if c["subtitle"] ~= "" then
        text = text .. " (" .. c["subtitle"] .. ")"
    end

    local t = Text.new(-100, -100, "ArialUnicode.fnt", text, 42)
    currentMenu:addObject(t)
    local banner = nil

    

    table.insert(chartTexts, t)
    if banner ~= nil then
        table.insert(chartBanners, banner)
    else
        table.insert(chartBanners, {})
    end
end