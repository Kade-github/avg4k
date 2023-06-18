checkbox = {}

checkbox.checkboxes = {}

--[[
container - the container that the checkbox will be added to [if nil, it will be added to the screen parent] (gameObject)


_setting - the setting that the checkbox will change (string)

_tag - the tag of the checkbox (string)

tinyPos - the position of the checkbox (table of 2 numbers. x and y)

_changeFunction - the function that will be called when the checkbox is changed (function)
]]

function checkbox.CreateCheckbox(container, _setting, _tag, tinyPos, _changeFunction, informationText)
    -- create a table, kind of like a class type thing.
    -- it's pretty simple.

    local bTable = {
        currentValue = settings[_setting],
        tag = _tag,
        setting = _setting,
        objects = {},
        changeFunction = _changeFunction,
        position = rect.new(tinyPos[1], tinyPos[2]),
        startTime = 0,
        startPos = 0,
        hoverTime = -1,
        lastHover = -1,
        setRect = false
    }

    local background = helper.createSprite("Menu/MainMenu/Settings/toggle_bg", 0.0, 0.0)
    if container ~= nil then
        create(background)
        container:add(background)
    else
        add(background)
    end
    background.tag = "CHECKBOX_BG_" .. _tag
    background.ratio = true
    background.order = 1
    background.transform.scale = skin["upscale"]


    local toggle = helper.createSprite("Menu/MainMenu/Settings/toggle", 0.0, 0.0)
    create(toggle)
    background:add(toggle)
    toggle.tag = "CHECKBOX_TOGGLE_" .. _tag
    toggle.ratio = true
    toggle.order = 1
    toggle.transform.w = 0.5
    toggle.transform.h = 1

    local text = text.new(0, 0, "Arial.fnt", bTable.setting)
    if container ~= nil then
        create(text)
        container:add(text)
    else
        add(text)
    end
    text.size = 18 * skin["upscale"]
    text.characterSpacing = 2.33
    text.ratio = true
    text.order = 1
    text.transform.x = -1
    text.transform.y = 0.18
    text.tag = "CHECKBOX_TEXT_" .. _tag

    local oText = text.new(0, 0, "ArialBold.fnt", "off")
    create(oText)
    background:add(oText)
    oText.size = 14 * skin["upscale"]
    oText.characterSpacing = 1.67
    oText.ratio = true
    oText.order = 1
    oText.tag = "CHECKBOX_STATUS_" .. _tag

    if bTable.currentValue then
        oText.text = "on"
    else
        oText.text = "off"
    end

    oText.center = true

    oText.transform.r = 13
    oText.transform.g = 28
    oText.transform.b = 64


    table.insert(bTable.objects, background)
    table.insert(bTable.objects, toggle)
    table.insert(bTable.objects, text)
    table.insert(bTable.objects, oText)

    if informationText ~= "" then
        local infoText = text.new(0, 0, "ArialBold.fnt", informationText)
        if container ~= nil then
            create(infoText)
            container:add(infoText)
        else
            add(infoText)
        end
        infoText.size = 15 * skin["upscale"]
        infoText.characterSpacing = 1
        infoText.order = 1
        infoText.tag = "CHECKBOX_INFO_" .. _tag
        table.insert(bTable.objects, infoText)
    end

    table.insert(checkbox.checkboxes, bTable)
end

function checkbox.checked(cb)
    cb.startPos = cb.position.x
    cb.currentValue = not cb.currentValue
    if cb.currentValue then
        cb.objects[4].text = "on"
    else
        cb.objects[4].text = "off"
    end
    cprint("[Settings] " .. cb.tag .. " = " .. tostring(cb.currentValue))
    cb.startTime = Globals.lt
    if cb.changeFunction ~= nil then
        cb.changeFunction(cb.setting, cb.currentValue)
    end
end

function checkbox.mouseDown(pos)
    for i, cb in ipairs(checkbox.checkboxes) do
        if helper.aabb(pos, cb.objects[1]:getRealRect()) then
            checkbox.checked(cb)
            return
        end
    end
end

function checkbox.update(time)
    for i, cb in ipairs(checkbox.checkboxes) do
        if not cb.setRect then
            local x, y = cb.position.x, cb.position.y
            cb.position = copyRect(cb.objects[1].transform)
            cb.position.x = x
            cb.position.y = y
            cb.position.w = cb.objects[1].transform.w / cb.objects[1].parent.transform.w
            cb.position.h = cb.objects[1].transform.h / cb.objects[1].parent.transform.h
            cb.setRect = true
        end
        cb.objects[1].transform = cb.position
        cb.objects[1].transform.x = cb.position.x + 0.01 + cb.objects[3].transform.w
        cb.objects[1].transform.y = cb.position.y

        -- simple tweening
        local tweenTime = math.min((time - cb.startTime) / 0.1, 1)
        if not cb.currentValue then
            cb.objects[2].transform.x = helper.lerp(cb.startPos, 0, tweenTime)
            cb.objects[4].transform.x = 0.95 - (cb.objects[4].transform.w / 2)
        else
            local endPos = 1 - cb.objects[2].transform.w
            cb.objects[2].transform.x = helper.lerp(cb.startPos, endPos,
                tweenTime)
            cb.objects[4].transform.x = (cb.objects[4].transform.w / 2) + 0.1
        end
        cb.objects[4].transform.y = 0.5

        cb.objects[3].transform.y = cb.position.y + ((cb.objects[1].transform.h * skin["upscale"]) / 2) -
            ((cb.objects[3].transform.h * skin["upscale"]) / 2)
        cb.objects[3].transform.x = cb.position.x
        -- information text, animation as well
        if cb.objects[5] ~= nil then
            local ht = time - cb.hoverTime
            -- create a bigger hitbox for text
            local endPosReal = cb.objects[3]:getRealRect().y + cb.objects[3]:getRealRect().h +
                cb.objects[5]:getRealRect()
                .h - textbox.container:getRealRect().y
            cb.objects[5].transform.x = cb.objects[3]:getRealRect().x - textbox.container:getRealRect().x

            local bigRect = copyRect(cb.objects[4].transform)
            bigRect.y = endPosReal
            bigRect.w = bigRect.w + 15

            local realYPos = cb.objects[3]:getRealRect().y - textbox.container:getRealRect().y


            local thingRect = copyRect(cb.objects[1]:getRealRect())

            thingRect.y = thingRect.y + cb.objects[1].transformOffset.y

            local oRect = copyRect(cb.objects[3]:getRealRect())

            -- check all of these hit boxes because we need it to be ux friendly
            if helper.aabb(Globals.mouseRect, thingRect) or helper.aabb(Globals.mouseRect, oRect) then
                local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                local y = helper.lerp(realYPos, endPosReal, helper.outCubic(a))
                cb.objects[5].transform.y = y
                cb.objects[5].transform.alpha = a
                cb.lastHover = time
            else
                local a = helper.lerp(1, 0, math.min((time - cb.lastHover) / 0.5, 1))
                local y = helper.lerp(realYPos,
                    endPosReal, helper.outCubic(a))
                cb.hoverTime = time
                cb.objects[5].transform.y = y
                cb.objects[5].transform.alpha = a
            end
        end
    end
end

function checkbox.GetCheckbox(_tag)
    for i, cb in ipairs(checkbox.checkboxes) do
        if cb.tag == _tag then
            return cb
        end
    end
    return nil
end
