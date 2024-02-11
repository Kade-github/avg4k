function setup()
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

    local accuracy = Text.new(24, 24, "FuturaBoldOutlined.fnt", "100%", math.floor(42 * getHeightScale()))

    accuracy.tag = "accuracy"

    currentMenu:addObject(accuracy)

    local judgementHeader = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()), "FuturaBoldOutlined.fnt", "Judgements", math.floor(42 * getHeightScale()))

    judgementHeader.tag = "_ui_judgement_header"

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

    setNoteSize(256,256)

    setAccuracyTag("accuracy")
    setJudgementTextTag("judgement")
    setComboTag("combo")

    setJudgementTag("marvelous", "marv")
    setJudgementTag("perfect", "perf")
    setJudgementTag("great", "great")
    setJudgementTag("good", "good")
    setJudgementTag("bad", "bad")
    setJudgementTag("miss", "miss")
end

function receptorSetup(rec, w, h)
    -- index 2,4
    rec:setSrcRec(4 * w, 2 * h, w, h)
end

function noteSetup(n, w, h, beat, type, lane)
    -- figure out what quant its at
    local row = 0
    local col = 0

    local beatRow = beat * 48

    if type == 1 then
        -- special hold note functions, for it's body and end sprites
        n.row = 0
        n.col = 3
        n.endRow = 2
        n.endCol = 2
    elseif type == 4 then 
        -- mine

        n:addOverlay(3,2) -- spin

        row = 0
        col = 4
        n:setSrcRec(col * w, row * h, w, h)
        return
    end

    if math.fmod(beatRow, 192 / 4) == 0 then -- 4th
        row = 1
        col = 1
    elseif math.fmod(beatRow, 192 / 8) == 0 then -- 8th
        row = 2
        col = 1
    elseif math.fmod(beatRow, 192 / 12) == 0 or math.fmod(beatRow, 192 / 24) == 0 then -- 12th/24th
        row = 0
        col = 0
    elseif math.fmod(beatRow, 192 / 16) == 0 then -- 16th
        row = 1
        col = 0
    elseif math.fmod(beatRow, 192 / 32) == 0 then -- 32nd
        row = 0
        col = 1
    elseif math.fmod(beatRow, 192 / 64) == 0 then -- 64th
        row = 0
        col = 2
    end

    if type == 5 then
        row = 1
        col = 2
    end

    n:setSrcRec(col * w, row * h, w, h)

    if lane == 0 then
        n.angle = 90
    end
    if lane == 2 then
        n.angle = 180
    end
    if lane == 3 then
        n.angle = -90
    end

end

function overlayUpdate(type, curAngle, curOpacity)
    if type == 4 then -- spinning
        return {curAngle + 1, curOpacity}
    end
    return {curAngle, curOpacity}
end