local logo = nil
local back = nil
local workshop = nil

local view = {}

local selection = 1
local page = 1
local inSubmenu = false
local submenuIndex = 1

local isUploading = false

local hasnotuploaded = true

-- workshop details

local cFolder = ""
local cThumb = ""
local cTitle = ""
local cDescription = ""
local cDifficulty = ""
local cTags = {}
local cPreviews = {}

local width = 0
local height = 0

function containsValue(table, value)
    for i = 1, #table do
        if table[i] == value then
            return true
        end
    end
    return false
end

function findValue(table, value)
    for i = 1, #table do
        if table[i] == value then
            return i
        end
    end
    return 0
end


function createView(list)
    currentMenu:removeAll()

    logo = Sprite.new(20, 20, "Images/Logo.png")

    logo.width = math.floor(700 * getWidthScale())
    logo.height = math.floor(314 * getHeightScale())

    currentMenu:addObject(logo)

    local name = getSteamName()

    back = Text.new(20, logo.y + logo.height + math.floor(20 * getHeightScale()), "Arial.fnt", "Press escape to go back (Logged in as " .. name .. ")", math.floor(42 * getHeightScale()))

    currentMenu:addObject(back)

    workshop = Text.new(20, back.y + back.height + math.floor(42 * getHeightScale()), "Arial.fnt", "Average4K Workshop", math.floor(52 * getHeightScale()))

    currentMenu:addObject(workshop)

    view = {}
    for i = 1, #list do
        local t = Text.new(20, (workshop.y + workshop.height + (40 * getHeightScale())) + (i - 1) * (60 * getHeightScale()), "Arial.fnt", list[i], (42 * getHeightScale()))

        table.insert(view, t)

        currentMenu:addObject(t)
    end

    selection = 1

    view[selection].text = "> " .. view[selection].text
end

function Page1()
    hasnotuploaded = true
    page = 1

    -- reset workshop details

    cFolder = ""
    cThumb = ""
    cTitle = ""
    cDescription = ""
    cDifficulty = ""
    cTags = {}
    cPreviews = {}

    createView({"Upload a Pack", "Upload a Noteskin", "Upload a Theme", "Browse the Workshop"})
end

function Page2()
    page = 2

    createView({"Select folder", "Select thumbnail", "Set title", "Set description", "Overall difficulty", "Special tags", "Upload"})
end

function Page3()
    page = 3

    createView({"Select folder", "Select thumbnail", "Set title", "Set description", "Select preview pictures", "Tags", "Upload"})
end

function Page4()
    page = 4

    createView({"Select folder", "Select thumbnail", "Set title", "Set description", "Select preview pictures", "Upload"})
end

function Folder()
    inSubmenu = true

    createView({"Browse"})

    local folder = Text.new(20, view[1].y + view[1].height + math.floor(40 * getHeightScale()), "Arial.fnt", "\"" .. cFolder .. "\"", math.floor(42 * getHeightScale()))

    currentMenu:addObject(folder)
end

function Thumb()
    inSubmenu = true

    createView({"Browse"})

    local folder = Text.new(20, view[1].y + view[1].height + math.floor(40 * getHeightScale()), "Arial.fnt", "\"" .. cThumb .. "\"", math.floor(42 * getHeightScale()))

    currentMenu:addObject(folder)
end

function Title()
    inSubmenu = true

    createView({"Please start typing:"})

    local folder = Text.new(20, view[1].y + view[1].height + math.floor(40 * getHeightScale()), "Arial.fnt", cTitle .. "_", math.floor(42 * getHeightScale()))

    currentMenu:addObject(folder)
end

function Description()
    inSubmenu = true

    createView({"Please start typing:"})

    local folder = Text.new(20, view[1].y + view[1].height + math.floor(40 * getHeightScale()), "Arial.fnt", cDescription .. "_", math.floor(42 * getHeightScale()))

    currentMenu:addObject(folder)
end

function Diff()
    inSubmenu = true

    local selectedDiff = cDifficulty

    createView({"Beginner [" .. tostring(selectedDiff == "Beginner") .. "]", "Easy [" .. tostring(selectedDiff == "Easy") .. "]", "Medium [" .. tostring(selectedDiff == "Medium") .. "]", "Hard [" .. tostring(selectedDiff == "Hard") .. "]", "Challenge [" .. tostring(selectedDiff == "Challenge") .. "]", "Edit [" .. tostring(selectedDiff == "Edit") .. "]"})
end

