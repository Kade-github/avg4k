-- helper files
HelperFiles = {'avatar.lua', 'container.lua', 'selection.lua'}

function Create()
    -- dofile on all helper files
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/main-menu/" .. HelperFiles[i]))
    end

    -- helper file calls

    Avatar.create()
end

-- last time
local lt = 0

function Update(time)
    local delta = time - lt
    lt = time


end