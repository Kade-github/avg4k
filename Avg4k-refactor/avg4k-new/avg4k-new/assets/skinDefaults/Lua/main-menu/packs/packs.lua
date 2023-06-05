packContainer = {}
packContainer.sprites = {}
packContainer.hitboxes = {}
packContainer.selectedIndex = -1
packContainer.last = nil

function packContainer.loadBanners()
    -- print all the packs
    for i = 1, #packs, 1 do
        local p = packs[i]
        local banner = helper.createChartSprite(p.path .. '/' .. p.banner, 0, 0)
        create(banner)
        banner.tag = p.name .. "-banner"
        banner.order = 1

        table.insert(packContainer.sprites, banner)
    end
end

function packContainer.mouseDown()
    for i = 1, #packContainer.hitboxes, 1 do
        local hitbox = packContainer.hitboxes[i]
        local real = hitbox:getRealRect()
        if aabb_rect(Globals.mouseRect, real, false) then
            cprint("Selected pack " .. tostring(i))
            packContainer.selectedIndex = i
            hitbox.transform.alpha = 0.2
            songWheel.setSongs(packs[i].files)
            if last ~= nil then
                last.transform.alpha = 0
            end
            songWheel.Select(0)
            last = hitbox
        end
    end
end

function packContainer.update(time)
    local c = packContainer.main
    local real = c:getRealRect()
    for i = 1, #packContainer.sprites, 1 do
        local s = packContainer.sprites[i]
        s.transform.x = 2
        s.transform.y = ((75 * skin["upscale"]) * (i - 1)) + 2
        s.transform.w = real.w
        s.transform.h = 75 * skin["upscale"]
    end
end

function packContainer.loadPacks(c)
    packContainer.main = c
    Containers.scontainer.packsInfo.text = "Loaded " .. #packs .. " packs!"
    local real = c:getRealRect()

    -- add all the packs from sprites
    for i = 1, #packContainer.sprites, 1 do
        local s = packContainer.sprites[i]
        c:add(s)
        s.transform.x = 2
        s.transform.y = ((75 * skin["upscale"]) * (i - 1)) + 2
        s.transform.w = real.w
        s.transform.h = 75

        local hitbox = rectangle.new(0, 0, real.w, 73)
        create(hitbox)
        hitbox.tag = "hitbox"
        hitbox.order = 2
        hitbox.transform.alpha = 0
        hitbox.transform.scale = skin["upscale"]
        s:add(hitbox)

        table.insert(packContainer.hitboxes, hitbox)
    end
end
