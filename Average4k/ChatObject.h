#pragma once
#include "Object.h"
#include "includes.h"
#include "SPacketOnChat.h"
#include "CPacketSendMessage.h"
#include "GL.h"
#include "AvgRect.h"
#include "Text.h"
#include "TweenManager.h"

struct message {
	std::string name;
	std::string tag;

	Text* tagT;
	Text* text;

	int ind;
};

class ChatObject : public Object
{
public:
	bool opened;
	bool typing;

	AvgRect* chatBody;
	AvgRect* chatSend;
	AvgRect* chatNotif;
	std::vector<message> messages;
	Text* sendText;
	Text* rank;

	Text* notifText;
	Text* notifRank;

	bool shouldNotif = false;

	std::string fuckin;

	int w;
	int h;

	int wait = 0;
	bool showNotifs = false;
	bool startTween = false;

	ChatObject(float _x, float _y);
	~ChatObject();

	void open();

	void close();

	void clearMessages();

	void addMessage(SPacketOnChat packetChat);
	void onPacket(PacketType pt, char* data, int32_t length) override;
	void draw();
	void keyDown(SDL_KeyboardEvent ev) override;
	void textInput(SDL_TextInputEvent ev) override;
};

