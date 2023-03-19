Avatar = {}

function Avatar.createIcon(default)
    -- create the border using usertypes.

    local border = helper.createSprite("Menu/border", 0,0)
    add(border)
    border.ratio = true
    border.transform.y = -0.1

    local endRect = copyRect(border.transform)
    endRect.y = 0 

    tween(border, endRect, 1, "outcubic")


    -- we create a texture usertype, with the "path" of the avatar data
    -- this is actually just a base64 repersentation of the jpg data returned by steam

    local avatarTexture = texture.new(online["avatarData"])

    -- now we actually tell average4k to load the data into the texture

    loadTextureData(avatarTexture)

    -- cool, now lets create the sprite (which is also another usertype)

    local sprite = sprite.new(0,0, avatarTexture)
    -- a lot of functions are blocked behind actually creating the sprite, so lets do that
    create(sprite)
    -- add it as a child to the border
    border:add(sprite)
    

end

function Avatar.create()
    -- ok lets go ahead and fetch the avatar data
    -- we can index the "online" table, which is given by average4k
    -- but lets also first make sure that we're connected to the server

    local isConnected = online["connected"]

    if not isConnected then
        -- create default icon
        Avatar:createIcon(true)
        return
    end

    Avatar:createIcon(false)
end