-- helper files
HelperFiles = { 'popup.lua' }

initialRect = nil
comboInitialRect = nil
judgement = nil
underlane = nil
accuracy = nil
grade = nil
combo = nil

judge_counter_marv = nil
judge_counter_perf = nil
judge_counter_great = nil
judge_counter_good = nil
judge_counter_bad = nil
judge_counter_miss = nil

function Create()
    cprint(jit.version)
    dofile(resource("Lua/helper-functions.lua"))
    for i = 1, #HelperFiles, 1 do
        dofile(resource("Lua/gameplay/" .. HelperFiles[i]))
    end

    -- load background

    local mainFolder = options["chart"]["folder"] .. '/'

    local bg = helper.createChartSprite(mainFolder .. options["chart"]["background"], 0.0, 0.0)
    add(bg)
    bg.ratio = true
    bg.transform.w = 1
    bg.transform.h = 1

    bg.order = -2

    bg.transform.alpha = tonumber(settings["Background Transparency"])

    setObject(bg, 0) -- tell avg4k that this is the background

    judgement = helper.createAnimatedSpriteSkin("judgements/judgements", 0, 0, 0, 210, 85, nil)

    judgement.transform.x = display["width"] / 2
    judgement.transform.y = display["height"] / 2
    judgement.transform.alpha = 0
    judgement.order = 5
    judgement.center = true

    combo = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo.size = 24 * skin["upscale"]
    combo.transform.x = display["width"] / 2
    combo.transform.y = (display["height"] / 2) + (judgement.transform.h / 2)
    combo.transform.alpha = 0
    combo.order = 5
    combo.center = true

    add(combo)

    setObject(combo, 3) -- tell avg4k that this is the combo

    initialRect = copyRect(judgement.transform)
    initialRect.alpha = 1

    comboInitialRect = copyRect(combo.transform)
    comboInitialRect.alpha = 1

    local noteSize = tonumber(settings["Note Size"])

    local underlaneX = (display["width"] / 2) - ((64 * noteSize) * 2)
    underlane = rectangle.new(0, 0, (64 * noteSize) * 4, display["height"])
    underlane.transform.x = underlaneX
    underlane.order = -1
    underlane.transform.alpha = tonumber(settings["Underlane Transparency"])
    underlane.transform.r = tonumber(settings["Underlane R Color"])
    underlane.transform.g = tonumber(settings["Underlane G Color"])
    underlane.transform.b = tonumber(settings["Underlane B Color"])
    add(underlane)

    accuracy = text.new(0, 4, "FuturaBoldOutlined.fnt", "100%")
    accuracy.size = 46 * skin["upscale"]
    add(accuracy)

    grade = text.new(0, 0, "FuturaBoldOutlined.fnt", "A+")
    grade.size = 46 * skin["upscale"]
    add(grade)

    setObject(accuracy, 1) -- tell avg4k that this is the accuracy
    setObject(grade, 2)    -- tell avg4k that this is the grade

    -- create combo counter

    combo_cnt_marv = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_marv.size = 24 * skin["upscale"]
    combo_cnt_marv.transform.x = 8
    combo_cnt_marv.order = 5
    add(combo_cnt_marv)

    combo_cnt_perf = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_perf.size = 24 * skin["upscale"]
    combo_cnt_perf.transform.x = 8
    combo_cnt_perf.order = 5
    add(combo_cnt_perf)

    combo_cnt_great = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_great.size = 24 * skin["upscale"]
    combo_cnt_great.transform.x = 8
    combo_cnt_great.order = 5
    add(combo_cnt_great)

    combo_cnt_good = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_good.size = 24 * skin["upscale"]
    combo_cnt_good.transform.x = 8
    combo_cnt_good.order = 5
    add(combo_cnt_good)

    combo_cnt_bad = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_bad.size = 24 * skin["upscale"]
    combo_cnt_bad.transform.x = 8
    combo_cnt_bad.order = 5
    add(combo_cnt_bad)

    combo_cnt_miss = text.new(0, 0, "FuturaBoldOutlined.fnt", "0")
    combo_cnt_miss.size = 24 * skin["upscale"]
    combo_cnt_miss.transform.x = 8
    combo_cnt_miss.order = 5
    add(combo_cnt_miss)

    setObject(combo_cnt_marv, 4)  -- tell avg4k that this is the combo cnt for marvelous
    setObject(combo_cnt_perf, 5)  -- tell avg4k that this is the combo cnt for perfect
    setObject(combo_cnt_great, 6) -- tell avg4k that this is the combo cnt for great
    setObject(combo_cnt_good, 7)  -- tell avg4k that this is the combo cnt for good
    setObject(combo_cnt_bad, 8)   -- tell avg4k that this is the combo cnt for bad
    setObject(combo_cnt_miss, 9)  -- tell avg4k that this is the combo cnt for miss


    Popup.init()
    Popup.showPopup(options["chart"]["title"], options["chart"]["diff"] .. " charted by " .. options["chart"]["charter"])
end

function Update(time)
    accuracy.transform.x = display["width"] - (accuracy.transform.w + 8)
    grade.transform.y = 8 + (accuracy.transform.h)
    grade.transform.x = accuracy.transform.x + accuracy.transform.w - grade.transform.w;

    combo_cnt_marv.transform.y = (display["height"] / 2) - (combo_cnt_marv.transform.h * 4)
    combo_cnt_perf.transform.y = (display["height"] / 2) - (combo_cnt_perf.transform.h * 3)
    combo_cnt_great.transform.y = (display["height"] / 2) - (combo_cnt_great.transform.h * 2)
    combo_cnt_good.transform.y = (display["height"] / 2) - (combo_cnt_good.transform.h)
    combo_cnt_bad.transform.y = (display["height"] / 2)
    combo_cnt_miss.transform.y = (display["height"] / 2) + (combo_cnt_miss.transform.h)
end

function ArrowJudged(judge)
    judgement.frame = tonumber(judge)
    judgement.transform.alpha = 1
    combo.transform.alpha = 1

    local newRect = copyRect(initialRect)
    newRect.scale = 1.1

    judgement.transform = newRect

    tween(judgement, initialRect, 0.2, "outcubic", "")

    local newComboRect = copyRect(comboInitialRect)
    newComboRect.scale = 1.1

    combo.transform = newComboRect

    tween(combo, comboInitialRect, 0.2, "outcubic", "")
end
