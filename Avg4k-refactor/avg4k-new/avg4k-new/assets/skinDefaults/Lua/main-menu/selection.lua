Selection = {}

function Selection.create()
    Selection.soloText = text.new(0,0, "AndaleMono.fnt", "solo")
    add(Selection.soloText)
    Selection.soloText.ratio = true
    Selection.soloText.center = true
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.soloText.characterSpacing = 3

    Selection.soloText.transform.y = Containers.endRect.y - 35
    Selection.soloText.transform.x = Containers.endRect.x - 27

    local sTEnd = copyRect(Selection.soloText.transform)

    Selection.multiplayerText = text.new(0,0, "AndaleMono.fnt", "multiplayer")
    add(Selection.multiplayerText)
    Selection.multiplayerText.center = true
    Selection.multiplayerText.ratio = true
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.characterSpacing = 3

    Selection.multiplayerText.transform.y = Containers.endRect.y - 35
    Selection.multiplayerText.transform.x = Containers.endRect.x - 1

    local mTEnd = copyRect(Selection.multiplayerText.transform)

    Selection.settingsText = text.new(0,0, "AndaleMono.fnt", "settings")
    add(Selection.settingsText)
    Selection.settingsText.center = true
    Selection.settingsText.ratio = true
    Selection.settingsText.size = 36 * (skin["upscale"])
    Selection.settingsText.characterSpacing = 3

    Selection.settingsText.transform.y = Containers.endRect.y - 35
    Selection.settingsText.transform.x = Containers.endRect.x + 27

    local seTEnd = copyRect(Selection.settingsText.transform)

    Selection.soloText.transform.y = -75
    Selection.multiplayerText.transform.y = -75
    Selection.settingsText.transform.y = -75

    tween(Selection.soloText, sTEnd, 1.2, "outcubic")
    tween(Selection.multiplayerText, mTEnd, 1.2, "outcubic")
    tween(Selection.settingsText, seTEnd, 1.2, "outcubic")
end

function Resize()
    Selection.soloText.size = 36 * (skin["upscale"])
    Selection.multiplayerText.size = 36 * (skin["upscale"])
    Selection.settingsText.size = 36 * (skin["upscale"])
end