textbox = {}

textbox.textboxes = {}

function textbox.CreateTextbox(c, _setting, _maxCharacters, _tag, tinyPos, _changeFunction, _infoText)
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
    oText.transform.x = 0.125
    oText.transform.y = 0.5
    oText.center = true

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
        infoText.ratio = true
        infoText.order = 1
        infoText.tag = "TEXTBOX_INFO_" .. _tag
        table.insert(bTable.objects, infoText)
    end
    table.insert(textbox.textboxes, bTable)
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
        -- information text, animation as well
        if tb.objects[4] ~= nil then
            local ht = time - tb.hoverTime
            -- create a bigger hitbox for text
            local endPosReal = tb.objects[2]:getRealRect().y + tb.objects[2]:getRealRect().h +
                tb.objects[4]:getRealRect()
                .h

            local bigRect = tb.objects[4]:getRealRect()
            bigRect.y = endPosReal
            bigRect.w = bigRect.w + 15
            local endPos = tb.objects[2].transform.y + tb.objects[2].transform.h +
                tb.objects[4].transform
                .h

            -- check all of these hit boxes because we need it to be ux friendly
            if helper.aabb(Globals.mouseRect, tb.objects[1]:getRealRect()) or helper.aabb(Globals.mouseRect, bigRect) or helper.aabb(Globals.mouseRect, tb.objects[2]:getRealRect()) then
                local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                local y = helper.lerp(tb.objects[2].transform.y, endPos, helper.outCubic(a))
                tb.objects[4].transform.x = tb.objects[2].transform.x
                tb.objects[4].transform.y = y
                tb.objects[4].transform.alpha = a
                tb.lastHover = time
            else
                local a = helper.lerp(1, 0, math.min((time - tb.lastHover) / 0.5, 1))
                local y = helper.lerp(tb.objects[2].transform.y, endPos, helper.outCubic(a))
                tb.hoverTime = time
                tb.objects[4].transform.y = y
                tb.objects[4].transform.alpha = a
            end
        end
    end
end
