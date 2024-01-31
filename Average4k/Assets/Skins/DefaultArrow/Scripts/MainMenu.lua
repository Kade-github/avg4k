function create()
    print('hello world')

    local s = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(s)

end

function draw()
    --print(tostring(mouse.x) .. " " .. tostring(mouse.y))
end