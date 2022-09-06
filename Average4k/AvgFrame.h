#pragma once
#include "includes.h"
#include "Rendering.h"
#include "pugixml.hpp"

struct AvgFrame {
	Rect frameRect;
	Rect srcRect;
	int numba = 0;
	std::string name;

};

struct AvgAnimation {
	std::vector<AvgFrame> frames;
};


class AvgSparrow
{
public:
	std::map<std::string, AvgAnimation> animations;

	std::vector<int> framesToPlay;

	std::string currentAnim;

	void left_fill_zeros(char* dest, const char* str, int length)
	{
		sprintf(dest, "%.*d%s", (int)(length - strlen(str)), 0, str);
	}

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	int realFrame;
	int lastFrame;

	void frameswap(AvgFrame* A, AvgFrame* B) {
		AvgFrame temp = *A;
		*A = *B;
		*B = temp;
	}

	AvgFrame getRectFromFrame(std::string name, int frame)
	{
		char buffer[4];
		left_fill_zeros(buffer, std::to_string(frame).c_str(), 4);
		AvgFrame src;

		if (framesToPlay.size() != 0)
		{
			std::vector<AvgFrame> fra = animations[name].frames;
			if (realFrame > framesToPlay.size() - 1)
				realFrame = 0;
			int frr = framesToPlay[realFrame];
			frr--;
			if (frr < 0)
				frr++;
			AvgFrame fr = fra[frr];
			if (lastFrame != frame)
			{
				lastFrame = frame;
				realFrame++;
			}
			return fr;
		}
		else
		{
			for (AvgFrame frame : animations[name].frames)
			{

				if (frame.name == buffer)
				{
					src = frame;
					break;
				}
			}
		}
		return src;
	}

	AvgFrame getRectFromFrame(int frame)
	{
		char buffer[4];
		left_fill_zeros(buffer, std::to_string(frame).c_str(), 4);
		AvgFrame src;

		if (framesToPlay.size() != 0)
		{
			std::vector<AvgFrame> fra = animations[currentAnim].frames;
			if (realFrame > framesToPlay.size() - 1)
				realFrame = 0;
			int frr = framesToPlay[realFrame];
			frr--;
			if (frr < 0)
				frr++;
			AvgFrame fr = fra[frr];
			if (lastFrame != frame)
			{
				lastFrame = frame;
				realFrame++;
			}
			return fr;
		}
		else
		{
			for (AvgFrame frame : animations[currentAnim].frames)
			{

				if (frame.name == buffer)
				{
					src = frame;
					break;
				}
			}
		}
		return src;
	}

	void playAnim(std::string anim)
	{
		currentAnim = anim;
	}

	~AvgSparrow()
	{
		std::map<std::string, AvgAnimation>::iterator it;
		for (it = animations.begin(); it != animations.end(); it++)
			it->second.frames.clear();
		animations.clear();
		framesToPlay.clear();
	}

	AvgSparrow(std::string xmlPath, float imgW, float imgH)
	{
		pugi::xml_document doc;
		doc.load_file(xmlPath.c_str());

		std::string currentName = "";
		AvgAnimation curAnim;
		int framei = 0;

		for (pugi::xml_node node : doc.child("TextureAtlas").children())
		{
			std::string name = node.attribute("name").as_string();

			std::string firstPart = name.substr(0, name.size() - 4);

			if (firstPart != currentName)
			{
				if (animations.find(firstPart) != animations.end())
				{
					animations[currentName] = curAnim;
					currentName = firstPart;
					curAnim = animations[currentName];
				}
				else
				{
					if (currentName != "")
						animations[currentName] = curAnim;
					curAnim.frames = {};
					currentName = firstPart;
				}
			}

			AvgFrame frame;

			std::string numbas = name.substr(name.size() - 4, 4);

			frame.name = numbas;
			numbas.erase(0, numbas.find_first_not_of('0'));
			if (numbas.size() == 0)
				numbas = "0";
			frame.numba = std::stoi(numbas);

			// specifically, dstRect isn't the destination rect used in game
			// it's actually just kind of a intermediate variable.
			// it's not really used other than keeping the original x and y + w and h

			Rect selectRect;
			selectRect.x = node.attribute("x").as_int();
			selectRect.y = node.attribute("y").as_int();
			selectRect.w = node.attribute("width").as_int();
			if (selectRect.w == 0)
				selectRect.w = node.attribute("frameWidth").as_int();
			selectRect.h = node.attribute("height").as_int();
			if (selectRect.h == 0)
				selectRect.h = node.attribute("frameHeight").as_int();

			// this exists because opengl uses normalized coordinates
			// ie, 0->1
			// 0 is left/top most
			// 1 is right/bottom most
			// 0.5 is the middle 
			Rect rect;
			rect.x = selectRect.x / imgW;
			rect.y = selectRect.y / imgH;


			rect.w = selectRect.w / imgW;
			rect.h = selectRect.h / imgH;

			frame.srcRect = rect;

			Rect r;
			r.x = -node.attribute("frameX").as_int();
			r.y = -node.attribute("frameY").as_int();
			r.w = node.attribute("width").as_int();
			r.h = node.attribute("height").as_int();

			if (r.w > node.attribute("frameWidth").as_int() && node.attribute("frameWidth").as_int() != 0)
				r.w = node.attribute("frameWidth").as_int();
			if (r.h > node.attribute("frameHeight").as_int() && node.attribute("frameHeight").as_int() != 0)
				r.h = node.attribute("frameHeight").as_int();
			frame.frameRect = r;

			curAnim.frames.push_back(frame);
		}
		
		if (currentName != "")
			animations[currentName] = curAnim;

		// order the frames and reformat

		bool minus = false;

		char buffer[4];


		/*for (auto& data : animations)
			{
				for (int ii = 0; ii < data.second.frames.size(); ii++) {
					for (int j = ii; j < (data.second.frames.size()); j++) {
						if (data.second.frames[ii].numba > data.second.frames[j].numba) {
							frameswap(&data.second.frames[ii], &data.second.frames[j]);
						}
					}
				}

				if (data.second.frames[0].name == "0001")
					minus = true;

				for (int ii = 0; ii < data.second.frames.size(); ii++)
				{
					if (minus)
					{
						data.second.frames[ii].numba--;
						for (int b = 0; b < 4; b++)
							buffer[b] = 0;
						left_fill_zeros(buffer, std::to_string(data.second.frames[ii].numba).c_str(), 4);
						data.second.frames[ii].name = std::string(buffer);
					}
				}
			}*/
		
	}
};
