songWheel = {}
songWheel.textures = {}
function songWheel.init(container)
    songWheel.main = container
end

function songWheel.setSongs(files)
    songWheel.main:removeAll()

    for i = 1, #songWheel.textures, 1 do
        deleteTexture(songWheel.textures[i])
    end

    local wheelTexture = texture.new("Menu/MainMenu/Solo/wheelTop")
    loadTexture(wheelTexture)
    

    -- obtain banners
    local banners = {}
    for i = 1, files:size(), 1 do
        local file = files[i]
        table.insert(banners, file.folder .. '/' .. file.songBanner)
    end

    -- load them (with 10 threads)
    songWheel.textures = loadChartTexturesThreaded(banners, 10)

    for i = 1, #songWheel.textures, 1 do
        local t = songWheel.textures[i]
        local sprite = sprite.new(0,0, t)
        create(sprite)
        sprite.transform.w = wheelTexture.width
        sprite.transform.h = wheelTexture.height
        sprite.order = -1
        local y = (i - 1) * wheelTexture.height
        cprint(tostring(y))
        local wheelTop = sprite.new(0,0, wheelTexture)
        create(wheelTop)

        songWheel.main:add(wheelTop)
        wheelTop:add(sprite)
        wheelTop.transform.y = y
    end
    table.insert(songWheel.textures, wheelTexture)
end