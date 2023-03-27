#include "LuaSprite.h"
#include "Sprite.h"

void Average4k::Lua::Base::sprite::setCenter(bool center)
{
	if (base)
	{
		AvgEngine::Base::Sprite* s = static_cast<AvgEngine::Base::Sprite*>(base);
		s->center = center;
	}
}

bool Average4k::Lua::Base::sprite::getCenter()
{
	if (base)
	{
		AvgEngine::Base::Sprite* s = static_cast<AvgEngine::Base::Sprite*>(base);
		return s->center;
	}
	return false;
}