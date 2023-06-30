textbox = {}

textbox.inTextbox = false
textbox.container = nil
textbox.textboxes = {}

function textbox.CreateTextbox(c, _setting, _maxCharacters, _tag, tinyPos, _changeFunction, inc, _keyInput, blacklisted,
                               _infoText)
    textbox.container = c
    local bTable = {
        currentValue = settings[_setting],
        tag = _tag,
        setting = _setting,
        objects = {},
        changeFunction = _changeFunction,
        position = rect.new(tinyPos[1], tinyPos[2]),
        hoverTime = -1,
        lastHover = -1,
        maxCharacters = _maxCharacters,
        increment = inc,
        selected = false,
        keyInput = _keyInput, -- if it should convert the key presses into the name of the key.
        blacklistedKeys = blacklisted,
        setRect = false
    }

    local background = helper.createSprite("Menu/MainMenu/Settings/typeinputcontainer", 0.0, 0.0)
    if c ~= nil then
        create(background)
        c:add(background)
    else
        add(background)
    end
    background.tag = "TEXTBOX_BG_" .. _tag
    background.ratio = true
    background.order = 1
    background.transform.scale = skin["upscale"]

    local text = text.new(0, 0, "Arial.fnt", bTable.setting)
    if c ~= nil then
        create(text)
        c:add(text)
    else
        add(text)
    end
    text.size = 18 * skin["upscale"]
    text.characterSpacing = 2.33
    text.ratio = true
    text.order = 1
    text.transform.y = 0.18
    text.tag = "TEXTBOX_TEXT_" .. _tag

    local oText = text.new(0, 0, "ArialBold.fnt", bTable.currentValue)
    create(oText)
    background:add(oText)
    oText.size = 16 * skin["upscale"]
    oText.characterSpacing = 1.67
    oText.ratio = true
    oText.order = 1
    oText.tag = "TEXTBOX_DATA_" .. _tag
    oText.transform.r = 13
    oText.transform.g = 28
    oText.transform.b = 64
    oText.transform.x = 0.05
    oText.transform.y = 0.25

    table.insert(bTable.objects, background)
    table.insert(bTable.objects, text)
    table.insert(bTable.objects, oText)
    if _infoText ~= "" then
        local infoText = text.new(0, 0, "ArialBold.fnt", _infoText)
        if c ~= nil then
            create(infoText)
            c:add(infoText)
        else
            add(infoText)
        end
        infoText.size = 15 * skin["upscale"]
        infoText.characterSpacing = 1
        infoText.order = 1
        infoText.tag = "TEXTBOX_INFO_" .. _tag
        table.insert(bTable.objects, infoText)
    end
    table.insert(textbox.textboxes, bTable)
end

function textbox.mouseDown(pos)
    if dropdown.inDropdown then
        return
    end
    local isOut = true
    for i, tb in ipairs(textbox.textboxes) do
        local text = tb.objects[3]
        local realerRect = copyRect(tb.objects[1]:getRealRect())
        realerRect.y = realerRect.y + tb.objects[1].transformOffset.y
        if helper.aabb(pos, realerRect) then
            for i, tbb in ipairs(textbox.textboxes) do -- make sure all the other textboxes are not selected
                tbb.selected = false
            end
            tb.selected = true
            textbox.inTextbox = true
            text.text = tb.currentValue .. "_"
            isOut = false
        end
    end
    if isOut then
        for i, tb in ipairs(textbox.textboxes) do
            local text = tb.objects[3]
            if tb.selected then
                tb.changeFunction(tb.setting, tb.currentValue)
                tb.currentValue = settings[tb.setting]
            end
            tb.selected = false
            textbox.inTextbox = false
            text.text = tb.currentValue
        end
    end
end

function textbox.keyDown(key)
    for i, tb in ipairs(textbox.textboxes) do
        local text = tb.objects[3]
        if tb.selected then
            if tb.keyInput then
                for i, v in ipairs(tb.blacklistedKeys) do
                    if v == string.lower(getKeyName(key)) then
                        return
                    end
                end
                local keyy = getKeyName(tonumber(key))
                tb.currentValue = string.upper(keyy)
                tb.selected = false
                textbox.inTextbox = false
                tb.changeFunction(tb.setting, tb.currentValue)
                tb.currentValue = settings[tb.setting]
                text.text = tb.currentValue
            else
                if key == 259 and string.len(tb.currentValue) > 0 then
                    tb.currentValue = tb.currentValue:sub(1, -2)
                    text.text = tb.currentValue .. "_"
                    return
                end
                if key == 257 then
                    tb.selected = false
                    textbox.inTextbox = false
                    tb.changeFunction(tb.setting, tb.currentValue)
                    tb.currentValue = settings[tb.setting]
                    text.text = tb.currentValue
                    return
                end
                if string.len(tb.currentValue) >= tb.maxCharacters then
                    return
                end
                for i, v in ipairs(tb.blacklistedKeys) do
                    if string.lower(v) == string.lower(string.char(key)) then
                        return
                    end
                end
                tb.currentValue = tb.currentValue .. string.char(key)
                text.text = tb.currentValue .. "_"
            end
            return
        end
    end
end

function textbox.update(time)
    for i, tb in ipairs(textbox.textboxes) do
        local bg = tb.objects[1]
        local text = tb.objects[2]
        local oText = tb.objects[3]
        local infoText = tb.objects[4]
        if not tb.setRect and text.transform.w ~= 0 and oText.transform.w ~= 0 then
            local x, y = tb.position.x, tb.position.y
            tb.position = copyRect(bg.transform)
            tb.position.x = x
            tb.position.y = y
            tb.position.w = bg.transform.w / bg.parent.transform.w
            tb.position.h = bg.transform.h / bg.parent.transform.h
            tb.setRect = true
            bg.transform.x = tb.position.x + 0.01 + text.transform.w
            bg.transform.y = tb.position.y
            text.transform.x = tb.position.x
            oText.transform.y = 0.25
            text.transform.y = tb.position.y + ((bg.transform.h * skin["upscale"]) / 2) -
                ((text.transform.h * skin["upscale"]) / 2)
        end

        if tb.setRect then
            -- information text, animation as well
            if infoText ~= nil then
                local ht = time - tb.hoverTime

                infoText.transform.x = text:getRealRect().x - textbox.container:getRealRect().x

                local thingRect = copyRect(bg:getRealRect())

                thingRect.y = thingRect.y + bg.transformOffset.y

                local oRect = copyRect(text:getRealRect())

                -- check all of these hit boxes because we need it to be ux friendly
                if helper.aabb(Globals.mouseRect, thingRect) or helper.aabb(Globals.mouseRect, oRect) then
                    local endPosReal = text:getRealRect().y + text:getRealRect().h +
                        infoText:getRealRect()
                        .h - textbox.container:getRealRect().y
                    local realYPos = text:getRealRect().y - textbox.container:getRealRect().y
                    local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                    local y = helper.lerp(realYPos, endPosReal, helper.outCubic(a))
                    infoText.transform.y = y
                    infoText.transform.alpha = a
                    tb.lastHover = time
                else
                    local endPosReal = text:getRealRect().y + text:getRealRect().h +
                        infoText:getRealRect()
                        .h - textbox.container:getRealRect().y
                    local realYPos = text:getRealRect().y - textbox.container:getRealRect().y
                    local a = helper.lerp(1, 0, math.min((time - tb.lastHover) / 0.5, 1))
                    local y = helper.lerp(realYPos,
                        endPosReal, helper.outCubic(a))
                    tb.hoverTime = time
                    infoText.transform.y = y
                    infoText.transform.alpha = a
                end
            end
        end
    end
end
