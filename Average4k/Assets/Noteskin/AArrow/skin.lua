function setup()
    setNoteSize(256,256)

    centerX = (1920 / 2) * getWidthScale()

    centerY = (1080 / 2) * getHeightScale()

    local comboText = Text.new(24, 24, "FuturaBoldOutlined.fnt", "Combo: 0", math.floor(38 * getHeightScale()))

    comboText.y = centerY
    comboText.x = centerX

    comboText.center = true

    comboText.tag = "combo"

    currentMenu:addObject(comboText)

    local judgementText = Text.new(0,0, "FuturaBoldOutlined.fnt", "judge", math.floor(38 * getHeightScale()))

    judgementText.center = true

    judgementText.tag = "judgement"

    judgementText.y = centerY - math.floor(38 * getHeightScale())
    judgementText.x = centerX

    currentMenu:addObject(judgementText)

    setJudgementTextTag("judgement")

    setComboTag("combo")

    local judgementHeader = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()), "FuturaBoldOutlined.fnt", "Judgements", math.floor(42 * getHeightScale()))

    currentMenu:addObject(judgementHeader)

    local marvelousText = Text.new(24, judgementHeader.y + judgementHeader.height + math.floor(10 * getHeightScale()), "FuturaBoldOutlined.fnt", "Marvelous: 0", math.floor(24 * getHeightScale()))

    marvelousText.tag = "marv"

    currentMenu:addObject(marvelousText)

    local perfectText = Text.new(24, marvelousText.y + marvelousText.height + math.floor(10 * getHeightScale()), "FuturaBoldOutlined.fnt", "Perfect: 0", math.floor(24 * getHeightScale()))

    perfectText.tag = "perf"

    currentMenu:addObject(perfectText)

    local greatText = Text.new(24, perfectText.y + perfectText.height + math.floor(10 * getHeightScale()), "FuturaBoldOutlined.fnt", "Great: 0", math.floor(24 * getHeightScale()))

    greatText.tag = "great"

    currentMenu:addObject(greatText)

    local goodText = Text.new(24, greatText.y + greatText.height + math.floor(10 * getHeightScale()), "FuturaBoldOutlined.fnt", "Good: 0", math.floor(24 * getHeightScale()))

    goodText.tag = "good"

    currentMenu:addObject(goodText)

    local badText = Text.new(24, goodText.y + goodText.height + math.floor(10 * getHeightScale()), "FuturaBoldOutlined.fnt", "Bad: 0", math.floor(24 * getHeightScale()))

    badText.tag = "bad"

    currentMenu:addObject(badText)

    setJudgementTag("marvelous", "marv")
    setJudgementTag("perfect", "perf")
    setJudgementTag("great", "great")
    setJudgementTag("good", "good")
    setJudgementTag("bad", "bad")
    setJudgementTag("miss", "miss")
end

function receptorSetup(rec, w, h)
    rec:setSrcRec(6 * w, h, w, h)
end