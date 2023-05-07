-- helper files
HelperFiles = { 'popup.lua' }

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

    Popup.init()
    cprint(helper.dump(options))
    Popup.showPopup(options["chart"]["title"], options["chart"]["diff"] .. " charted by " .. options["chart"]["charter"])
end
