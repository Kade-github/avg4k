function create()
    local bg = texture.new("Menu/darkmodebg")
    loadTexture(bg)
    local bgSpr = sprite.new(0,0,bg)
    bgSpr.transform.w = 1920
    bgSpr.transform.h = 1080
    
    add(s)
end

-- last time
local lt = 0

function update(time)
    local delta = time - lt
    lt = time


end