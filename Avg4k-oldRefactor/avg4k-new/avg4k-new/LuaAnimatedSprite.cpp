#include "LuaAnimatedSprite.h"
#include "SpriteAnimated.h"

void Average4k::Lua::Base::animatedSprite::setCenter(bool center)
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		s->center = center;
	}
}

bool Average4k::Lua::Base::animatedSprite::getCenter()
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		return s->center;
	}
	return false;
}

void Average4k::Lua::Base::animatedSprite::setFrameSize(int _w, int _h)
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		s->SetFrameSize(_w, _h);
	}
}

void Average4k::Lua::Base::animatedSprite::setFPS(int _fps)
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		s->fps = _fps;
	}
}

int Average4k::Lua::Base::animatedSprite::getFPS()
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		return s->fps;
	}
	return 0;
}

void Average4k::Lua::Base::animatedSprite::setFrame(int _frame)
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		s->frame = _frame;
	}
}

int Average4k::Lua::Base::animatedSprite::getFrame()
{
	if (base)
	{
		Average4k::External::Spritesheet::AnimatedSprite* s = static_cast<Average4k::External::Spritesheet::AnimatedSprite*>(base);
		return s->frame;
	}
	return 0;
}