function Tags_Pack()
    inSubmenu = true

    createView({"Contains Modfiles [" .. tostring(containsValue(cTags, "Modfiles")) .. "]", "Contains XMod files [" .. tostring(containsValue(cTags, "XMod")) .. "]"})
end


function Tags_Noteskin()
    inSubmenu = true

    createView({"Made for Downscroll [" .. tostring(containsValue(cTags, "Downscroll")) .. "]", "Made for Upscroll [" .. tostring(containsValue(cTags, "Upscroll")) .. "]"})
end

function Previews()
    inSubmenu = true

    createView({"Add preview (Browse)"})

    for i= 1, #cPreviews do
        local t = Text.new(20, view[1].y + view[1].height + (i - 1) * (60 * getHeightScale()), "Arial.fnt", cPreviews[i], (42 * getHeightScale()))

        currentMenu:addObject(t)
    end
end

function CreatePack()
    table.insert(cTags, "Packs")

    if cDifficulty == "" then
        cDifficulty = "Beginner"
    end

    table.insert(cTags, cDifficulty)
    uploadPack(cFolder, cThumb, cTitle, cDescription, cTags)
    hasnotuploaded = false
    isUploading = true
end

function CreateNoteskin()
    table.insert(cTags, "Noteskins")
    
    uploadNoteskin(cFolder, cThumb, cTitle, cDescription, cPreviews, cTags)
    hasnotuploaded = false
    isUploading = true
end

function CreateTheme()
    table.insert(cTags, "Themes")

    uploadTheme(cFolder, cThumb, cTitle, cDescription, cPreviews, cTags)
    hasnotuploaded = false
    isUploading = true
end

function create()

    local vData = getVideoData()

    width = vData["width"]
    height = vData["height"]

    Page1()
end

function up()
    view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text))

    selection = selection - 1

    if selection < 1 then
        selection = #view
    end

    view[selection].text = "> " .. view[selection].text
end

function down()
    view[selection].text = string.sub(view[selection].text, 3, string.len(view[selection].text))

    selection = selection + 1

    if selection > #view then
        selection = 1
    end

    view[selection].text = "> " .. view[selection].text
end

function enter()
    if page == 1 then
        if selection == 1 then
            createItem()
            Page2()
        end
        if selection == 2 then
            createItem()
            Page3()
        end
        if selection == 3 then
            createItem()
            Page4()
        end
        if selection == 4 then
            openWorkshop()
        end
    else
        if inSubmenu then
            if submenuIndex == 1 then
                cFolder = getFolderFromUser()
                Folder()
            end
            if submenuIndex == 2 then
                cThumb = getFileFromUser("Image Files (*.png, *.jpg)\0*.png;*.jpg\0")
                Thumb()
            end

            if submenuIndex == 3 then
                cTitle = cTitle .. "\n"
            end

            if submenuIndex == 4 then
                cDescription = cDescription .. "\n"
            end

            if page == 2 then
                if submenuIndex == 5 then
                    if selection == 1 then
                        cDifficulty = "Beginner"
                    end
                    if selection == 2 then
                        cDifficulty = "Easy"
                    end
                    if selection == 3 then
                        cDifficulty = "Medium"
                    end
                    if selection == 4 then
                        cDifficulty = "Hard"
                    end
                    if selection == 5 then
                        cDifficulty = "Challenge"
                    end
                    if selection == 6 then
                        cDifficulty = "Edit"
                    end
                    Diff()
                end

                if submenuIndex == 6 then
                    if selection == 1 then
                        -- if it exists
                        if containsValue(cTags, "Modfiles") then
                            table.remove(cTags, findValue(cTags, "Modfiles"))
                        else
                            table.insert(cTags, "Modfiles")
                        end
                    end
                    if selection == 2 then
                        -- if it exists
                        if containsValue(cTags, "XMod") then
                            table.remove(cTags, findValue(cTags, "XMod"))
                        else
                            table.insert(cTags, "XMod")
                        end
                    end
                    Tags_Pack()
                end
            end

            if page == 3 then
                if submenuIndex == 5 then
                    if #cPreviews < 5 then
                        if selection == 1 then
                            table.insert(cPreviews, getFileFromUser("Image Files (*.png, *.jpg)\0*.png;*.jpg\0"))
                        end
                    end
                    Previews()
                end

                if submenuIndex == 6 then
                    if selection == 1 then
                        -- if it exists
                        if containsValue(cTags, "Downscroll") then
                            table.remove(cTags, findValue(cTags, "Downscroll"))
                        else
                            table.insert(cTags, "Downscroll")
                        end
                    end
                    if selection == 2 then
                        -- if it exists
                        if containsValue(cTags, "Upscroll")  then
                            table.remove(cTags, findValue(cTags, "Upscroll"))
                        else
                            table.insert(cTags, "Upscroll")
                        end
                    end
                    Tags_Noteskin()
                end
            end

            if page == 4 then
                if submenuIndex == 5 then
                    if #cPreviews < 5 then
                        if selection == 1 then
                            table.insert(cPreviews, getFileFromUser("Image Files (*.png, *.jpg)\0*.png;*.jpg\0"))
                        end
                    end
                    Previews()
                end
            end
        else
            if selection == 1 then
                inSubmenu = true
                submenuIndex = 1
                Folder()
            end
            if selection == 2 then
                inSubmenu = true
                submenuIndex = 2
                Thumb()
            end
    
            if selection == 3 then
                inSubmenu = true
                submenuIndex = 3
                Title()
            end
    
            if selection == 4 then
                inSubmenu = true
                submenuIndex = 4
                Description()
            end
    
            if page == 2 then
                if selection == 5 then
                    inSubmenu = true
                    submenuIndex = 5
                    Diff()
                end
                if selection == 6 then
                    inSubmenu = true
                    submenuIndex = 6
                    Tags_Pack()
                end
            end
    
            if page == 3 then
                if selection == 5 then
                    inSubmenu = true
                    submenuIndex = 5
                    Previews()
                end
                if selection == 6 then
                    inSubmenu = true
                    submenuIndex = 6
                    Tags_Noteskin()
                end
            end
    
            if page == 4 then
                if selection == 5 then
                    inSubmenu = true
                    submenuIndex = 5
                    Previews()
                end
            end

            if page == 2 then
                if selection == 7 then
                    CreatePack()
                end
            end
            if page == 3 then
                if selection == 7 then
                    CreateNoteskin()
                end
            end
            if page == 4 then
                if selection == 6 then
                    CreateTheme()
                end
            end
        end
        
        
    end
    

