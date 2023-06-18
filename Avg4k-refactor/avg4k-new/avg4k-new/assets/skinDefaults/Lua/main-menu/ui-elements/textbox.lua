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
    local isOut = true
    for i, tb in ipairs(textbox.textboxes) do
        if helper.aabb(pos, tb.objects[1]:getRealRect()) then
            for i, tb in ipairs(textbox.textboxes) do -- make sure all the other textboxes are not selected
                tb.selected = false
            end
            tb.selected = true
            textbox.inTextbox = true
            isOut = false
        end
    end
    if isOut then
        for i, tb in ipairs(textbox.textboxes) do
            if tb.selected then
                tb.changeFunction(tb.setting, tb.currentValue)
                tb.currentValue = settings[tb.setting]
            end
            tb.selected = false
            textbox.inTextbox = false
        end
    end
end

function textbox.keyDown(key)
    for i, tb in ipairs(textbox.textboxes) do
        if tb.selected then
            if tb.keyInput then
                for i, v in ipairs(tb.blacklistedKeys) do
                    if string.lower(v) == string.lower(string.char(key)) then
                        return
                    end
                end
                tb.currentValue = getKeyName(tonumber(key))
                tb.selected = false
                textbox.inTextbox = false
                tb.changeFunction(tb.setting, tb.currentValue)
                tb.currentValue = settings[tb.setting]
            else
                if key == 259 and string.len(tb.currentValue) > 0 then
                    tb.currentValue = tb.currentValue:sub(1, -2)
                    return
                end
                if key == 257 then
                    tb.selected = false
                    textbox.inTextbox = false
                    tb.changeFunction(tb.setting, tb.currentValue)
                    tb.currentValue = settings[tb.setting]
                    return
                end
                if string.len(tb.currentValue) >= tb.maxCharacters then
                    return
                end
                cprint(tostring(key))
                for i, v in ipairs(tb.blacklistedKeys) do
                    if string.lower(v) == string.lower(string.char(key)) then
                        return
                    end
                end
                tb.currentValue = tb.currentValue .. string.char(key)
            end
            return
        end
    end
end

function textbox.update(time)
    for i, tb in ipairs(textbox.textboxes) do
        if not tb.setRect then
            local x, y = tb.position.x, tb.position.y
            tb.position = copyRect(tb.objects[1].transform)
            tb.position.x = x
            tb.position.y = y
            tb.position.w = tb.objects[1].transform.w / tb.objects[1].parent.transform.w
            tb.position.h = tb.objects[1].transform.h / tb.objects[1].parent.transform.h
            tb.setRect = true
        end
        tb.objects[1].transform = tb.position
        tb.objects[1].transform.x = tb.position.x + 0.01 + tb.objects[2].transform.w

        tb.objects[2].transform.y = tb.position.y + ((tb.objects[1].transform.h * skin["upscale"]) / 2) -
            ((tb.objects[2].transform.h * skin["upscale"]) / 2)
        tb.objects[2].transform.x = tb.position.x

        -- display text
        tb.objects[3].text = tb.currentValue
        if tb.selected then
            tb.objects[3].text = tb.objects[3].text .. "_"
        end

        -- information text, animation as well
        if tb.objects[4] ~= nil then
            local ht = time - tb.hoverTime
            -- create a bigger hitbox for text
            local endPosReal = tb.objects[2]:getRealRect().y + tb.objects[2]:getRealRect().h +
                tb.objects[4]:getRealRect()
                .h - textbox.container:getRealRect().y
            tb.objects[4].transform.x = tb.objects[2]:getRealRect().x - textbox.container:getRealRect().x


            local realYPos = tb.objects[2]:getRealRect().y - textbox.container:getRealRect().y


            local thingRect = copyRect(tb.objects[1]:getRealRect())

            thingRect.y = thingRect.y + tb.objects[1].transformOffset.y

            local oRect = copyRect(tb.objects[2]:getRealRect())

            -- check all of these hit boxes because we need it to be ux friendly
            if helper.aabb(Globals.mouseRect, thingRect) or helper.aabb(Globals.mouseRect, oRect) then
                local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                local y = helper.lerp(realYPos, endPosReal, helper.outCubic(a))
                tb.objects[4].transform.y = y
                tb.objects[4].transform.alpha = a
                tb.lastHover = time
            else
                local a = helper.lerp(1, 0, math.min((time - tb.lastHover) / 0.5, 1))
                local y = helper.lerp(realYPos,
                    endPosReal, helper.outCubic(a))
                tb.hoverTime = time
                tb.objects[4].transform.y = y
                tb.objects[4].transform.alpha = a
            end
        end
    end
end
