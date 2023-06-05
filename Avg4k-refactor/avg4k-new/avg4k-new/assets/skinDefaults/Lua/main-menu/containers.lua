Containers = {}

function Containers.soloCreate(c)
    Containers.scontainer = {}
    Containers.scontainer.packs = helper.createSprite("Menu/MainMenu/Solo/packscontainer", 0, 0)
    create(Containers.scontainer.packs)
    c:add(Containers.scontainer.packs)
    Containers.scontainer.packs.tag = "packContainer"
    Containers.scontainer.packs.order = 2
    Containers.scontainer.packs.ratio = true

    Containers.scontainer.packs.transform.y = 0.12

    Containers.scontainer.packs.transform.h = 0.88
    Containers.scontainer.packs.transform.w = 0.2

    Containers.scontainer.packsText = text.new(0, 0, "ArialBold.fnt", "Packs")
    create(Containers.scontainer.packsText)
    c:add(Containers.scontainer.packsText)
    Containers.scontainer.packsText.tag = "packsText"

    Containers.scontainer.packsText.size = 24 * skin["upscale"]
    Containers.scontainer.packsText.characterSpacing = 3

    Containers.scontainer.packsText.order = 1
    Containers.scontainer.packsText.ratio = true

    Containers.scontainer.packsText.transform.x = 0.02
    Containers.scontainer.packsText.transform.y = 0.03

    Containers.scontainer.packsInfo = text.new(0, 0, "ArialItalic.fnt", "holy crap lois")
    create(Containers.scontainer.packsInfo)
    c:add(Containers.scontainer.packsInfo)
    Containers.scontainer.packsInfo.tag = "packsInfo"

    Containers.scontainer.packsInfo.size = 18 * skin["upscale"]

    Containers.scontainer.packsInfo.order = 1
    Containers.scontainer.packsInfo.ratio = true

    Containers.scontainer.packsInfo.transform.x = 0.02
    Containers.scontainer.packsInfo.transform.y = 0.07

    helper.initContainer(Containers.scontainer.packs, true)

    Containers.scontainer.songWheel = helper.createSprite("Menu/MainMenu/Solo/packscontainer", 0, 0)
    create(Containers.scontainer.songWheel)
    c:add(Containers.scontainer.songWheel)
    Containers.scontainer.songWheel.tag = "songWheel"
    Containers.scontainer.songWheel.order = 2
    Containers.scontainer.songWheel.ratio = true
    Containers.scontainer.songWheel.transform.x = Containers.scontainer.packs.transform.x +
        Containers.scontainer.packs.transform.w
    Containers.scontainer.songWheel.transform.y = 0.12

    Containers.scontainer.songWheel.transform.h = 0.88
    Containers.scontainer.songWheel.transform.w = 0.54
    Containers.scontainer.songWheel.transform.alpha = 0

    songWheel.init(Containers.scontainer.songWheel)
    helper.initContainer(Containers.scontainer.songWheel, false)

    Containers.scontainer.songInfo = helper.createSprite("Menu/MainMenu/Solo/songcontainer", 0, 0)
    create(Containers.scontainer.songInfo)
    c:add(Containers.scontainer.songInfo)
    Containers.scontainer.songInfo.tag = "songInfo"
    Containers.scontainer.songInfo.order = 2
    Containers.scontainer.songInfo.ratio = true
    Containers.scontainer.songInfo.transform.x = Containers.scontainer.songWheel.transform.x +
        Containers.scontainer.songWheel.transform.w
    Containers.scontainer.songInfo.transform.y = 0
    Containers.scontainer.songInfo.transform.h = 1
    Containers.scontainer.songInfo.transform.w = 0.26
    helper.initContainer(Containers.scontainer.songInfo, false)

    Containers.scontainer.title = text.new(0, 0, "ArialBold.fnt", "Song Title")
    create(Containers.scontainer.title)
    Containers.scontainer.title.size = 28 * skin["upscale"]
    Containers.scontainer.title.characterSpacing = 3
    Containers.scontainer.songInfo:add(Containers.scontainer.title)
    Containers.scontainer.title.ratio = true
    Containers.scontainer.title.transform.x = 0.06
    Containers.scontainer.title.transform.y = 0.04
    Containers.scontainer.title.tag = "songInfo_title"
    Containers.scontainer.title.order = 2
    Containers.scontainer.title.transform.alpha = 0

    Containers.scontainer.artist = text.new(0, 0, "Arial.fnt", "Song Artist")
    create(Containers.scontainer.artist)
    Containers.scontainer.artist.size = 20 * skin["upscale"]
    Containers.scontainer.artist.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.artist)
    Containers.scontainer.artist.ratio = true
    Containers.scontainer.artist.transform.x = 0.06
    Containers.scontainer.artist.transform.y = 0.08
    Containers.scontainer.artist.tag = "songInfo_artist"
    Containers.scontainer.artist.order = 2
    Containers.scontainer.artist.transform.alpha = 0

    Containers.scontainer.diffValue = text.new(0, 0, "ArialBold.fnt", "Song Diff")
    create(Containers.scontainer.diffValue)
    Containers.scontainer.diffValue.size = 28 * skin["upscale"]
    Containers.scontainer.diffValue.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.diffValue)
    Containers.scontainer.diffValue.ratio = true
    Containers.scontainer.diffValue.ratio = true
    Containers.scontainer.diffValue.center = true
    Containers.scontainer.diffValue.transform.x = 0.5
    Containers.scontainer.diffValue.transform.y = 0.55
    Containers.scontainer.diffValue.tag = "songInfo_difficulty"
    Containers.scontainer.diffValue.order = 2
    Containers.scontainer.diffValue.transform.alpha = 0

    Containers.scontainer.diffName = text.new(0, 0, "Arial.fnt", "Song Diff Name")
    create(Containers.scontainer.diffName)
    Containers.scontainer.diffName.size = 26 * skin["upscale"]
    Containers.scontainer.diffName.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.diffName)
    Containers.scontainer.diffName.ratio = true
    Containers.scontainer.diffName.center = true
    Containers.scontainer.diffName.transform.x = 0.5
    Containers.scontainer.diffName.transform.y = 0.5
    Containers.scontainer.diffName.tag = "songInfo_difficultyName"
    Containers.scontainer.diffName.order = 2
    Containers.scontainer.diffName.transform.alpha = 0

    Containers.scontainer.diffLeftArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow",
        0, 0)
    create(Containers.scontainer.diffLeftArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.diffLeftArrow)
    Containers.scontainer.diffLeftArrow.transform.alpha = 0
    Containers.scontainer.diffLeftArrow.ratio = true
    Containers.scontainer.diffLeftArrow.order = 2
    Containers.scontainer.diffLeftArrow.tag = "songInfo_difficultyLeftArrow"
    Containers.scontainer.diffLeftArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.diffRightArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow",
        0, 0)
    create(Containers.scontainer.diffRightArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.diffRightArrow)
    Containers.scontainer.diffRightArrow.transform.alpha = 0
    Containers.scontainer.diffRightArrow.ratio = true
    Containers.scontainer.diffRightArrow.transform.angle = 180
    Containers.scontainer.diffRightArrow.order = 2
    Containers.scontainer.diffRightArrow.tag = "songInfo_difficultyRightArrow"
    Containers.scontainer.diffRightArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.chartType = text.new(0, 0, "Arial.fnt", "Song Chart Type")
    create(Containers.scontainer.chartType)
    Containers.scontainer.chartType.size = 22 * skin["upscale"]
    Containers.scontainer.chartType.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.chartType)
    Containers.scontainer.chartType.ratio = true
    Containers.scontainer.chartType.transform.x = 0.02
    Containers.scontainer.chartType.transform.y = 0.96
    Containers.scontainer.chartType.tag = "songInfo_chartType"
    Containers.scontainer.chartType.order = 2
    Containers.scontainer.chartType.transform.alpha = 0

    Containers.scontainer.onlineText = text.new(0, 0, "Arial.fnt", "Song Online Text")
    create(Containers.scontainer.onlineText)
    Containers.scontainer.onlineText.size = 22 * skin["upscale"]
    Containers.scontainer.onlineText.ratio = true
    Containers.scontainer.onlineText.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.onlineText)
    Containers.scontainer.onlineText.tag = "songInfo_onlineText"
    Containers.scontainer.onlineText.order = 2
    Containers.scontainer.onlineText.transform.alpha = 0

    Containers.scontainer.bg = nil
