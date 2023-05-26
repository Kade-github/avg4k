songWheel = {}
songWheel.files = {};
songWheel.textures = {}
songWheel.selectedIndex = 1
songWheel.selectedDiff = 1
songWheel.fakeIndex = 1
songWheel.bg = nil
songWheel.selectedFile = nil
songWheel.moreInfo = false
songWheel.playedSong = true
songWheel.started = 0

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
        sprite.order = -1
        local y = (i - 1) * wheelTexture.height
        local wheelTop = sprite.new(0, 0, wheelTexture)
        create(wheelTop)

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
    Containers.scontainer.songInfo_difficulty.text = diff
    Containers.scontainer.songInfo_difficultyName.text = diffName

    Containers.scontainer.songInfo_difficulty.transform.alpha = 1
    Containers.scontainer.songInfo_difficultyName.transform.alpha = 1

    -- Check if the file has more than one diff, if it does then set the arrows to visible (otherwise don't) and space the correctly around the diff text
    if #songWheel.files[songWheel.selectedIndex].difficulties > 1 then
        local real = Containers.scontainer.songInfo_difficultyName:getRealRect()
        local p = Containers.scontainer.songInfo_difficultyName.parent:getRealRect()
        Containers.scontainer.songInfo_difficultyLeftArrow.transform.alpha = 1
        Containers.scontainer.songInfo_difficultyRightArrow.transform.alpha = 1
        Containers.scontainer.songInfo_difficultyLeftArrow.transform.x = real.x -
            ((Containers.scontainer.songInfo_difficultyLeftArrow.transform.w / 2) * Containers.scontainer.songInfo_difficultyLeftArrow.transform.scale) -
            p.x
        Containers.scontainer.songInfo_difficultyLeftArrow.transform.y = real.y - p.y -
            (((Containers.scontainer.songInfo_difficultyLeftArrow.transform.h / 2) * Containers.scontainer.songInfo_difficultyLeftArrow.transform.scale))
        Containers.scontainer.songInfo_difficultyRightArrow.transform.x = (real.x + real.w - 4) - p.x
        Containers.scontainer.songInfo_difficultyRightArrow.transform.y = real.y - p.y -
            (((Containers.scontainer.songInfo_difficultyRightArrow.transform.h / 2) * Containers.scontainer.songInfo_difficultyRightArrow.transform.scale))
    else
        Containers.scontainer.songInfo_difficultyLeftArrow.transform.alpha = 0
        Containers.scontainer.songInfo_difficultyRightArrow.transform.alpha = 0
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

        Containers.scontainer.songInfo_title.text = title
        Containers.scontainer.songInfo_artist.text = artist
        Containers.scontainer.songInfo_title.transform.alpha = 1
        Containers.scontainer.songInfo_artist.transform.alpha = 1

        if songWheel.selectedFile.chartType == 1 then
            Containers.scontainer.songInfo_chartType.text = "Stepmania"
        elseif songWheel.selectedFile.chartType == 2 then
            Containers.scontainer.songInfo_chartType.text = "Quaver"
        elseif songWheel.selectedFile.chartType == 3 then
            Containers.scontainer.songInfo_chartType.text = "Osu!"
        else
            Containers.scontainer.songInfo_chartType.text = "Unknown"
        end
        Containers.scontainer.songInfo_chartType.transform.alpha = 0.7

        if songWheel.selectedFile.isSteam then
            Containers.scontainer.songInfo_onlineText.text = "Steam Workshop"
        else
            Containers.scontainer.songInfo_onlineText.text = "Local File"
        end

        local p = Containers.scontainer.songInfo_chartType.parent:getRealRect()
        local real = Containers.scontainer.songInfo_onlineText:getRealRect()
        Containers.scontainer.songInfo_onlineText.transform.alpha = 0.7
        Containers.scontainer.songInfo_onlineText.transform.x = p.w -
            ((Containers.scontainer.songInfo_onlineText:getRealRect().w / 2) + (14 * skin['upscale']))
        Containers.scontainer.songInfo_onlineText.transform.y = Containers.scontainer.songInfo_chartType:getRealRect().y -
            p.y


        songWheel.SetDiff()

        if songWheel.selectedFile.songBackground == "" then
            songWheel.bg = nil
            return
        end
        local bg = helper.createChartSprite(
            songWheel.selectedFile.folder .. '/' .. songWheel.selectedFile.songBackground, 0.0, 0.0)
        create(bg)
        bg.center = true
        Containers.scontainer.songInfo:add(bg)
        bg.ratio = true
        bg.transform.x = 0.5
        bg.transform.y = 0.5
        bg.transform.w = 1.0
        bg.transform.h = 1.0
        songWheel.bg = bg

        songWheel.started = Globals.lt
        songWheel.playedSong = false
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
        item.transform.y = ((real.h / 2) - item.transform.h / 2) + ((item.transform.h + 8) * away)
    end

    if not songWheel.playedSong and Globals.lt - songWheel.started > 0.25 then
        local channel = playChannelAsync(songWheel.selectedFile.folder .. '/' .. songWheel.selectedFile.songFile, "menu")
        channel.time = songWheel.selectedFile.previewStart
        songWheel.playedSong = true
    end
end
