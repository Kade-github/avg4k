songWheel = {}
songWheel.files = {}
songWheel.textures = {}
songWheel.selectedIndex = 1
songWheel.selectedDiff = 1
songWheel.fakeIndex = 1
songWheel.bg = nil
songWheel.selectedFile = nil
songWheel.moreInfo = false
songWheel.playedSong = true
songWheel.started = 0

songWheel.tops = {}
songWheel.banners = {}

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
        local sprite = sprite.new(0, 0, t)
        create(sprite)
        sprite.tag = "bBackground"
        sprite.transform.w = wheelTexture.width
        sprite.transform.h = wheelTexture.height
        sprite.transform.scale = skin["upscale"]
        sprite.order = -1
        local y = (i - 1) * wheelTexture.height
        local wheelTop = sprite.new(0, 0, wheelTexture)
        wheelTop.transform.scale = skin["upscale"]
        create(wheelTop)

        songWheel.tops[i] = wheelTop
        songWheel.banners[i] = sprite

        wheelTop.tag = "top-" .. tostring(i)

        songWheel.main:add(wheelTop)
        wheelTop:add(sprite)
        wheelTop.transform.y = y
    end
    table.insert(songWheel.textures, wheelTexture)

    songWheel.Select(0)
end

function songWheel.SetDiff()
    local diffName = songWheel.files[songWheel.selectedIndex].difficulties[songWheel.selectedDiff].name

    if diffName == "" then
        diffName = "Unknown"
    end
    if string.len(diffName) > 20 then
        diffName = string.sub(diffName, 1, 30) .. "..."
    end

    local diff = tostring(songWheel.files[songWheel.selectedIndex].difficulties[songWheel.selectedDiff].difficultyRating)
    Containers.scontainer.diffValue.text = diff
    Containers.scontainer.diffName.text = diffName

    Containers.scontainer.diffValue.transform.alpha = 1
    Containers.scontainer.diffName.transform.alpha = 1

    -- Check if the file has more than one diff, if it does then set the arrows to visible (otherwise don't) and space the correctly around the diff text
    if #songWheel.files[songWheel.selectedIndex].difficulties > 1 then
        Containers.scontainer.diffLeftArrow.transform.alpha = 1
        Containers.scontainer.diffRightArrow.transform.alpha = 1
    else
        Containers.scontainer.diffLeftArrow.transform.alpha = 0
        Containers.scontainer.diffRightArrow.transform.alpha = 0
    end
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

    songWheel.selectedDiff = 1



    if songWheel.files[songWheel.selectedIndex] ~= nil then
        songWheel.selectedFile = songWheel.files[songWheel.selectedIndex]
        local title = songWheel.selectedFile.songTitle
        local artist = songWheel.selectedFile.songArtist
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

        Containers.scontainer.title.text = title
        Containers.scontainer.artist.text = artist
        Containers.scontainer.title.transform.alpha = 1
        Containers.scontainer.artist.transform.alpha = 1

        if songWheel.selectedFile.chartType == 1 then
            Containers.scontainer.chartType.text = "Stepmania"
        elseif songWheel.selectedFile.chartType == 2 then
            Containers.scontainer.chartType.text = "Quaver"
        elseif songWheel.selectedFile.chartType == 3 then
            Containers.scontainer.chartType.text = "Osu!"
        else
            Containers.scontainer.chartType.text = "Unknown"
        end
        Containers.scontainer.chartType.transform.alpha = 0.7

        if songWheel.selectedFile.isSteam then
            Containers.scontainer.onlineText.text = "Steam Workshop"
        else
            Containers.scontainer.onlineText.text = "Local File"
        end

        Containers.scontainer.onlineText.transform.alpha = 0.7

        if #songWheel.files[songWheel.selectedIndex].difficulties > 1 then
            Containers.scontainer.diffLeftArrow.transform.alpha = 1
            Containers.scontainer.diffRightArrow.transform.alpha = 1
        else
            Containers.scontainer.diffLeftArrow.transform.alpha = 0
            Containers.scontainer.diffRightArrow.transform.alpha = 0
        end

        songWheel.SetDiff()

        songWheel.started = Globals.lt
        songWheel.playedSong = false

        if songWheel.selectedFile.songBackground == "" then
            songWheel.bg = nil
            return
        end
        Containers.scontainer.bg = helper.createChartSprite(
            songWheel.selectedFile.folder .. '/' .. songWheel.selectedFile.songBackground, 0.0, 0.0)
        create(Containers.scontainer.bg)
        Containers.scontainer.bg.center = true
        Containers.scontainer.songInfo:add(Containers.scontainer.bg)
        Containers.scontainer.bg.ratio = true
        Containers.scontainer.bg.transform.x = 0.5
        Containers.scontainer.bg.transform.y = 0.5
        Containers.scontainer.bg.transform.w = 1.0
        Containers.scontainer.bg.transform.h = 1.0
        Containers.scontainer.bg.order = -1
        songWheel.bg = Containers.scontainer.bg
    end
end

function songWheel.keyPress(num)
    if num == 265 then
        songWheel.Select(-1)
    elseif num == 264 then
        songWheel.Select(1)
    end

    if songWheel.selectedFile == nil then
        return
    end

    if num == 263 then
        songWheel.selectedDiff = songWheel.selectedDiff - 1
        if songWheel.selectedDiff < 1 then
            songWheel.selectedDiff = #songWheel.selectedFile.difficulties
        end
        songWheel.SetDiff()
    end
    if num == 262 then
        songWheel.selectedDiff = songWheel.selectedDiff + 1
        if songWheel.selectedDiff > #songWheel.selectedFile.difficulties then
            songWheel.selectedDiff = 1
        end

        songWheel.SetDiff()
    end

    if not songWheel.moreInfo then
        if num == 257 then
            cprint("set chart to " .. songWheel.selectedFile.songTitle)
            setChart(songWheel.selectedFile, songWheel.selectedDiff)
            startChart()
        end
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
        item.transform.y = (((real.h * skin["upscale"]) / 2) - item.transform.h / 2) + ((item.transform.h + 8) * away)
    end

    Containers.scontainer.onlineText.transform.x = 1 -
        (Containers.scontainer.onlineText.transform.w + (Containers.scontainer.onlineText.transform.w / 12))
    Containers.scontainer.onlineText.transform.y = Containers.scontainer.chartType.transform.y

    Containers.scontainer.diffLeftArrow.transform.x = Containers.scontainer.diffName
        .transform.x -
        (Containers.scontainer.diffLeftArrow.transform.w + Containers.scontainer.diffName
            .transform.w / 2)
    Containers.scontainer.diffLeftArrow.transform.y = Containers.scontainer.diffName
        .transform.y - (Containers.scontainer.diffName.transform.h / 2)

    Containers.scontainer.diffRightArrow.transform.x = Containers.scontainer.diffName
        .transform.x +
        ((Containers.scontainer.diffName.transform.w / 2) + (Containers.scontainer.diffRightArrow.transform.w / 2))

    Containers.scontainer.diffRightArrow.transform.y = Containers.scontainer.diffName
        .transform.y - (Containers.scontainer.diffName.transform.h / 2)

    if not songWheel.playedSong and Globals.lt - songWheel.started > 0.25 then
        local channel = playChannelAsync(songWheel.selectedFile.folder .. '/' .. songWheel.selectedFile.songFile, "menu")
        channel.time = songWheel.selectedFile.previewStart
        songWheel.playedSong = true
    end
end
