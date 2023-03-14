helper = {}

--[[
    A helper function to create a sprite
]]
function helper.createSprite(path, x, y)
    local tempTexture = texture.new(path)
    loadTexture(tempTexture)
    return sprite.new(x,y,tempTexture)
end