helper = {}

helper.containers = {}

function helper.dump(o)
    if type(o) == 'table' then
       local s = '{ '
       for k,v in pairs(o) do
          if type(k) ~= 'number' then k = '"'..k..'"' end
          s = s .. '['..k..'] = ' .. helper.dump(v) .. ','
       end
       return s .. '} '
    else
       return tostring(o)
    end
 end


--[[
    A helper function to create a sprite
]]
function helper.createSprite(path, x, y)
    local tempTexture = texture.new(path)
    loadTexture(tempTexture)
    return sprite.new(x,y,tempTexture)
end

--[[
    A helper function to create a sprite with a container texture
]]
function helper.createContainer(path, x, y)
    local tempTexture = texture.new("Menu/MainMenu/" .. path)
    loadTexture(tempTexture)
    return sprite.new(x,y,tempTexture)
end

--[[
    A helper function to initialize a container
]]
function helper.initContainer(container)
    -- add it to the container list (with some extra variables)
    local c = {}
    c.c = container
    c.scroll = 0
    c.shouldScroll = false
    table.insert(helper.containers, {c})

    -- create a scroll bar
    -- its actually made up of 3 things (technically two), the bar, and the two arrows (one is rotated 180 degrees)

    local bar = rectangle.new(0,0,18 * skin["upscale"],60)
    create(bar)
    container:add(bar)
    bar.order = 1
    bar.transform.alpha = 0

    bar.tag = "container_bar"

    local arrow1 = helper.createSprite("Menu/MainMenu/Container/scrollbaruparrow", 0,0)
    create(arrow1)
    container:add(arrow1)
    arrow1.order = 1
    arrow1.transform.alpha = 0

    arrow1.tag = "container_arrow_1"

    local arrow2 = helper.createSprite("Menu/MainMenu/Container/scrollbaruparrow", 0,0)
    create(arrow2)
    container:add(arrow2)
    arrow2.order = 1
    arrow2.transform.alpha = 0

    arrow2.tag = "container_arrow_2"
end

--[[
    A helper function to update containers scroll bar and item positions
]]
function helper.containerUpdate(time)
    if Globals.start - time < 2 then
        return
    end
    for i, t in ipairs(helper.containers) do
        local ind = t[1]

        local container = ind["c"]
        local rc = container:getRealRect()

        -- get the bar and arrows
        local bar = container:getChildByTag("container_bar")
        local arrow1 = container:getChildByTag("container_arrow_1")
        local arrow2 = container:getChildByTag("container_arrow_2")

        local currentScroll = ind["scroll"]


        -- overspace
        local overspace = 0

        for i, child in ipairs(container.children) do
            if child.id == bar.id or child.id == arrow1.id or child.id == arrow2.id then
                -- do nothing
            else
                local real = child:getRealRect()
                if real.y + real.h > rc.y + rc.h then
                    overspace = (real.y + real.h) - rc.h
                end
                child.transformOffset.y = currentScroll
                if ind["shouldScroll"] then
                    child.transformOffset.x = bar.transform.w + 8
                else
                    child.transformOffset.x = 0
                end
            end
        end

        ind["shouldScroll"] = overspace ~= 0

        if ind["shouldScroll"] then
            -- set the bar and arrows to visible
            
            bar.transform.alpha = 1
            arrow1.transform.alpha = 1
            arrow2.transform.alpha = 1

            -- set the bar and arrows positions
            bar.transform.x = 4
            bar.transform.y = currentScroll + 4 + arrow1.transform.h

            bar.transform.w = 20 * skin["upscale"]

            arrow1.transform.x = 4
            arrow1.transform.y = 2
            arrow1.transform.w = bar.transform.w
            arrow1.transform.h = 18 * skin["upscale"]
            
            arrow2.transform.x = 4
            arrow2.transform.w = bar.transform.w
            arrow2.transform.h = 18 * skin["upscale"]
            arrow2.transform.y = rc.h - arrow2.transform.h
            arrow2.transform.angle = 180

            bar.transform.h = (rc.h - ((arrow2.transform.h * 2) + 8)) - overspace

        else
            -- set the bar and arrows to invisible
            bar.transform.alpha = 0
            arrow1.transform.alpha = 0
            arrow2.transform.alpha = 0
        end
    end
end