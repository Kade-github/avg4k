-- helper files
HelperFiles = { 'avatar.lua', 'containers.lua', 'selection.lua', 'packs/packs.lua', 'packs/songWheel.lua' }

function Create()
    cprint(jit.version)
    -- dofile on all helper files
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/main-menu/" .. HelperFiles[i]))
    end

    -- setup background (darkModeBg.png)

    local bg = helper.createSprite("Menu/darkModeBg", 0.0, 0.0)

    add(bg)
    bg.ratio = true
    bg.transform.x = 0.0
    bg.transform.y = 0.0
    bg.transform.w = 1
    bg.transform.h = 1

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
    Selection.mouseDown()
    packContainer.mouseDown()
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

function Resize()
    cprint("Resize scale: " .. tostring(skin["upscale"]))
    Avatar.Resize()
    Containers.Resize()
    Selection.Resize()
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
end
