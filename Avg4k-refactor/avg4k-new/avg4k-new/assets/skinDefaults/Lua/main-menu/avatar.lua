Avatar = {}

function Avatar.create()
    -- ok lets go ahead and fetch the avatar data
    -- we can index the "online" table, which is given by average4k
    -- but lets also first make sure that we're connected to the server

    local isConnected = online["connected"]

    if not isConnected then
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
    add(sprite)
    sprite.transform.x = 0
    sprite.transform.y = -0.1
    
    -- the reason why we have (what is called normalized) coordinates is because we're toggling the "ratio" property on the sprite
    sprite.ratio = true
    -- this basically positions the x,y according to the width/height of the game window (or parent)
    -- it also does the same to the width/height of the sprite
    -- basically think of it as, the x coordinate is 5% of the way to the windows width.

    -- lets scale it down a bit though
    sprite.transform.scale = 0.3



    -- now lets make it tween down smoothly to where it actually should be
    local endRect = copyRect(sprite.transform) -- copies the current transform and stores it
    endRect.y = 0 -- set the y to where we want it to be
    cprint(tostring(endRect.x))
    tween(sprite, endRect, 1, "outcubic") -- tween it, arguments are: the sprite, the end transform, length (in seconds), the easing function to use
end