Popup = {}
Popup.backgroundSprite = nil
Popup.text = nil
Popup.tinyText = nil
Popup.half = 0

function Popup.init()
    Popup.backgroundSprite = rectangle.new(0, 0, 18 * skin["upscale"], 60)
    add(Popup.backgroundSprite)
    Popup.backgroundSprite.order = 99
    Popup.backgroundSprite.transform.alpha = 0
    Popup.backgroundSprite.center = true
    Popup.backgroundSprite.tag = "popup_background"
    Popup.backgroundSprite.transform.w = 0.2
    Popup.backgroundSprite.transform.h = 0.05

    Popup.text = text.new(0, 0, "ArialBold.fnt", "")
    create(Popup.text)
    Popup.backgroundSprite:add(Popup.text)
    Popup.text.order = 1
    Popup.text.transform.alpha = 0
    Popup.text.tag = "popup_text"
    Popup.text.size = 24 * skin["upscale"]
    Popup.text.center = true

    Popup.tinyText = text.new(0, 0, "ArialItalic.fnt", "")
    create(Popup.tinyText)
    Popup.backgroundSprite:add(Popup.tinyText)
    Popup.tinyText.order = 1
    Popup.tinyText.transform.alpha = 0
    Popup.tinyText.tag = "popup_tiny_text"
    Popup.tinyText.size = 16 * skin["upscale"]
    Popup.tinyText.center = true

    Popup.backgroundSprite.ratio = true
    Popup.text.ratio = true
    Popup.tinyText.ratio = true

    Popup.backgroundSprite.transform.r = 0
    Popup.backgroundSprite.transform.g = 0
    Popup.backgroundSprite.transform.b = 0

    Popup.backgroundSprite.transform.x = 0.5
    Popup.backgroundSprite.transform.y = 0.5
    Popup.text.transform.x = 0.5
    Popup.text.transform.y = 0.35
    Popup.tinyText.transform.x = 0.5
    Popup.tinyText.transform.y = 0.7

    Popup.half = tonumber(settings["Start Offset"]) / 2
end

function endPopupTween()
    local e = copyRect(Popup.backgroundSprite.transform)
    local e2 = copyRect(Popup.text.transform)
    local e3 = copyRect(Popup.tinyText.transform)
    e.alpha = 0
    e2.alpha = 0
    e3.alpha = 0

    tween(Popup.backgroundSprite, e, Popup.half, "inexpo", "")
    tween(Popup.text, e2, Popup.half, "inexpo", "")
    tween(Popup.tinyText, e3, Popup.half, "inexpo", "")
end

function Popup.showPopup(title, tiny)
    local e = copyRect(Popup.backgroundSprite.transform)
    local e2 = copyRect(Popup.text.transform)
    local e3 = copyRect(Popup.tinyText.transform)
    e.alpha = 0.8
    e2.alpha = 1
    e3.alpha = 1
    tween(Popup.backgroundSprite, e, Popup.half, "outcubic", "endPopupTween")
    tween(Popup.text, e2, Popup.half, "outcubic", "")
    tween(Popup.tinyText, e3, Popup.half, "outcubic", "")
    Popup.text.text = title
    Popup.tinyText.text = tiny
end
