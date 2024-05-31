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

		/// <summary>
		/// Set texture column of the note
		/// </summary>
		/// <param name="col">The column</param>
		void SetCol(int col)
		{
			if (_base == nullptr) return;
			_base->sheet_col = col;
		}

		/// <summary>
		/// Set texture row of the note
		/// </summary>
		/// <param name="row">The row</param>
		void SetRow(int row)
		{
			if (_base == nullptr) return;
			_base->sheet_row = row;
		}

		/// <summary>
		/// Get texture column of the note
		/// </summary>
		/// <returns>The column</returns>
		int GetCol()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_col;
		}

		/// <summary>
		/// Get texture row of the note
		/// </summary>
		/// <returns>The row</returns>
		int GetRow()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_row;
		}

		/// <summary>
		/// Set the end texture row of the note
		/// </summary>
		/// <param name="row">The row</param>
		void SetEndRow(int row)
		{
			if (_base == nullptr) return;
			_base->sheet_end_row = row;
		}

		/// <summary>
		/// Set the end texture column of the note
		/// </summary>
		/// <param name="col">The column</param>
		void SetEndCol(int col)
		{
			if (_base == nullptr) return;
			_base->sheet_end_col = col;
		}

		/// <summary>
		/// Get the end texture row of the note
		/// </summary>
		/// <returns>The row</returns>
		int GetEndRow()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_end_row;
		}

		/// <summary>
		/// Get the end texture column of the note
		/// </summary>
		/// <returns>The column</returns>
		int GetEndCol()
		{
			if (_base == nullptr) return 0;
			return _base->sheet_end_col;
		}

		/// <summary>
		/// Set the angle of the note
		/// </summary>
		/// <param name="angle">The angle</param>
		void setAngle(float angle)
		{
			if (_base == NULL)
				return;
			_base->transform.angle = angle;
		}

		/// <summary>
		/// Get the angle of the note
		/// </summary>
		/// <returns>The angle</returns>
		float getAngle()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.angle;
		}

		/// <summary>
		/// Set the red color of the note
		/// </summary>
		/// <param name="r">The red color</param>
		void setR(float r)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
		}

		/// <summary>
		/// Set the green color of the note
		/// </summary>
		/// <param name="g">The green color</param>
		void setG(float g)
		{
			if (_base == NULL)
				return;
			_base->transform.g = g;
		}

		/// <summary>
		/// Set the blue color of the note
		/// </summary>
		/// <param name="b">The blue color</param>
		void setB(float b)
		{
			if (_base == NULL)
				return;
			_base->transform.b = b;
		}

		/// <summary>
		/// Set the alpha of the note
		/// </summary>
		/// <param name="a">The alpha</param>
		void setA(float a)
		{
			if (_base == NULL)
				return;
			_base->transform.a = a;
		}

		/// <summary>
		/// Get the red color of the note
		/// </summary>
		/// <returns>The red color</returns>
		float getR()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.r;
		}


		/// <summary>
		/// Get the green color of the note
		/// </summary>
		/// <returns>The green color</returns>
		float getG()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.g;
		}

		/// <summary>
		/// Get the blue color of the note
		/// </summary>
		/// <returns>The blue color</returns>
		float getB()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.b;
		}

		/// <summary>
		/// Get the alpha of the note
		/// </summary>
		/// <returns>The alpha</returns>
		float getA()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.a;
		}

		/// <summary>
		/// Get the scale of the note
		/// </summary>
		/// <returns>The scale</returns>
		float getScale()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.scale;
		}

		/// <summary>
		/// Set the scale of the note
		/// </summary>
		/// <param name="scale">The scale</param>
		void setScale(float scale)
		{
			if (_base == NULL)
				return;
			_base->transform.scale = scale;
		}

		/// <summary>
		/// Set the z index of the note
		/// </summary>
		/// <param name="index">The index</param>
		void setZIndex(int index)
		{
			if (_base == NULL)
				return;
			_base->zIndex = index;
		}

		/// <summary>
		/// Get the z index of the note
		/// </summary>
		/// <returns>The index</returns>
		int getZIndex()
		{
			if (_base == NULL)
				return 0;
			return _base->zIndex;
		}

		/// <summary>
		/// Get the tag of the note
		/// </summary>
		/// <returns>The tag</returns>
		std::string getTag()
		{
			if (_base == NULL)
				return "";
			return _base->tag;
		}

		/// <summary>
		/// Set the tag of the note
		/// </summary>
		/// <param name="tag">The tag</param>
		void setTag(const std::string& tag)
		{
			if (_base == NULL)
				return;
			_base->tag = tag;
		}

		/// <summary>
		/// Set the source rectangle of the note
		/// </summary>
		/// <param name="x">The x position</param>
		/// <param name="y">The y position</param>
		/// <param name="w">The width</param>
		/// <param name="h">The height</param>
		void setSrcRec(float x, float y, float w, float h)
		{
			if (_base == NULL)
				return;
			_base->src.x = x;
			_base->src.y = y;
			_base->src.w = w;
			_base->src.h = h;
		}

		/// <summary>
		/// Add an overlay to the note
		/// </summary>
		/// <param name="col">The column</param>
		/// <param name="row">The row</param>
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