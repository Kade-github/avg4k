#pragma once
#include "includes.h"
#include "Packet.h"
#include "Object.h"
class Menu
{
public:
	Menu() {};

	std::vector<Object*> children;

	virtual void onPacket(PacketType pt, char* data, int32_t length) {};
	virtual void onSteam(std::string s) {};

	void add(Object* obj) 
	{
		children.push_back(obj);
	}

	void removeObj(Object* obj)
	{
		children.erase(std::remove(children.begin(), children.end(), obj), children.end());
		obj->beforeDeath();
		obj->die();
	}

	void removeAll()
	{
		for (Object* obj : children)
		{
			obj->beforeDeath();
			obj->die();
		}
		children.clear();
	}

	virtual void update(Events::updateEvent ev) = 0;
	virtual void postUpdate(Events::updateEvent ev) {};
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void keyUp(SDL_KeyboardEvent ev) {};
};