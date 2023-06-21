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
        currentValue = false,
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

    if settings[_setting] == "true" then
        bTable.currentValue = true
    else
        bTable.currentValue = false
    end

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

    if not bTable.currentValue then
        bTable.objects[2].transform.x = 0
        bTable.objects[4].transform.x = 0.95 - (bTable.objects[4].transform.w / 2)
    else
        local endPos = 1 - bTable.objects[2].transform.w
        bTable.objects[2].transform.x = endPos
        bTable.objects[4].transform.x = (bTable.objects[4].transform.w / 2) + 0.1
    end

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
    cprint("[Settings] " .. cb.setting .. " = " .. tostring(cb.currentValue))
    cb.startTime = Globals.lt
    if cb.changeFunction ~= nil then
        cb.changeFunction(cb.setting, cb.currentValue)
    end
end

function checkbox.mouseDown(pos)
    for i, cb in ipairs(checkbox.checkboxes) do
        local realerRect = copyRect(cb.objects[1]:getRealRect())
        realerRect.y = realerRect.y + cb.objects[1].transformOffset.y
        if helper.aabb(pos, realerRect) then
            checkbox.checked(cb)
            return
        end
    end
end

function checkbox.update(time)
    for i, cb in ipairs(checkbox.checkboxes) do
        local bg = cb.objects[1]
        local toggle = cb.objects[2]
        local text = cb.objects[3]
        local status = cb.objects[4]
        local info = cb.objects[5]
        local realProg = (time - cb.startTime) / 0.1
        local tweenTime = math.min(realProg, 1)
        if not cb.setRect and text.transform.w ~= 0 and status.transform.w ~= 0 then
            local x, y = cb.position.x, cb.position.y
            cb.position = copyRect(bg.transform)
            cb.position.x = x
            cb.position.y = y
            cb.position.w = bg.transform.w / bg.parent.transform.w
            cb.position.h = bg.transform.h / bg.parent.transform.h
            cb.setRect = true
            bg.transform.x = cb.position.x + 0.01 + text.transform.w
            bg.transform.y = cb.position.y
            text.transform.x = cb.position.x
            status.transform.y = 0.5
            text.transform.y = cb.position.y + ((bg.transform.h * skin["upscale"]) / 2) -
                ((text.transform.h * skin["upscale"]) / 2)

            if not cb.currentValue then
                toggle.transform.x = 0
                status.transform.x = 0.95 - (status.transform.w / 2)
            else
                toggle.transform.x = 1 - toggle.transform.w
                status.transform.x = (status.transform.w / 2) + 0.1
            end
            realProg = 2
        end
        if cb.setRect then
            -- simple tweening
            if realProg < 1.1 then
                if not cb.currentValue then
                    toggle.transform.x = helper.lerp(cb.startPos, 0, tweenTime)
                    status.transform.x = 0.95 - (status.transform.w / 2)
                else
                    local endPos = 1 - toggle.transform.w
                    toggle.transform.x = helper.lerp(cb.startPos, endPos,
                        tweenTime)
                    status.transform.x = (status.transform.w / 2) + 0.1
                end
            end

            -- information text, animation as well
            if info ~= nil then
                local ht = time - cb.hoverTime

                info.transform.x = text:getRealRect().x - textbox.container:getRealRect().x

                local thingRect = copyRect(bg:getRealRect())

                thingRect.y = thingRect.y + bg.transformOffset.y

                local oRect = copyRect(text:getRealRect())

                -- check all of these hit boxes because we need it to be ux friendly
                if helper.aabb(Globals.mouseRect, thingRect) or helper.aabb(Globals.mouseRect, oRect) then
                    local endPosReal = text:getRealRect().y + text:getRealRect().h +
                        info:getRealRect()
                        .h - textbox.container:getRealRect().y
                    local realYPos = text:getRealRect().y - textbox.container:getRealRect().y
                    local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                    local y = helper.lerp(realYPos, endPosReal, helper.outCubic(a))
                    info.transform.y = y
                    info.transform.alpha = a
                    cb.lastHover = time
                else
                    local endPosReal = text:getRealRect().y + text:getRealRect().h +
                        info:getRealRect()
                        .h - textbox.container:getRealRect().y
                    local realYPos = text:getRealRect().y - textbox.container:getRealRect().y
                    local a = helper.lerp(1, 0, math.min((time - cb.lastHover) / 0.5, 1))
                    local y = helper.lerp(realYPos,
                        endPosReal, helper.outCubic(a))
                    cb.hoverTime = time
                    info.transform.y = y
                    info.transform.alpha = a
                end
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
