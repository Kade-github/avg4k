t = texture.new("Menu/darkmodebg")
loadTexture(t)
s = sprite.new(0,0,t)
s.transform.w = 1920
s.transform.h = 1080
function create()
    add(s)
end