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
    Containers.scontainer.songWheel.transform.w = 0.6
    Containers.scontainer.songWheel.transform.alpha = 0

    songWheel.init(Containers.scontainer.songWheel)
    helper.initContainer(Containers.scontainer.songWheel, false)
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
