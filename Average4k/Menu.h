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

	bool created = false;

	virtual void onPacket(PacketType pt, char* data, int32_t length) {};
	virtual void onSteam(std::string s) {};

	void addCamera(AvgCamera* _cam)
	{
		cam = _cam;
		children.push_back(_cam);
	}

	virtual void create()
	{
		created = true;
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
	}

	void removeAll()
	{
		for(int i = 0; i < cam->children.size(); i++)
		{
			Object* obj = cam->children[i];
			if (obj)
				if (!obj->isDead)
					delete obj;
		}
		std::cout << "removed all " << children.size() << std::endl;
		cam->children.clear();
	}
	virtual void imguiUpdate(float elapsed) {};
	virtual void update(Events::updateEvent ev) = 0;
	virtual void postUpdate(Events::updateEvent ev) {};
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void keyUp(SDL_KeyboardEvent ev) {};
	virtual void mouseWheel(float wheel) {};
	virtual void leftMouseDown() {};
	virtual void leftMouseUp() {};
};