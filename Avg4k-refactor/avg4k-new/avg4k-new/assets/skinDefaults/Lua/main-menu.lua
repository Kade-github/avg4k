-- helper files
HelperFiles = { 'avatar.lua', 'containers.lua', 'selection.lua', 'packs/packs.lua', 'packs/songWheel.lua',
    'ui-elements/checkbox.lua' }

fuck_bg = nil

function Create()
    cprint(jit.version)
    -- dofile on all helper files
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/main-menu/" .. HelperFiles[i]))
    end

    -- setup background (darkModeBg.png)

    fuck_bg = helper.createSprite("Menu/darkModeBg", 0.0, 0.0)

    add(fuck_bg)
    fuck_bg.ratio = true
    fuck_bg.transform.x = 0.0
    fuck_bg.transform.y = 0.0
    fuck_bg.transform.w = 1
    fuck_bg.transform.h = 1

    -- helper file calls

    cprint("Initial scale: " .. tostring(skin["upscale"]))

    Avatar.create()
    Containers.create()
    Selection.create()

    cprint("menu created!")

    packContainer.loadBanners()
    packContainer.loadPacks(Containers.scontainer.packs)
end

function MouseDown(pos)
    local mouseRect = vec2torect(pos)
    mouseRect.w = 32
    mouseRect.h = 32
    Selection.mouseDown()
    packContainer.mouseDown()
    checkbox.mouseDown(mouseRect)
end

function MouseWheel(data)
    helper.containerMouseWheel(data)
end

function KeyPress(key)
    local num = tonumber(key)
    if num ~= nil then
        Selection.keyPress(num)
        songWheel.keyPress(num)
    end
end

function GamepadPress(key)
    local num = tonumber(key)
    if num ~= nil then
        Selection.gamepadPress(num)
    end
end

Globals = {}

-- last time
Globals.lt = 0
Globals.start = 0
Globals.mouseRect = nil

function Update(time)
    if Globals.start == 0 then
        Globals.start = time
    end
    local mouse = getMousePos()

    local mRect = rect.new(mouse[1], mouse[2], 16, 16)
    Globals.mouseRect = mRect
    local t = tonumber(time)
    local delta = t - Globals.lt
    Globals.lt = t


    helper.containerUpdate(time)
    songWheel.update(time)
    packContainer.update(time)
    checkbox.update(time)
end
