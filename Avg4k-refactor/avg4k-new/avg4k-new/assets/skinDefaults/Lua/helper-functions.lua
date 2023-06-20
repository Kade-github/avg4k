helper = {}

helper.containers = {}

-- round helper
function helper.round(num, numDecimalPlaces)
    local mult = 10 ^ (numDecimalPlaces or 0)
    return math.floor(num * mult + 0.5) / mult
end

function helper.isNumber(input)
    local num = tonumber(input)
    return num ~= nil and type(num) == "number" and tostring(num) == input
end

-- dumps lua tables into a string so you can print usertables (like sprite, animatedsprite, etc)
function helper.dump(o)
    if type(o) == 'table' then
        local s = '{ '
        for k, v in pairs(o) do
            if type(k) ~= 'number' then k = '"' .. k .. '"' end
            s = s .. '[' .. k .. '] = ' .. helper.dump(v) .. ','
        end
        return s .. '} '
    else
        return tostring(o)
    end
end

function helper.split(pString, pPattern)
    local Table = {} -- NOTE: use {n = 0} in Lua-5.0
    local fpat = "(.-)" .. pPattern
    local last_end = 1
    local s, e, cap = pString:find(fpat, 1)
    while s do
        if s ~= 1 or cap ~= "" then
            table.insert(Table, cap)
        end
        last_end = e + 1
        s, e, cap = pString:find(fpat, last_end)
    end
    if last_end <= #pString then
        cap = pString:sub(last_end)
        table.insert(Table, cap)
    end
    return Table
end

-- there is aabb_rect but it is slow since it's a cpp call.
function helper.aabb(r1, r2)
    return r1.x < r2.x + r2.w and
        r1.x + r1.w > r2.x and
        r1.y < r2.y + r2.h and
        r1.y + r1.h > r2.y
end

-- there is a lerp function on cpp, but its a cpp call so it's slow
function helper.lerp(a, b, t)
    return a + (b - a) * t
end

function helper.outCubic(t)
    t = t - 1
    return t * t * t + 1
end

--[[
    A helper function to create a sprite
]]
function helper.createSprite(path, x, y)
    local tempTexture = texture.new(path)
    loadTexture(tempTexture)
    return sprite.new(x, y, tempTexture)
end

--[[
    A helper function to create a sprite with a texture
]]
function helper.createSprite(path, x, y, texture)
    return sprite.new(x, y, texture)
end

--[[
    A helper function to create an animated sprite
]]
function helper.createAnimatedSprite(path, x, y, fps, frameW, frameH, parent)
    local tempTexture = texture.new(path)
    loadTexture(tempTexture)
    local s = animatedSprite.new(x, y, tempTexture)
    if parent ~= nil then
        parent:add(s)
    else
        add(s)
    end
    s.fps = fps
    s:setFrameSize(frameW, frameH)
    return s
end

--[[
    A helper function to create an animated sprite
]]
function helper.createAnimatedSpriteSkin(path, x, y, fps, frameW, frameH, parent)
    local tempTexture = texture.new(path)
    loadArrowTexture(tempTexture)
    local s = animatedSprite.new(x, y, tempTexture)
    if parent ~= nil then
        parent:add(s)
    else
        add(s)
    end
    s.fps = fps
    s:setFrameSize(frameW, frameH)
    return s
end

--[[
    A helper function to create a chart sprite
]]
function helper.createChartSprite(path, x, y)
    local tempTexture = texture.new(path)
    loadChartTexture(tempTexture)
    return sprite.new(x, y, tempTexture)
end

--[[
    A helper function to create a sprite with a container texture
]]
function helper.createContainer(path, x, y)
    local tempTexture = texture.new("Menu/MainMenu/" .. path)
    loadTexture(tempTexture)
    return sprite.new(x, y, tempTexture)
end

--[[
    A helper function to initialize a container.
    Don't use this in gameplay
]]
function helper.initContainer(container, allowScroll)
    -- add it to the container list (with some extra variables)
    local c = {}
    c.c = container
    c.scroll = 0
    c.shouldScroll = false
    c.hover = false
    c.overspace = 0
    c.allowScroll = allowScroll
    c.tweenRect = nil
    c.startTween = 0
    c.endTween = 0
    table.insert(helper.containers, { c })

    -- create a scroll bar
    -- its actually made up of 3 things (technically two), the bar, and the two arrows (one is rotated 180 degrees)

    local bar = rectangle.new(0, 0, 18 * skin["upscale"], 60)
    create(bar)
    container:add(bar)
    bar.order = 1
    bar.transform.alpha = 0

    bar.tag = "container_bar"

    local arrow1 = helper.createSprite("Menu/MainMenu/Container/scrollbaruparrow", 0, 0)
    create(arrow1)
    container:add(arrow1)
    arrow1.order = 1
    arrow1.transform.alpha = 0

    arrow1.tag = "container_arrow_1"

    local arrow2 = helper.createSprite("Menu/MainMenu/Container/scrollbaruparrow", 0, 0)
    create(arrow2)
    container:add(arrow2)
    arrow2.order = 1
    arrow2.transform.alpha = 0

    arrow2.tag = "container_arrow_2"