end

function Containers.create()
    Containers.solo = helper.createContainer("container", 0, 0)
    add(Containers.solo)
    Containers.solo.tag = "solo"
    Containers.solo.ratio = true
    Containers.solo.transform.x = 0.5
    Containers.solo.center = true
    Containers.solo.transform.y = -0.5
    Containers.solo.transform.scale = skin["upscale"]

    Containers.endRect = copyRect(Containers.solo.transform)
    Containers.endRect.y = 0.5

    Containers.soloCreate(Containers.solo)

    Containers.multiplayer = helper.createContainer("container", 0, 0)
    add(Containers.multiplayer)
    Containers.multiplayer.tag = "multiplayer"
    Containers.multiplayer.ratio = true
    Containers.multiplayer.transform.x = 1.5
    Containers.multiplayer.center = true
    Containers.multiplayer.transform.y = 0.5
    Containers.multiplayer.transform.scale = skin["upscale"]

    Containers.settings = helper.createContainer("container", 0, 0)
    add(Containers.settings)
    Containers.settings.tag = "settings"
    Containers.settings.ratio = true
    Containers.settings.transform.x = 1.5
    Containers.settings.center = true
    Containers.settings.transform.y = 0.5
    Containers.settings.transform.scale = skin["upscale"]

    -- Tweens

    tween(Containers.solo, Containers.endRect, 1, "outcubic", "")

    helper.initContainer(Containers.solo, false)
    helper.initContainer(Containers.multiplayer, false)
    helper.initContainer(Containers.settings, true)
end

function Containers.Resize()
    cprint("resizing containers to " .. tostring(skin["upscale"]))
    Containers.scontainer.title.size = 28 * skin["upscale"]
    Containers.scontainer.artist.size = 28 * skin["upscale"]
    Containers.scontainer.diffValue.size = 28 * skin["upscale"]
    Containers.scontainer.diffName.size = 26 * skin["upscale"]
    Containers.scontainer.chartType.size = 22 * skin["upscale"]
    Containers.scontainer.onlineText.size = 22 * skin["upscale"]
    Containers.scontainer.diffLeftArrow.transform.scale = 0.5 * skin["upscale"]
    Containers.scontainer.diffRightArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.packsText.size = 24 * skin["upscale"]
    Containers.scontainer.packsInfo.size = 24 * skin["upscale"]
    songWheel.resize()
end
