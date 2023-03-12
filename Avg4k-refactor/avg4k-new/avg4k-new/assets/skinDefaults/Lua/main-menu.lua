function create()
    local bg = texture.new("Menu/darkmodebg")
    loadTexture(bg)
    local bgSpr = sprite.new(0,0,bg)
    add(bgSpr)
    bgSpr.transform.w = 1920
    bgSpr.transform.h = 1080

    local soloText = text.new(0.5,0.25, "AndaleMono.fnt", "solo")
    add(soloText)
    soloText.characterSpacing = 3
    soloText.ratio = true
    soloText.transform.x = 0.5
    soloText.transform.y = 0.25
    soloText.center = true
    soloText.size = 40
end

-- last time
local lt = 0

function update(time)
    local delta = time - lt
    lt = time


end