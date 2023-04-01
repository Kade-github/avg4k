Selection = {}

function Selection.select(container)
    cprint("Selecting " .. tostring(container))
    if container == 1 then
        Selection.selectionRect.transform.x = Selection.soloText.transform.x
    elseif container == 2 then
        Selection.selectionRect.transform.x = Selection.multiplayerText.transform.x
    elseif container == 3 then
        Selection.selectionRect.transform.x = Selection.settingsText.transform.x
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

function Selection.create()
    Selection.soloText = text.new(0,0, "AndaleMono.fnt", "solo")
    add(Selection.soloText)
    Selection.soloText.ratio = true
    Selection.soloText.center = true
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.soloText.characterSpacing = 3

    Selection.soloText.transform.y = 0.15
    Selection.soloText.transform.x = Containers.endRect.x - 0.27

    local sTEnd = copyRect(Selection.soloText.transform)

    Selection.multiplayerText = text.new(0,0, "AndaleMono.fnt", "multiplayer")
    add(Selection.multiplayerText)
    Selection.multiplayerText.center = true
    Selection.multiplayerText.ratio = true
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.characterSpacing = 3

    Selection.multiplayerText.transform.y = 0.15
    Selection.multiplayerText.transform.x = Containers.endRect.x - 0.01

    local mTEnd = copyRect(Selection.multiplayerText.transform)

    Selection.settingsText = text.new(0,0, "AndaleMono.fnt", "settings")
    add(Selection.settingsText)
    Selection.settingsText.center = true
    Selection.settingsText.ratio = true
    Selection.settingsText.size = 36 * (skin["upscale"])
    Selection.settingsText.characterSpacing = 3

    Selection.settingsText.transform.y = 0.15
    Selection.settingsText.transform.x = Containers.endRect.x + 0.27

    local seTEnd = copyRect(Selection.settingsText.transform)

    Selection.soloText.transform.y = -0.75
    Selection.multiplayerText.transform.y = -0.75
    Selection.settingsText.transform.y = -0.75


    Selection.selectionRect = rectangle.new(0,0,0.15,0.004)
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


    tween(Selection.soloText, sTEnd, 1.2, "outcubic")
    tween(Selection.selectionRect, srEnd, 1.2, "outcubic")
    tween(Selection.multiplayerText, mTEnd, 1.2, "outcubic")
    tween(Selection.settingsText, seTEnd, 1.2, "outcubic")
end

function Selection.Resize()
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.settingsText.size = 36 * (skin["upscale"])
end