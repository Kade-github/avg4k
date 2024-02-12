local accText = nil
local comboText = nil

local judgeText = nil

local marvTex = nil
local perfTex = nil
local greatTex = nil
local goodTex = nil
local badTex = nil

function setup()
    centerX = (1920.0 / 2.0) * getWidthScale()

    centerY = (1080.0 / 2.0) * getHeightScale()

    comboText = Text.new(24, 24, "FuturaBoldOutlined.fnt", "Combo: 0", math.floor(38 * getHeightScale()))

    comboText.y = centerY
    comboText.x = centerX

    comboText.center = true

    comboText.tag = "combo"

    currentMenu:addObject(comboText)

    judgeText = Text.new(0,0, "FuturaBoldOutlined.fnt", "judge", math.floor(38 * getHeightScale()))

    judgeText.center = true

    judgeText.tag = "judgement"

    judgeText.y = centerY - math.floor(38 * getHeightScale())
    judgeText.x = centerX

    currentMenu:addObject(judgeText)

    accText = Text.new(24, 24, "FuturaBoldOutlined.fnt", "100%", math.floor(42 * getHeightScale()))

    accText.tag = "accuracy"

    currentMenu:addObject(accText)

    local judgementHeader = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()), "FuturaBoldOutlined.fnt", "Judgements", math.floor(42 * getHeightScale()))

    judgementHeader.tag = "_ui_judgement_header"

    currentMenu:addObject(judgementHeader)

    marvTex = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()) + judgementHeader.height, "FuturaBoldOutlined.fnt", "Marvelous: 0", math.floor(38 * getHeightScale()))

    marvTex.tag = "_ui_marv"

    currentMenu:addObject(marvTex)

    perfTex = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()) + judgementHeader.height + marvTex.height, "FuturaBoldOutlined.fnt", "Perfect: 0", math.floor(38 * getHeightScale()))

    perfTex.tag = "_ui_perf"

    currentMenu:addObject(perfTex)

    greatTex = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()) + judgementHeader.height + marvTex.height + perfTex.height, "FuturaBoldOutlined.fnt", "Great: 0", math.floor(38 * getHeightScale()))

    greatTex.tag = "_ui_great"

    currentMenu:addObject(greatTex)

    goodTex = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()) + judgementHeader.height + marvTex.height + perfTex.height + greatTex.height, "FuturaBoldOutlined.fnt", "Good: 0", math.floor(38 * getHeightScale()))

    goodTex.tag = "_ui_good"

    currentMenu:addObject(goodTex)

    badTex = Text.new(24, 24 + comboText.height + math.floor(200 * getHeightScale()) + judgementHeader.height + marvTex.height + perfTex.height + greatTex.height + goodTex.height, "FuturaBoldOutlined.fnt", "Bad: 0", math.floor(38 * getHeightScale()))

    badTex.tag = "_ui_bad"

    currentMenu:addObject(badTex)

    setNoteSize(256,256)
end

function start()
    accText.text = "100%"
    comboText.text = ""

    marvTex.text = "Marvelous: 0"
    perfTex.text = "Perfect: 0"
    greatTex.text = "Great: 0"
    goodTex.text = "Good: 0"
    badTex.text = "Bad: 0"

    judgeText.text = ""
end

function hitNote(type, lane, judge, combo, acc)

    accText.text = string.format("%.2f", acc * 100.0) .. "%"

    comboText.text = tostring(combo)

    judgeText.text = judge

    print(judge)

    if judge == "Marvelous" then
        marvTex.text = "Marvelous: " .. (tonumber(marvTex.text:sub(12)) + 1)
        judgeText:setColor(73,247,255, 1)
    elseif judge == "Perfect" then
        perfTex.text = "Perfect: " .. (tonumber(perfTex.text:sub(9)) + 1)
        judgeText:setColor(255,252,0, 1)
    elseif judge == "Great" then
        greatTex.text = "Great: " .. (tonumber(greatTex.text:sub(7)) + 1)
        judgeText:setColor(54,255,0, 1)
    elseif judge == "Good" then
        goodTex.text = "Good: " .. (tonumber(goodTex.text:sub(6)) + 1)
        judgeText:setColor(255,148,0, 1)
    elseif judge == "Bad" then
        badTex.text = "Bad: " .. (tonumber(badTex.text:sub(5)) + 1)
        judgeText:setColor(83,0,0, 1)
    end
end

function missNote(combo, acc)
    comboText.text = "0"

    judgeText.text = "Miss"
    judgeText:setColor(255,0,0, 1)

    accText.text = string.format("%.2f", acc * 100.0) .. "%"
end

function receptorSetup(rec, w, h)
    -- index 2,4
    rec:setSrcRec(4 * w, 2 * h, w, h)
end

function receptorHit(rec, w, h)
    rec:setSrcRec(3 * w, 1 * h, w, h)
    rec.scale = 0.95
end

function receptorRelease(rec, w, h)
    rec:setSrcRec(4 * w, 2 * h, w, h)
    rec.scale = 1
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