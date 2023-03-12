#include "LuaText.h"
#include "Average4K.h"
void Average4k::Lua::Base::textObject::setFont(std::string _font)
{
	font = _font;
	if (base)
	{
		AvgEngine::Base::Text* te = static_cast<AvgEngine::Base::Text*>(base);
		te->SetFont(Average4K::skin->GetFontPath(), _font);
		transform.w = base->transform.w;
		transform.h = base->transform.h;
	}
}
