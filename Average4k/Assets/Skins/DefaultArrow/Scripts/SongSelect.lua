charts = {}
chartObjects = {}

function loadCharts()
    -- clear the charts table
    charts = {}
    for i = 1, #chartObjects do
        currentMenu:removeObject(chartObjects[i])
    end

    chartObjects = {}

    collectgarbage("collect")

    scanCharts();

end

function create()
    local s = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(s)

    loadCharts()
end

function chartFound(c)
    local text = c["path"]
    local t = Text.new(20, 100, "Arial.fnt", text, 42)
    currentMenu:addObject(t)

    if #chartObjects > 0 then
        t.y = chartObjects[#chartObjects].y + chartObjects[#chartObjects].height + 44
    end

    table.insert(chartObjects, t)
end