end

function charInput(data)
    if inSubmenu then
        if submenuIndex == 3 then
            cTitle = cTitle .. string.char(data)

            if string.len(cTitle) > 60 then
                cTitle = string.sub(cTitle, 1, 20)
            end
            Title()
        end
        if submenuIndex == 4 then
            cDescription = cDescription .. string.char(data)

            if string.len(cDescription) > 200 then
                cDescription = string.sub(cDescription, 1, 200)
            end
            Description()
        end
    end
end

local holdingBackspace = false

function keyPress(data)

    if isUploading then
        return
    end

    -- escape

    if data == 256 then
        if page == 1 then
            switchTo("Scripts/MainMenu.lua")
        else
            if inSubmenu then
                inSubmenu = false
                if page == 2 then
                    Page2()
                end
                if page == 3 then
                    Page3()
                end
                if page == 4 then
                    Page4()
                end
            else
                if hasnotuploaded then
                    deleteItem()
                end
                Page1()
            end
        end
    end
    
    -- up

    if data == 265 then
        up()
    end

    -- down

    if data == 264 then
        down()
    end

    -- enter

    if data == 257 then
        enter()
    end

    -- backspace

    if data == 259 then
        holdingBackspace = true
        if inSubmenu then
            if submenuIndex == 3 then
                if string.len(cTitle) - 1 >= 0 then
                    cTitle = string.sub(cTitle, 1, string.len(cTitle) - 1)
                    Title()
                end
            end
            if submenuIndex == 4 then
                if string.len(cDescription) - 1 >= 0 then
                    cDescription = string.sub(cDescription, 1, string.len(cDescription) - 1)
                    Description()
                end
            end
        end
    end
end

function keyRelease(data)
    if data == 259 then
        holdingBackspace = false
    end
end

local offset = 0

function draw()

    if isUploading then
        local progress = getCurrentItemProgress()

        if progress >= 1 then
            isUploading = false
            Page1()
        end

        debugText(24, height - 24, "Uploading... please hold: " .. string.format("%.2f", progress * 100) .. "%", 24)
        return
    end

    if holdingBackspace then
        offset = offset + 1
    else
        offset = 0
    end
    if holdingBackspace and offset > 60 then
        if inSubmenu then
            if submenuIndex == 3 then
                if string.len(cTitle) - 1 >= 0 then
                    cTitle = string.sub(cTitle, 1, string.len(cTitle) - 1)
                    Title()
                end
            end
            if submenuIndex == 4 then
                if string.len(cDescription) - 1 >= 0 then
                    cDescription = string.sub(cDescription, 1, string.len(cDescription) - 1)
                    Description()
                end
            end
        end
    end
end