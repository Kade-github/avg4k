Containers = {}

function Containers.create()
    Containers.solo = helper.createContainer("container", 0, 0)
    add(Containers.solo)
    Containers.solo.ratio = true
    Containers.solo.transform.x = 0.5
    Containers.solo.center = true
    Containers.solo.transform.y = -0.5
    Containers.solo.transform.scale = skin["upscale"]
    
    Containers.endRect = copyRect(Containers.solo.transform)
    Containers.endRect.y = 0.5

    tween(Containers.solo, Containers.endRect, 1, "outcubic")
end

function Containers.Resize()
end