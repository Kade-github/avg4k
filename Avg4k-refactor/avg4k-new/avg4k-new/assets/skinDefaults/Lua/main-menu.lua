-- helper files
HelperFiles = {'avatar.lua', 'container.lua', 'selection.lua'}

function Create()
    -- dofile on all helper files
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/main-menu/" .. HelperFiles[i]))
    end

    -- setup background (darkModeBg.png)

    local bg = helper.createSprite("Menu/darkModeBg", 0.0,0.0)

    add(bg)
    bg.ratio = true
    bg.transform.x = 0.0
    bg.transform.y = 0.0
    bg.transform.w = 1.0
    bg.transform.h = 1.0

    -- helper file calls

    Avatar.create()
end

-- last time
local lt = 0

function Update(time)
    local delta = time - lt
    lt = time


end