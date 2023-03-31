Containers = {}

function Containers.create()
    Containers.solo = helper.createContainer("container", 0, 0)
    add(Containers.solo)
    Containers.solo.ratio = true
    Containers.solo.transform.x = 50
    Containers.solo.center = true
    Containers.solo.transform.y = -50
    Containers.solo.transform.scale = skin["upscale"]

    Containers.endRect = copyRect(Containers.solo.transform)
    Containers.endRect.y = 55

    tween(Containers.solo, Containers.endRect, 1, "outcubic")
end

function Containers.Resize()
    Containers.solo.transform.scale = skin["upscale"]
    cprint("resized container")
end