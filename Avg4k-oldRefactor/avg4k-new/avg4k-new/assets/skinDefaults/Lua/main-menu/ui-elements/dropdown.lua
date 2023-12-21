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
        itemValues = {},
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
    background.order = 5
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
    oText.order = 5
    oText.tag = "DROPDOWN_DATA_" .. _tag
    oText.transform.r = 13
    oText.transform.g = 28
    oText.transform.b = 64
    oText.transform.x = 0.05
    oText.transform.y = 0.25

    table.insert(bTable.objects, background)
    table.insert(bTable.objects, text)
    table.insert(bTable.objects, oText)

    local connector = helper.createSpriteTex(0, 0, dropdown.itemTexture)

    if c ~= nil then
        create(connector)
        c:add(connector)
    else
        add(connector)
    end
    connector.ratio = true
    connector.order = 2
    connector.transform.scale = skin["upscale"]
    connector.transform.alpha = 0
    connector.tag = "DROPDOWN_CONNECTOR_" .. _tag
    table.insert(bTable.objects, connector)

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
        itemBg.order = 18
        itemBg.transform.scale = skin["upscale"]
        itemBg.transform.alpha = 0
        itemBg.tag = "DROPDOWN_BG_" .. _tag .. "_" .. i

        local itemText = text.new(0, 0, "ArialBold.fnt", it)
        create(itemText)
        itemBg:add(itemText)
        itemText.size = 16 * skin["upscale"]
        itemText.characterSpacing = 1.67
        itemText.ratio = true
        itemText.order = 18
        itemText.tag = "DROPDOWN_ITEM_" .. _tag .. "_" .. i
        itemText.transform.r = 13
        itemText.transform.g = 28
        itemText.transform.b = 64
        itemText.transform.alpha = 0
        itemText.transform.x = 0.05

        itemText.transform.y = 0.25

        if i == size then
            itemText.transform.y = 0.15
        end

        table.insert(bTable.items, itemBg)
        table.insert(bTable.itemValues, it)
    end


    table.insert(dropdown.dropdowns, bTable)
end

function dropdown.mouseDown(pos)
    if textbox.inTextbox then
        return
    end
    local isOut = true
    for i, dd in ipairs(dropdown.dropdowns) do
        if not dropdown.inDropdown then
            local realerRect = copyRect(dd.objects[1]:getRealRect())
            local connector = dd.objects[4]
            realerRect.y = realerRect.y + dd.objects[1].transformOffset.y
            if helper.aabb(pos, realerRect) then
                for i, dd2 in ipairs(dropdown.dropdowns) do -- make sure all the other dropdowns are not selected
                    dd2.selected = false
                end
                connector.transform.alpha = 1
                for i, it in ipairs(dd.items) do
                    it.children[1].transform.alpha = 1
                    it.transform.alpha = 1
                    it.transform.y = 1 + ((it.transform.h * (i - 1)) * skin["upscale"])
                    if i == #dd.items then
                        it.transform.y = it.transform.y - 0.05
                    end
                end
                dd.selected = true
                dropdown.inDropdown = true
                isOut = false
            end
        end
    end
    if isOut then
        for i, dd in ipairs(dropdown.dropdowns) do
            local oText = dd.objects[3]
            local connector = dd.objects[4]
            local itemm = false
            if dd.selected then
                for i, item in ipairs(dd.items) do -- check if you clicked on an item
                    if not itemm then
                        local realerRect = copyRect(item.children[1]:getRealRect())
                        realerRect.w = item:getRealRect().w
                        if helper.aabb(Globals.mouseRect, realerRect) then
                            dd.currentValue = dd.itemValues[i]
                            dd.changeFunction(dd.setting, dd.currentValue)
                            oText.text = dd.currentValue
                            itemm = true
                        end
                    end
                end
                if not itemm then
                    dd.currentValue = settings[dd.setting]
                    dd.changeFunction(dd.setting, dd.currentValue)
                    oText.text = dd.currentValue
                end
                connector.transform.alpha = 0
                for i, it in ipairs(dd.items) do
                    it.children[1].transform.alpha = 0
                    it.transform.alpha = 0
                end
                dd.selected = false
                dropdown.inDropdown = false
            end
        end
    end
end

function dropdown.update(time)
    for i, dd in ipairs(dropdown.dropdowns) do
        local bg = dd.objects[1]
        if bg.visible then
            local text = dd.objects[2]
            local oText = dd.objects[3]
            local connector = dd.objects[4]
            local infoText = dd.objects[5]

            if not dd.setRect and text.transform.w ~= 0 and oText.transform.w ~= 0 then
                local x, y = dd.position.x, dd.position.y
                dd.position = copyRect(bg.transform)
                dd.position.x = x
                dd.position.y = y
                dd.position.w = bg.transform.w / bg.parent.transform.w
                dd.position.h = bg.transform.h / bg.parent.transform.h
                dd.setRect = true
                bg.transform.x = dd.position.x + 0.01 + text.transform.w
                bg.transform.y = dd.position.y
                text.transform.x = dd.position.x
                oText.transform.y = 0.25
                text.transform.y = dd.position.y + ((bg.transform.h * skin["upscale"]) / 2) -
                    ((text.transform.h * skin["upscale"]) / 2)
                connector.transform.x = bg.transform.x
                connector.transform.y = bg.transform.y + (connector.transform.h / 2)
            end

            if dd.setRect then
                if dd.selected then
                    local stop = false
                    for id, it in ipairs(dd.items) do
                        if not stop then
                            local realerRect = copyRect(it.children[1]:getRealRect())
                            realerRect.w = it:getRealRect().w
                            if helper.aabb(Globals.mouseRect, realerRect) then
                                stop = true
                                it.children[1].transform.alpha = 0.7
                            else
                                it.children[1].transform.alpha = 1
                            end
                        else
                            it.children[1].transform.alpha = 1
                        end
                    end
                end

                -- information text, animation as well
                if infoText ~= nil then
                    local ht = time - dd.hoverTime

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
                        dd.lastHover = time
                    else
                        local endPosReal = text:getRealRect().y + text:getRealRect().h +
                            infoText:getRealRect()
                            .h - textbox.container:getRealRect().y
                        local realYPos = text:getRealRect().y - textbox.container:getRealRect().y
                        local a = helper.lerp(1, 0, math.min((time - dd.lastHover) / 0.5, 1))
                        local y = helper.lerp(realYPos,
                            endPosReal, helper.outCubic(a))
                        dd.hoverTime = time
                        infoText.transform.y = y
                        infoText.transform.alpha = a
                    end
                end
            end
        end
    end
end
