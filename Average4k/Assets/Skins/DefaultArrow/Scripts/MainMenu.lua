function create()
    print('hello world')

    local s = Sprite.new(25,25, "Images/Logo.png")

    print("created sprite")

    print(s.transform.w .. " " .. s.transform.h .. " " .. s.texture.path)

    menu:addObject(s)

    print('hello world 2')
end