end

function helper.findContainer(container)
    for i, t in ipairs(helper.containers) do
        local ind = t[1]
        if ind["c"].tag == container then
            return ind
        end
    end
    return nil
end

--[[
    A helper function for mouse wheel events on containers
]]
function helper.containerMouseWheel(amount)
    for i, t in ipairs(helper.containers) do
        local ind = t[1]
        if ind["hover"] and ind["shouldScroll"] then
            ind["scroll"] = ind["scroll"] - (amount * 30)

            if ind["scroll"] >= ind["overspace"] then
                ind["scroll"] = ind["overspace"]
            end

            if ind["scroll"] < 0 then
                ind["scroll"] = 0
            end
        end
    end
end

function helper.setTween(container_name, length, rect)
    local ind = helper.findContainer(container_name)
    if ind ~= nil then
        ind["tweenRect"] = rect
        ind["startTween"] = length
        ind["endTween"] = length
    end
end

--[[
    A helper function to update containers scroll bar and item positions
]]
function helper.containerUpdate(time)
    for i, t in ipairs(helper.containers) do
        local ind = t[1]

        local container = ind["c"]

        if ind["tweenRect"] ~= nil then
            if container.transform.x ~= ind["tweenRect"].x then
                container.transform.x = helper.lerp(container.transform.x, ind["tweenRect"].x,
                    helper.outCubic(ind["endTween"] / (time - ind["startTween"])))
            end

            if container.transform.y ~= ind["tweenRect"].y then
                container.transform.y = helper.lerp(container.transform.y, ind["tweenRect"].y,
                    helper.outCubic(ind["endTween"] / (time - ind["startTween"])))
            end
        end

        local rc = container:getRealRect()

        -- check hover using aabb
        ind["hover"] = helper.aabb(Globals.mouseRect, rc)

        -- set the clip

        container.clip.x = rc.x + container.transformOffset.x - 2
        container.clip.y = rc.y + container.transformOffset.y - 2
        container.clip.w = rc.w + container.transformOffset.w + 4
        container.clip.h = rc.h + container.transformOffset.h + 4

        if ind["allowScroll"] then
            -- get the bar and arrows
            local bar = container:getChildByTag("container_bar")
            local arrow1 = container:getChildByTag("container_arrow_1")
            local arrow2 = container:getChildByTag("container_arrow_2")

            local currentScroll = ind["scroll"]

            local table = container.children
            ind["overspace"] = 0
            for i = 1, table:size(), 1 do
                local child = table[i]
                if child.id == bar.id or child.id == arrow1.id or child.id == arrow2.id then
                    -- do nothing
                else
                    local real = child:getRealRect()

                    if child.ratio then
                        if child.transform.y + child.transform.h > 1 then
                            ind["overspace"] = (rc.h * (child.transform.y + child.transform.h)) - rc.h +
                                (8 * skin["upscale"])
                        end
                    else
                        if child.transform.y + child.transform.h > rc.y + rc.h then
                            ind["overspace"] = (real.y + real.h) - (rc.y - rc.h) + (8 * skin["upscale"])
                        end
                    end
                    child.transformOffset.y = -currentScroll
                    if ind["shouldScroll"] then
                        child.transformOffset.x = bar.transform.w + 4
                        if real.x + real.w + child.transformOffset.x > rc.x + rc.w then
                            child.transformOffset.w = -child.transformOffset.x
                        end
                    else
                        child.transformOffset.x = 0
                        child.transformOffset.w = 0
                    end
                end
            end
            ind["shouldScroll"] = ind["overspace"] ~= 0

            if ind["shouldScroll"] then
                -- set the bar and arrows to visible

                bar.transform.alpha = 1
                arrow1.transform.alpha = 1
                arrow2.transform.alpha = 1

                -- set the bar and arrows positions
                bar.transform.x = 4
                bar.transform.y = (8 + arrow1.transform.h) +
                    (rc.h - (16 + (arrow1.transform.h * 2) + bar.transform.h)) * (currentScroll / ind["overspace"])

                --cprint(tostring(bar.transform.y))

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

                bar.transform.h = rc.h / 4
            else
                -- set the bar and arrows to invisible
                bar.transform.alpha = 0
                arrow1.transform.alpha = 0
                arrow2.transform.alpha = 0
            end
        end
    end
end
