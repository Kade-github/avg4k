checkbox = {}

checkbox.checkboxes = {}

--[[
container - the container that the checkbox will be added to [if nil, it will be added to the screen parent] (gameObject)


_setting - the setting that the checkbox will change (string)

_tag - the tag of the checkbox (string)

tinyPos - the position of the checkbox (table of 2 numbers. x and y)

_changeFunction - the function that will be called when the checkbox is changed (function)
]]
function checkbox.CreateCheckbox(container, _setting, _tag, tinyPos, _changeFunction)
    -- create a table, kind of like a class type thing.
    -- it's pretty simple.

    local bTable = {
        currentValue = settings[_setting],
        tag = _tag,
        setting = _setting,
        objects = {},
        changeFunction = _changeFunction,
        position = rect.new(0, 0),
        startTime = 0,
        startPos = 0
    }

    local background = helper.createSprite("Menu/MainMenu/Settings/toggle_bg", 0.0, 0.0)
    if container ~= nil then
        create(background)
        container:add(background)
    else
        add(background)
    end
    background.tag = "CHECKBOX_BG_" .. _tag
    background.ratio = true
    background.order = 1

    bTable.position = copyRect(background.transform)
    bTable.position.x = tinyPos[1]
    bTable.position.y = tinyPos[2]


    local toggle = helper.createSprite("Menu/MainMenu/Settings/toggle", 0.0, 0.0)
    create(toggle)
    background:add(toggle)
    toggle.tag = "CHECKBOX_TOGGLE_" .. _tag
    toggle.ratio = true
    toggle.order = 1
    toggle.transform.w = 0.5
    toggle.transform.h = 1

    table.insert(bTable.objects, background)
    table.insert(bTable.objects, toggle)
    table.insert(checkbox.checkboxes, bTable)
end

function checkbox.checked(cb)
    cprint("[Settings] " .. cb.tag .. " = " .. tostring(cb.currentValue))
    cb.startPos = cb.position.x
    cb.currentValue = not cb.currentValue
    cb.startTime = Globals.lt
    if cb.changeFunction ~= nil then
        cb.changeFunction(cb.currentValue)
    end
end

function checkbox.mouseDown(pos)
    for i, cb in ipairs(checkbox.checkboxes) do
        if helper.aabb(pos, cb.objects[1]:getRealRect()) then
            checkbox.checked(cb)
            return
        end
    end
end

function checkbox.update(time)
    for i, cb in ipairs(checkbox.checkboxes) do
        cb.objects[1].transform = cb.position
        -- simple tweening
        local tweenTime = math.min((time - cb.startTime) / 0.1, 1)
        if cb.currentValue then
            cb.objects[2].transform.x = helper.lerp(cb.startPos, 0, tweenTime)
        else
            local endPos = 1 - cb.objects[2].transform.w
            cb.objects[2].transform.x = helper.lerp(cb.startPos, endPos,
                tweenTime)
        end
    end
end

function checkbox.GetCheckbox(_tag)
    for i, cb in ipairs(checkbox.checkboxes) do
        if cb.tag == _tag then
            return cb
        end
    end
    return nil
end
