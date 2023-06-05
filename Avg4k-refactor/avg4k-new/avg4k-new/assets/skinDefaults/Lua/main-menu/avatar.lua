Avatar = {}

function Avatar.createIcon(notDefault)
    -- create the border using usertypes.

    Avatar.border = helper.createSprite("Menu/border", 0, 0)
    add(Avatar.border)
    Avatar.border.ratio = true
    Avatar.border.transform.x = 0.01
    Avatar.border.transform.y = -0.8
    Avatar.border.transform.scale = skin["upscale"] * 0.35
    Avatar.oborder = helper.createSprite("Menu/outerBorder", 0, 0)
    add(Avatar.oborder)
    Avatar.oborder.ratio = true
    Avatar.oborder.transform.x = 0.01
    Avatar.oborder.transform.y = -0.8
    Avatar.oborder.transform.scale = skin["upscale"] * 0.35

    local endRect = copyRect(Avatar.border.transform)
    endRect.y = 0.02

    tween(Avatar.border, endRect, 1.5, "outcubic", "")

    Avatar.helloText = text.new(0, 0, "ArialBold.fnt", "Not logged in.")
    create(Avatar.helloText)
    Avatar.helloText.ratio = true
    Avatar.helloText.size = 24.0 * skin["upscale"]
    Avatar.helloText.transform.y = (14 * skin["upscale"]) / 100
    Avatar.helloText.transform.x = 1.2

    Avatar.versionText = text.new(0, 0, "ArialBold.fnt", "Avg4k indev-" .. online["version"])
    create(Avatar.versionText)
    Avatar.versionText.ratio = true
    Avatar.versionText.size = 24.0 * skin["upscale"]
    Avatar.versionText.transform.y = (40 * skin["upscale"]) / 100
    Avatar.versionText.transform.x = 1.2

    if not notDefault then
        -- if we're not connected to the server, we'll just use the default avatar
        cprint("Loading genericAvatar")
        Avatar.av = helper.createSprite("Menu/genericAvatar", 0, 0)
        create(Avatar.av)
        Avatar.border:add(Avatar.av)
        Avatar.border:add(Avatar.helloText)
        Avatar.border:add(Avatar.versionText)
        Avatar.av.transform.w = 1;
        Avatar.av.transform.h = 1.1;
        Avatar.av.transform.scale = 0.9
        Avatar.av.transform.x = 0.5
        Avatar.av.transform.y = 0.5
        Avatar.av.center = true
        Avatar.av.ratio = true
        return
    end

    Avatar.helloText.text = "Hi " .. online["username"]

    -- we create a texture usertype, with the "path" of the avatar data
    -- this is actually just a base64 repersentation of the jpg data returned by steam

    local avatarTexture = texture.new(online["avatarData"])

    -- now we actually tell average4k to load the data into the texture

    loadTextureData(avatarTexture)

    -- cool, now lets create the sprite (which is also another usertype)

    cprint("loaded avatar " .. tostring(avatarTexture.width) .. "x" .. tostring(avatarTexture.height))

    Avatar.av = sprite.new(0, 0, avatarTexture)
    -- a lot of functions are blocked behind actually creating the sprite, so lets do that
    create(Avatar.av)
    Avatar.border:add(Avatar.av)
    Avatar.border:add(Avatar.helloText)
    Avatar.border:add(Avatar.versionText)
    -- this makes it take up the entire sprite, since it's scalled down .55. (since 55% of the width/height is well, 100% of the width/height if it's scaled 55% down)
    Avatar.av.transform.w = 1;
    Avatar.av.transform.h = 1;
    Avatar.av.transform.scale = 0.9
    Avatar.av.transform.x = 0.5
    Avatar.av.transform.y = 0.5
    Avatar.av.center = true
    Avatar.av.ratio = true
end

function Avatar.create()
    -- ok lets go ahead and fetch the avatar data
    -- we can index the "online" table, which is given by average4k
    -- but lets also first make sure that we're connected to the server

    local isConnected = online["connected"]
    cprint("Steam connection: " .. tostring(isConnected))

    Avatar.createIcon(isConnected)
end
