packContainer = {}
packContainer.sprites = {}

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

function packContainer.loadPacks(c)
    packContainer.main = c
    Containers.scontainer.packsInfo.text = "Loaded " .. #packs .. " packs!"
    local real = c:getRealRect()

    -- add all the packs from sprites
    for i = 1, #packContainer.sprites, 1 do
        local s = packContainer.sprites[i]
        c:add(s)
        s.transform.x = 2
        s.transform.y = (75 * (i - 1)) + 2
        s.transform.w = real.w - 2
        s.transform.h = 73
    end
end
