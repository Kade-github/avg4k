#pragma once
#include "includes.h"
#include "Packet.h"
#include "Object.h"
#include "AvgCamera.h"

class Menu
{
public:
	Menu() {};
	virtual ~Menu()
	{
	}
	AvgCamera* cam;

	std::vector<Object*> children;

	virtual void onPacket(PacketType pt, char* data, int32_t length) {};
	virtual void onSteam(std::string s) {};

	void addCamera(AvgCamera* _cam)
	{
		cam = _cam;
		children.push_back(_cam);
	}

	virtual void create()
	{

	}

	void add(Object* obj) 
	{
		if (obj == NULL)
			return;
		cam->children.push_back(obj);
	}

	void removeObj(Object* obj)
	{
		if (obj == NULL)
			return;
		cam->children.erase(std::remove(cam->children.begin(), cam->children.end(), obj), cam->children.end());
		delete obj;
	}

	void removeAll()
	{
		for (Object* obj : cam->children)
		{
			delete obj;
		}
		std::cout << "removed all " << children.size() << std::endl;
		cam->children.clear();
	}

	virtual void update(Events::updateEvent ev) = 0;
	virtual void postUpdate(Events::updateEvent ev) {};
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void keyUp(SDL_KeyboardEvent ev) {};
};