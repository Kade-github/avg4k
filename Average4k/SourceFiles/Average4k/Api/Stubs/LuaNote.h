/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef AVG4K_LUA_NOTE_H
#define AVG4K_LUA_NOTE_H

#include "LuaSprite.h"
#include "../../Objects/BaseNote.h"

namespace Average4k::Api::Stubs
{
	class LuaNote : public LuaObject
	{
	public:
		Average4k::Objects::BaseNote* _base;

		LuaNote()
		{
			_base = nullptr;
		}

		LuaNote(Average4k::Objects::BaseNote* note)
		{
			_base = note;
			_base->texture->dontDelete = false;
			_baseObject = _base;
		}

		void SetCol(int col)
		{
			if (_base == nullptr) return;
			_base->sheet_col = col;
		}

		void SetRow(int row)
		{
			if (_base == nullptr) return;
			_base->sheet_row = row;
		}

		int GetCol()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_col;
		}

		int GetRow()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_row;
		}

		void SetEndRow(int row)
		{
			if (_base == nullptr) return;
			_base->sheet_end_row = row;
		}

		void SetEndCol(int col)
		{
			if (_base == nullptr) return;
			_base->sheet_end_col = col;
		}

		int GetEndRow()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_end_row;
		}

		int GetEndCol()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_end_col;
		}

		void setAngle(float angle)
		{
			if (_base == NULL)
				return;
			_base->transform.angle = angle;
		}

		float getAngle()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.angle;
		}

		void setR(float r)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
		}

		void setG(float g)
		{
			if (_base == NULL)
				return;
			_base->transform.g = g;
		}

		void setB(float b)
		{
			if (_base == NULL)
				return;
			_base->transform.b = b;
		}

		void setA(float a)
		{
			if (_base == NULL)
				return;
			_base->transform.a = a;
		}

		float getR()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.r;
		}

		float getG()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.g;
		}

		float getB()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.b;
		}

		float getA()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.a;
		}

		float getScale()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.scale;
		}

		void setScale(float scale)
		{
			if (_base == NULL)
				return;
			_base->transform.scale = scale;
		}

		void setZIndex(int index)
		{
			if (_base == NULL)
				return;
			_base->zIndex = index;
		}

		int getZIndex()
		{
			if (_base == NULL)
				return 0;
			return _base->zIndex;
		}

		std::string getTag()
		{
			if (_base == NULL)
				return "";
			return _base->tag;
		}

		void setTag(const std::string& tag)
		{
			if (_base == NULL)
				return;
			_base->tag = tag;
		}

		void setSrcRec(float x, float y, float w, float h)
		{
			if (_base == NULL)
				return;
			_base->src.x = x;
			_base->src.y = y;
			_base->src.w = w;
			_base->src.h = h;
		}

		void addOverlay(int col, int row)
		{
			if (_base == NULL)
				return;
			_base->overlays = { col, row };
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaNote>("Note",
				sol::no_constructor,
				"col", sol::property(&LuaNote::GetCol, &LuaNote::SetCol),
				"row", sol::property(&LuaNote::GetRow, &LuaNote::SetRow),
				"endCol", sol::property(&LuaNote::GetEndCol, &LuaNote::SetEndCol),
				"endRow", sol::property(&LuaNote::GetEndRow, &LuaNote::SetEndRow),
				"angle", sol::property(&LuaNote::getAngle, &LuaNote::setAngle),
				"r", sol::property(&LuaNote::getR, &LuaNote::setR),
				"g", sol::property(&LuaNote::getG, &LuaNote::setG),
				"b", sol::property(&LuaNote::getB, &LuaNote::setB),
				"alpha", sol::property(&LuaNote::getA, &LuaNote::setA),
				"scale", sol::property(&LuaNote::getScale, &LuaNote::setScale),
				"zIndex", sol::property(&LuaNote::getZIndex, &LuaNote::setZIndex),
				"setSrcRec", &LuaNote::setSrcRec,
				"addOverlay", &LuaNote::addOverlay,
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
	};
}

#endif