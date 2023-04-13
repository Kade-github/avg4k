songWheel = {}
songWheel.files = {};
songWheel.textures = {}
songWheel.selectedIndex = 1
songWheel.fakeIndex = 1

function songWheel.init(container)
    songWheel.main = container
end

function songWheel.setSongs(files)
    songWheel.files = files
    songWheel.main:removeAll()

    for i = 1, #songWheel.textures, 1 do
        deleteTexture(songWheel.textures[i])
    end

    songWheel.selectedIndex = 1
    songWheel.fakeIndex = 1

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
        sprite.tag = "bBackground"
        sprite.transform.w = wheelTexture.width
        sprite.transform.h = wheelTexture.height
        sprite.order = -1
        local y = (i - 1) * wheelTexture.height
        local wheelTop = sprite.new(0,0, wheelTexture)
        create(wheelTop)

        wheelTop.tag = "top-" .. tostring(i)

        songWheel.main:add(wheelTop)
        wheelTop:add(sprite)
        wheelTop.transform.y = y
    end
    table.insert(songWheel.textures, wheelTexture)
end

function songWheel.Select(amt)
    songWheel.selectedIndex = songWheel.selectedIndex + amt
    if songWheel.selectedIndex < 1 then
        songWheel.selectedIndex = #songWheel.textures - 1
    elseif songWheel.selectedIndex >= #songWheel.textures then
        songWheel.selectedIndex = 1
    end

    Containers.scontainer.songInfo:removeAll()

    local bg = helper.createChartSprite(songWheel.files[songWheel.selectedIndex].folder .. '/' .. songWheel.files[songWheel.selectedIndex].songBackground, 0.0, 0.0)
    create(bg)
    Containers.scontainer.songInfo:add(bg)
end

function songWheel.keyPress(num)
    if num == 265 then
        songWheel.Select(-1)
    elseif num == 264 then
        songWheel.Select(1)
    end
end

function songWheel.update(t)
    local ch = songWheel.main.children

    songWheel.fakeIndex = helper.lerp(songWheel.fakeIndex, songWheel.selectedIndex, 0.1);

    local real = songWheel.main:getRealRect()

    for i = 1, ch:size(), 1 do
        -- stagnate x coord of the wheel by selected index
        local item = ch[i]
        
        local away = i - songWheel.fakeIndex
        local rank = away / songWheel.fakeIndex
        local xBasedOnRank = helper.lerp(item.transform.w / 2, 0.0, math.abs(rank));

        if i ~= songWheel.selectedIndex then
            xBasedOnRank = xBasedOnRank * 0.6
        end
        item.transform.scale = skin["upscale"]
        item.transform.x = xBasedOnRank - (item.transform.w / 2)
        item.transform.y = ((real.h / 2) - item.transform.h / 2) + ((item.transform.h + 8) * away)
    end
end