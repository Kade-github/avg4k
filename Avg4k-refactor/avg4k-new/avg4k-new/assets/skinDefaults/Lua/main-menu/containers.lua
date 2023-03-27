Containers = {}

function Containers.create()
    local solo = helper.createContainer("container", 0, 0)
    add(solo)
    solo.ratio = true
    solo.transform.x = 50
    solo.center = true
    solo.transform.y = -50
    solo.transform.scale = skin["upscale"] * 0.75

    local endRect = copyRect(solo.transform)
    endRect.y = 55

    tween(solo, endRect, 1, "outcubic")
end