Selection = {}

Selection.currentContainerIndex = 1
Selection.currentContainer = nil
Selection.lastChange = 0

function Selection.select(container)
    if container == Selection.currentContainerIndex then
        return
    end

    local cTime = getTime()

    if cTime > Selection.lastChange + 0.25 then
        Selection.lastChange = cTime
    else
        return
    end

    cprint("Selecting " .. tostring(container))

    local cRect = copyRect(Selection.currentContainer.transform)
    local nContainer = nil

    if container == 1 then
        nContainer = Containers.solo
        Selection.selectionRect.transform.x = Selection.soloText.transform.x
    elseif container == 2 then
        nContainer = Containers.multiplayer
        Selection.selectionRect.transform.x = Selection.multiplayerText.transform.x
    elseif container == 3 then
        nContainer = Containers.settings
        Selection.selectionRect.transform.x = Selection.settingsText.transform.x
    end

    local nRect = copyRect(nContainer.transform)
    nRect.x = 0.5
    nRect.y = 0.5
    cRect.y = 0.5

    if container < Selection.currentContainerIndex then
        nContainer.transform.x = -1.5
        cRect.x = 1.5
    else
        nContainer.transform.x = 1.5
        cRect.x = -1.5
    end

    tween(Selection.currentContainer, cRect, 0.75, "outCubic")
    tween(nContainer, nRect, 0.75, "outCubic")

    Selection.currentContainerIndex = container
    Selection.currentContainer = nContainer
end

function Selection.mouseDown()
    local realSolo = Selection.soloHit:getRealRect()
    local realMulti = Selection.multiplayerHit:getRealRect()
    local realSettings = Selection.settingsHit:getRealRect()



    if helper.aabb(Globals.mouseRect, realSolo) then
        Selection.select(1)
    elseif helper.aabb(Globals.mouseRect, realMulti) then
        Selection.select(2)
    elseif helper.aabb(Globals.mouseRect, realSettings) then
        Selection.select(3)
    end
end

function Selection.keyPress(num)
    if num == 49 then
        Selection.select(1)
    end
    if num == 50 then
        Selection.select(2)
    end
    if num == 51 then
        Selection.select(3)
    end
end

function Selection.gamepadPress(num)
    if num == gamepad_rb then
        local next = Selection.currentContainerIndex + 1
        if next > 3 then
            next = 1
        end
        Selection.select(next)
    end

    if num == gamepad_lb then
        local next = Selection.currentContainerIndex - 1
        if next < 1 then
            next = 3
        end
        Selection.select(next)
    end
end

function Selection.create()
    Selection.soloText = text.new(0, 0, "AndaleMono.fnt", "solo")
    add(Selection.soloText)
    Selection.soloText.ratio = true
    Selection.soloText.center = true
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.soloText.characterSpacing = 3

    Selection.soloText.transform.y = 0.15
    Selection.soloText.transform.x = Containers.endRect.x - 0.25

    local sTEnd = copyRect(Selection.soloText.transform)

    Selection.multiplayerText = text.new(0, 0, "AndaleMono.fnt", "multiplayer")
    add(Selection.multiplayerText)
    Selection.multiplayerText.center = true
    Selection.multiplayerText.ratio = true
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.characterSpacing = 3

    Selection.multiplayerText.transform.y = 0.15
    Selection.multiplayerText.transform.x = Containers.endRect.x + 0.02

    local mTEnd = copyRect(Selection.multiplayerText.transform)

    Selection.settingsText = text.new(0, 0, "AndaleMono.fnt", "settings")
    add(Selection.settingsText)
    Selection.settingsText.center = true
    Selection.settingsText.ratio = true
    Selection.settingsText.size = 36 * (skin["upscale"])
    Selection.settingsText.characterSpacing = 3

    Selection.settingsText.transform.y = 0.15
    Selection.settingsText.transform.x = Containers.endRect.x + 0.25

    local seTEnd = copyRect(Selection.settingsText.transform)

    Selection.soloText.transform.y = -0.75
    Selection.multiplayerText.transform.y = -0.75
    Selection.settingsText.transform.y = -0.75


    Selection.selectionRect = rectangle.new(0, 0, 0.2, 0.004)
    add(Selection.selectionRect)
    Selection.selectionRect.ratio = true
    Selection.selectionRect.center = true
    Selection.selectionRect.transform.r = 83
    Selection.selectionRect.transform.g = 234
    Selection.selectionRect.transform.b = 209

    Selection.selectionRect.transform.x = Selection.soloText.transform.x
    Selection.selectionRect.transform.y = 0.17
    local srEnd = copyRect(Selection.selectionRect.transform)

    Selection.selectionRect.transform.y = -0.75

    -- Hitboxes
    Selection.soloHit = rectangle.new(0, 0, Selection.selectionRect.transform.w, 0)
    Selection.soloHit.transform.x = sTEnd.x
    Selection.soloHit.transform.y = sTEnd.y
    Selection.soloHit.transform.h = 0.03
    add(Selection.soloHit)
    Selection.soloHit.ratio = true
    Selection.soloHit.center = true
    Selection.soloHit.transform.alpha = 0

    Selection.multiplayerHit = rectangle.new(0, 0, Selection.selectionRect.transform.w, 0)
    Selection.multiplayerHit.transform.x = mTEnd.x
    Selection.multiplayerHit.transform.y = mTEnd.y
    Selection.multiplayerHit.transform.h = 0.03
    add(Selection.multiplayerHit)
    Selection.multiplayerHit.ratio = true
    Selection.multiplayerHit.center = true
    Selection.multiplayerHit.transform.alpha = 0

    Selection.settingsHit = rectangle.new(0, 0, Selection.selectionRect.transform.w, 0)
    Selection.settingsHit.transform.x = seTEnd.x
    Selection.settingsHit.transform.y = seTEnd.y
    Selection.settingsHit.transform.h = 0.03
    add(Selection.settingsHit)
    Selection.settingsHit.ratio = true
    Selection.settingsHit.center = true
    Selection.settingsHit.transform.alpha = 0


    tween(Selection.soloText, sTEnd, 1.2, "outcubic")
    tween(Selection.selectionRect, srEnd, 1.2, "outcubic")
    tween(Selection.multiplayerText, mTEnd, 1.2, "outcubic")
    tween(Selection.settingsText, seTEnd, 1.2, "outcubic")

    Selection.currentContainer = Containers.solo
    Selection.lastChange = getTime() + 1.2
end

function Selection.Resize()
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.settingsText.size = 36 * (skin["upscale"])
end
