Avatar = {}

function Avatar.createIcon(notDefault)
    -- create the border using usertypes.

    local border = helper.createSprite("Menu/border", 0,0)
    add(border)
    border.ratio = true
    border.transform.y = -0.15
    border.transform.scale = 0.55

    local endRect = copyRect(border.transform)
    endRect.y = 0 

    tween(border, endRect, 1, "outcubic")
    
    if not notDefault then
        -- if we're not connected to the server, we'll just use the default avatar
        local sprite = helper.createSprite("Menu/genericAvatar", 0,0)
        create(sprite)
        border:add(sprite)
        sprite.transform.w = 1
        sprite.transform.h = 1
        sprite.transform.scale = 0.55
        sprite.ratio = true
        return
    end

    -- we create a texture usertype, with the "path" of the avatar data
    -- this is actually just a base64 repersentation of the jpg data returned by steam

    local avatarTexture = texture.new(online["avatarData"])

    -- now we actually tell average4k to load the data into the texture

    loadTextureData(avatarTexture)

    -- cool, now lets create the sprite (which is also another usertype)

    local sprite = sprite.new(0,0, avatarTexture)
    -- a lot of functions are blocked behind actually creating the sprite, so lets do that
    create(sprite)
    border:add(sprite)
    -- this makes it take up the entire sprite (since 100% of the width/height is well, 100% of the width/height)
    sprite.transform.w = 1
    sprite.transform.h = 1

    sprite.transform.scale = 0.55

    sprite.ratio = true

end

function Avatar.create()
    -- ok lets go ahead and fetch the avatar data
    -- we can index the "online" table, which is given by average4k
    -- but lets also first make sure that we're connected to the server

    local isConnected = online["connected"]

    Avatar:createIcon(isConnected)
end