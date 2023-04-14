songWheel = {}
songWheel.files = {};
songWheel.textures = {}
songWheel.selectedIndex = 1
songWheel.fakeIndex = 1
songWheel.bg = nil

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

    songWheel.Select(0)
end

function songWheel.Select(amt)
    songWheel.selectedIndex = songWheel.selectedIndex + amt
    if songWheel.selectedIndex < 1 then
        songWheel.selectedIndex = #songWheel.textures - 1
    elseif songWheel.selectedIndex >= #songWheel.textures then
        songWheel.selectedIndex = 1
    end

    if songWheel.bg ~= nil then
        Containers.scontainer.songInfo:remove(songWheel.bg)
    end

    if songWheel.files[songWheel.selectedIndex] ~= nil then
        local title = songWheel.files[songWheel.selectedIndex].songTitle
        local artist = songWheel.files[songWheel.selectedIndex].songArtist
        cprint(tostring(string.len(title)) .. " " .. title)
        if string.len(title) > 22 then
            title = string.sub(title, 1, 22) .. "..."
        end
        if string.len(artist) > 30 then
            artist = string.sub(artist, 1, 30) .. "..."
        end
        if title == "" then
            title = "Unknown"
        end
        if artist == "" then
            artist = "Unknown"
        end
        Containers.scontainer.songInfo_title.text = title
        Containers.scontainer.songInfo_artist.text = artist
        Containers.scontainer.songInfo_title.transform.alpha = 1
        Containers.scontainer.songInfo_artist.transform.alpha = 1
    end

    if songWheel.files[songWheel.selectedIndex].songBackground == "" then
        songWheel.bg = nil
        return
    end
    local bg = helper.createChartSprite(songWheel.files[songWheel.selectedIndex].folder .. '/' .. songWheel.files[songWheel.selectedIndex].songBackground, 0.0, 0.0)
    create(bg)
    bg.center = true
    Containers.scontainer.songInfo:add(bg)
    bg.ratio = true
    bg.transform.x = 0.5
    bg.transform.y = 0.5
    bg.transform.w = 1.0
    bg.transform.h = 1.0
    songWheel.bg = bg
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