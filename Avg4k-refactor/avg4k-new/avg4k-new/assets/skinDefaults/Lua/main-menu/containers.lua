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
    Containers.scontainer.songWheel.transform.x = Containers.scontainer.packs.transform.x + Containers.scontainer.packs.transform.w
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
    Containers.scontainer.songInfo.transform.x = Containers.scontainer.songWheel.transform.x + Containers.scontainer.songWheel.transform.w
    Containers.scontainer.songInfo.transform.y = 0
    Containers.scontainer.songInfo.transform.h = 1
    Containers.scontainer.songInfo.transform.w = 0.26
    helper.initContainer(Containers.scontainer.songInfo, false)

    Containers.scontainer.songInfo_title = text.new(0, 0, "ArialBold.fnt", "Song Title")
    create(Containers.scontainer.songInfo_title)
    Containers.scontainer.songInfo_title.size = 28 * skin["upscale"]
    Containers.scontainer.songInfo_title.characterSpacing = 3
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_title)
    Containers.scontainer.songInfo_title.ratio = true
    Containers.scontainer.songInfo_title.transform.x = 0.06
    Containers.scontainer.songInfo_title.transform.y = 0.04
    Containers.scontainer.songInfo_title.tag = "songInfo_title"
    Containers.scontainer.songInfo_title.order = 2
    Containers.scontainer.songInfo_title.transform.alpha = 0

    Containers.scontainer.songInfo_artist = text.new(0, 0, "Arial.fnt", "Song Artist")
    create(Containers.scontainer.songInfo_artist)
    Containers.scontainer.songInfo_artist.size = 20 * skin["upscale"]
    Containers.scontainer.songInfo_artist.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_artist)
    Containers.scontainer.songInfo_artist.ratio = true
    Containers.scontainer.songInfo_artist.transform.x = 0.06
    Containers.scontainer.songInfo_artist.transform.y = 0.08
    Containers.scontainer.songInfo_artist.tag = "songInfo_artist"
    Containers.scontainer.songInfo_artist.order = 2
    Containers.scontainer.songInfo_artist.transform.alpha = 0
    
    Containers.scontainer.songInfo_difficulty = text.new(0, 0, "ArialBold.fnt", "Song Diff")
    create(Containers.scontainer.songInfo_difficulty)
    Containers.scontainer.songInfo_difficulty.size = 28 * skin["upscale"]
    Containers.scontainer.songInfo_difficulty.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_difficulty)
    Containers.scontainer.songInfo_difficulty.ratio = true
    Containers.scontainer.songInfo_difficulty.ratio = true
    Containers.scontainer.songInfo_difficulty.center = true
    Containers.scontainer.songInfo_difficulty.transform.x = 0.5
    Containers.scontainer.songInfo_difficulty.transform.y = 0.55
    Containers.scontainer.songInfo_difficulty.tag = "songInfo_difficulty"
    Containers.scontainer.songInfo_difficulty.order = 2
    Containers.scontainer.songInfo_difficulty.transform.alpha = 0

    Containers.scontainer.songInfo_difficultyName = text.new(0, 0, "Arial.fnt", "Song Diff Name")
    create(Containers.scontainer.songInfo_difficultyName)
    Containers.scontainer.songInfo_difficultyName.size = 26 * skin["upscale"]
    Containers.scontainer.songInfo_difficultyName.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_difficultyName)
    Containers.scontainer.songInfo_difficultyName.ratio = true
    Containers.scontainer.songInfo_difficultyName.center = true
    Containers.scontainer.songInfo_difficultyName.transform.x = 0.5
    Containers.scontainer.songInfo_difficultyName.transform.y = 0.5
    Containers.scontainer.songInfo_difficultyName.tag = "songInfo_difficultyName"
    Containers.scontainer.songInfo_difficultyName.order = 2
    Containers.scontainer.songInfo_difficultyName.transform.alpha = 0

    Containers.scontainer.songInfo_difficultyLeftArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow", 0, 0)
    create(Containers.scontainer.songInfo_difficultyLeftArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_difficultyLeftArrow)
    Containers.scontainer.songInfo_difficultyLeftArrow.transform.alpha = 0
    Containers.scontainer.songInfo_difficultyLeftArrow.order = 2
    Containers.scontainer.songInfo_difficultyLeftArrow.tag = "songInfo_difficultyLeftArrow"
    Containers.scontainer.songInfo_difficultyLeftArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.songInfo_difficultyRightArrow = helper.createSprite("Menu/MainMenu/Solo/difficultyselectarrow", 0, 0)
    create(Containers.scontainer.songInfo_difficultyRightArrow)
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_difficultyRightArrow)
    Containers.scontainer.songInfo_difficultyRightArrow.transform.alpha = 0
    Containers.scontainer.songInfo_difficultyRightArrow.transform.angle = 180
    Containers.scontainer.songInfo_difficultyRightArrow.order = 2
    Containers.scontainer.songInfo_difficultyRightArrow.tag = "songInfo_difficultyRightArrow"
    Containers.scontainer.songInfo_difficultyRightArrow.transform.scale = 0.5 * skin["upscale"]

    Containers.scontainer.songInfo_chartType = text.new(0, 0, "Arial.fnt", "Song Chart Type")
    create(Containers.scontainer.songInfo_chartType)
    Containers.scontainer.songInfo_chartType.size = 22 * skin["upscale"]
    Containers.scontainer.songInfo_chartType.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_chartType)
    Containers.scontainer.songInfo_chartType.ratio = true
    Containers.scontainer.songInfo_chartType.transform.x = 0.02
    Containers.scontainer.songInfo_chartType.transform.y = 0.96
    Containers.scontainer.songInfo_chartType.tag = "songInfo_chartType"
    Containers.scontainer.songInfo_chartType.order = 2
    Containers.scontainer.songInfo_chartType.transform.alpha = 0

    Containers.scontainer.songInfo_onlineText = text.new(0, 0, "Arial.fnt", "Song Online Text")
    create(Containers.scontainer.songInfo_onlineText)
    Containers.scontainer.songInfo_onlineText.size = 22 * skin["upscale"]
    Containers.scontainer.songInfo_onlineText.characterSpacing = 2.33
    Containers.scontainer.songInfo:add(Containers.scontainer.songInfo_onlineText)
    Containers.scontainer.songInfo_onlineText.tag = "songInfo_onlineText"
    Containers.scontainer.songInfo_onlineText.order = 2
    Containers.scontainer.songInfo_onlineText.transform.alpha = 0
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

    tween(Containers.solo, Containers.endRect, 1, "outcubic")

    helper.initContainer(Containers.solo, false)
    helper.initContainer(Containers.multiplayer, false)
    helper.initContainer(Containers.settings, true)
end

function Containers.Resize()
end
