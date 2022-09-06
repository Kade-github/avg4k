#pragma once
#include "Object.h"
#include "includes.h"
#include "SPacketOnChat.h"
#include "CPacketSendMessage.h"
#include "GL.h"

class ChatObject : public Object
{
public:
	bool opened;
	bool typing;

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

