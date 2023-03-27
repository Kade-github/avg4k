Avatar = {}

function Avatar.createIcon(notDefault)
    -- create the border using usertypes.

    local border = helper.createSprite("Menu/border", 0,0)
    add(border)
    border.ratio = true
    border.transform.x = 1
    border.transform.y = -15
    border.transform.scale = 0.55

    local endRect = copyRect(border.transform)
    endRect.y = 2.5

    tween(border, endRect, 1, "outcubic")

    local helloText = text.new(0,0, "ArialBold.fnt", "Not logged in.")
    create(helloText)
    helloText.ratio = true
    helloText.size = 16.0 * (skin["upscale"] * 0.9)
    helloText.transform.y = 12
    helloText.transform.x = 60

    local versionText = text.new(0,0, "ArialBold.fnt", "Avg4k indev-" .. online["version"])
    create(versionText)
    versionText.ratio = true
    versionText.size = 16.0 * (skin["upscale"] * 0.9)
    versionText.transform.y = 24
    versionText.transform.x = 60

    if not notDefault then
        -- if we're not connected to the server, we'll just use the default avatar
        local genAv = helper.createSprite("Menu/genericAvatar", 0,0)
        create(genAv)
        border:add(genAv)
        border:add(helloText)
        border:add(versionText)
        genAv.transform.w = 100
        genAv.transform.h = 100
        genAv.transform.scale = 0.55
        genAv.ratio = true
        return
    end

    helloText.text = "Hi " .. online["username"]

    -- we create a texture usertype, with the "path" of the avatar data
    -- this is actually just a base64 repersentation of the jpg data returned by steam

    local avatarTexture = texture.new(online["avatarData"])

    -- now we actually tell average4k to load the data into the texture

    loadTextureData(avatarTexture)

    -- cool, now lets create the sprite (which is also another usertype)

    local av = sprite.new(0,0, avatarTexture)
    -- a lot of functions are blocked behind actually creating the sprite, so lets do that
    create(av)
    border:add(av)
    border:add(helloText)
    border:add(versionText)
    -- this makes it take up the entire sprite, since it's scalled down .55. (since 55% of the width/height is well, 100% of the width/height if it's scaled 55% down)
    av.transform.w = 55
    av.transform.h = 55

    av.ratio = true

end

function Avatar.create()
    -- ok lets go ahead and fetch the avatar data
    -- we can index the "online" table, which is given by average4k
    -- but lets also first make sure that we're connected to the server

    local isConnected = online["connected"]

    Avatar:createIcon(isConnected)
end