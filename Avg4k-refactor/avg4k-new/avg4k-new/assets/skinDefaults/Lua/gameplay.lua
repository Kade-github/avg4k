-- helper files
HelperFiles = { 'popup.lua' }

initialRect = nil
judgement = nil

function Create()
    cprint(jit.version)
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/gameplay/" .. HelperFiles[i]))
    end

    -- load background

    local mainFolder = options["chart"]["folder"] .. '/'

    local bg = helper.createChartSprite(mainFolder .. options["chart"]["background"], 0.0, 0.0)
    add(bg)
    bg.ratio = true
    bg.transform.w = 1
    bg.transform.h = 1

    bg.transform.alpha = tonumber(settings["Background Transparency"])

    setObject(bg, 0)

    judgement = helper.createAnimatedSpriteSkin("judgements/judgements", 0, 0, 0, 210, 85, nil)

    judgement.transform.x = display["width"] / 2
    judgement.transform.y = display["height"] / 2
    judgement.transform.alpha = 0
    judgement.center = true

    initialRect = copyRect(judgement.transform)
    initialRect.alpha = 1

    Popup.init()
    Popup.showPopup(options["chart"]["title"], options["chart"]["diff"] .. " charted by " .. options["chart"]["charter"])
end

function ArrowJudged(judge)
    judgement.frame = tonumber(judge)
    judgement.transform.alpha = 1

    local newRect = copyRect(initialRect)
    newRect.scale = 1.1

    judgement.transform = newRect

    tween(judgement, initialRect, 0.2, "outcubic", "")
end
