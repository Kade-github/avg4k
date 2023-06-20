dropdown = {}

dropdown.dropdowns = {}
dropdown.inDropdown = false
dropdown.itemTexture = nil
dropdown.itemTextureEnd = nil

function dropdown.InitDropdown()
    dropdown.itemTexture = texture.new("Menu/MainMenu/Settings/dropdownItem")
    loadTexture(dropdown.itemTexture)
    dropdown.itemTextureEnd = texture.new("Menu/MainMenu/Settings/endOfDropdown")
    loadTexture(dropdown.itemTextureEnd)
end

function dropdown.CreateDrowndown(c, _setting, _tag, _items, _tinyPos, _changeFunction, _infoText)
    local bTable = {
        currentValue = settings[_setting],
        tag = _tag,
        setting = _setting,
        objects = {},
        changeFunction = _changeFunction,
        position = rect.new(_tinyPos[1], _tinyPos[2]),
        hoverTime = -1,
        lastHover = -1,
        items = {},
        infoText = _infoText
    }

    local background = helper.createSprite("Menu/MainMenu/Settings/dropdown", 0.0, 0.0)
    if c ~= nil then
        create(background)
        c:add(background)
    else
        add(background)
    end
    background.tag = "DROPDOWN_BG_" .. _tag
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
    text.tag = "DROPDOWN_TEXT_" .. _tag

    local oText = text.new(0, 0, "ArialBold.fnt", bTable.currentValue)
    create(oText)
    background:add(oText)
    oText.size = 16 * skin["upscale"]
    oText.characterSpacing = 1.67
    oText.ratio = true
    oText.order = 1
    oText.tag = "DROPDOWN_DATA_" .. _tag
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

    local size = #_items
    -- create items for the dropdown
    for i, it in ipairs(_items) do
        local itemBg = nil
        if i ~= size then
            itemBg = helper.createSpriteTex(0, 0, dropdown.itemTexture)
        else
            itemBg = helper.createSpriteTex(0, 0, dropdown.itemTextureEnd)
        end

        create(itemBg)
        background:add(itemBg)
        itemBg.ratio = true
        itemBg.order = 1
        itemBg.transform.scale = skin["upscale"]
        itemBg.transform.alpha = 0
        itemBg.tag = "DROPDOWN_BG_" .. _tag .. "_" .. i

        local itemText = text.new(0, 0, "ArialBold.fnt", it)
        create(itemText)
        itemBg:add(itemText)
        itemText.size = 16 * skin["upscale"]
        itemText.characterSpacing = 1.67
        itemText.ratio = true
        itemText.order = 1
        itemText.tag = "DROPDOWN_ITEM_" .. _tag .. "_" .. i
        itemText.transform.r = 13
        itemText.transform.g = 28
        itemText.transform.b = 64
        itemText.transform.alpha = 0
        itemText.transform.x = 0.5
        itemText.transform.y = 0.5
        itemText.center = true

        table.insert(bTable.items, itemBg)
    end
    cprint("size: " .. tostring(#bTable.items))
    table.insert(dropdown.dropdowns, bTable)
end

function dropdown.mouseDown(pos)
    if textbox.inTextbox then
        return
    end
    local isOut = true
    for i, dd in ipairs(dropdown.dropdowns) do
        local realerRect = copyRect(dd.objects[1]:getRealRect())
        realerRect.y = realerRect.y + dd.objects[1].transformOffset.y
        if helper.aabb(pos, realerRect) then
            for i, dd2 in ipairs(dropdown.dropdowns) do -- make sure all the other dropdowns are not selected
                dd2.selected = false
            end
            cprint("YES!!")
            dd.selected = true
            dropdown.inDropdown = true
            isOut = false
        end
    end
    if isOut then
        for i, dd in ipairs(dropdown.dropdowns) do
            local itemm = false
            for i, item in ipairs(dd.items) do -- check if you clicked on an item
                local realerRect = copyRect(item:getRealRect())
                realerRect.y = realerRect.y + dd.objects[1].transformOffset.y
                if helper.aabb(pos, realerRect) then
                    dd.currentValue = dd.items[i].text
                    itemm = true
                end
            end
            if dd.selected and not itemm then
                dd.changeFunction(dd.setting, dd.currentValue)
                dd.currentValue = settings[dd.setting]
            end
            dd.selected = false
            dropdown.inDropdown = false
        end
    end
end

function dropdown.update(time)
    for i, dd in ipairs(dropdown.dropdowns) do
        if not dd.setRect then
            local x, y = dd.position.x, dd.position.y
            dd.position = copyRect(dd.objects[1].transform)
            dd.position.x = x
            dd.position.y = y
            dd.position.w = dd.objects[1].transform.w / dd.objects[1].parent.transform.w
            dd.position.h = dd.objects[1].transform.h / dd.objects[1].parent.transform.h
            dd.setRect = true
        end
        dd.objects[1].transform = dd.position
        dd.objects[1].transform.x = dd.position.x + 0.01 + dd.objects[2].transform.w

        dd.objects[2].transform.y = dd.position.y + ((dd.objects[1].transform.h * skin["upscale"]) / 2) -
            ((dd.objects[2].transform.h * skin["upscale"]) / 2)
        dd.objects[2].transform.x = dd.position.x

        -- display text
        dd.objects[3].text = dd.currentValue


        -- information text, animation as well
        if dd.objects[4] ~= nil then
            local ht = time - dd.hoverTime
            -- create a bigger hitbox for text
            local endPosReal = dd.objects[2]:getRealRect().y + dd.objects[2]:getRealRect().h +
                dd.objects[4]:getRealRect()
                .h - textbox.container:getRealRect().y
            dd.objects[4].transform.x = dd.objects[2]:getRealRect().x - textbox.container:getRealRect().x


            local realYPos = dd.objects[2]:getRealRect().y - textbox.container:getRealRect().y


            local thingRect = copyRect(dd.objects[1]:getRealRect())

            thingRect.y = thingRect.y + dd.objects[1].transformOffset.y

            local oRect = copyRect(dd.objects[2]:getRealRect())

            -- check all of these hit boxes because we need it to be ux friendly
            if helper.aabb(Globals.mouseRect, thingRect) or helper.aabb(Globals.mouseRect, oRect) then
                local a = helper.lerp(0, 1, math.min(ht / 0.5, 1))
                local y = helper.lerp(realYPos, endPosReal, helper.outCubic(a))
                dd.objects[4].transform.y = y
                dd.objects[4].transform.alpha = a
                dd.lastHover = time
            else
                local a = helper.lerp(1, 0, math.min((time - dd.lastHover) / 0.5, 1))
                local y = helper.lerp(realYPos,
                    endPosReal, helper.outCubic(a))
                dd.hoverTime = time
                dd.objects[4].transform.y = y
                dd.objects[4].transform.alpha = a
            end
        end
    end
end
