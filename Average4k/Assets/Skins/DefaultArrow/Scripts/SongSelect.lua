logo = nil

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

currentView = 0

packs = {}
currentPack = 1

currentCharts = {}

selection = 1

packObjects = {}

function createPacks()
    currentMenu:removeAll()

    logo = Sprite.new(20,20, "Images/Logo.png")

    currentMenu:addObject(logo)

    for i = 1, #packs do
        local p = packs[i]
        local pack = Text.new(20, logo.y + logo.height + 20 + (i * 40), "ArialUnicode.fnt", packs[i].name, 42)

        local bannerId = -1

        if p["banner"] ~= "" then
            bannerId = loadAsyncTexture(p["path"] .. "/" .. p["banner"])
        end

        packObjects[i] = {text = pack, banner = nil, bId = bannerId}

        currentMenu:addObject(pack)
    end
end

function create()
    packs = getPacks()

    createPacks()
end

function keyPress(data)

    if data == 265 then -- up

        selection = selection - 1

        if selection < 1 then
            selection = 1
        end

        
    end

    if data == 264 then -- down
        selection = selection + 1

        if selection > 1 then
            selection = 1
        end
    end


end

function view_packs()

end

function view_charts()

end

function draw()
    if currentView == 0 then
        view_packs()
    else
        view_charts()
    end

    for i = 1, #packObjects do
        local p = packObjects[i]
        if p.banner == nil and p.bId ~= -1 then
            local spr = getAsyncTexture(p.bId)

            if spr.width == 0 and spr.height == 0 then -- doesn't exist yet
                return
            end

            print("banner loaded for " .. p.text.text)

            p.banner = spr
            p.banner.x = p.text.x + p.text.width + 20
            p.banner.height = 38
            p.banner.width = 200
            p.banner.y = p.text.y + (p.text.height / 2) - (p.banner.height / 2)

            currentMenu:addObject(p.banner)
        end
    end
end