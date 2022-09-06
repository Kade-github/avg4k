#include "ChatObject.h"
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

AvgRect* chatBody;
AvgRect* chatSend;
AvgRect* chatNotif;
AvgGroup* texts;
std::vector<message> messages;
Text* sendText;
Text* rank;

Text* notifText;
Text* notifRank;

bool shouldNotif = false;

std::string fuckin;

ChatObject::ChatObject(float _x, float _y)
{
	MUTATE_START
	x = _x;
	y = _y;

	h = 288;
	w = 1280;

	chatBody = new AvgRect(0, Game::gameHeight + h, w, h);
	chatBody->alpha = 0.6;
	add(chatBody);

	chatSend = new AvgRect(0, 0, w, 32);
	chatSend->alpha = 0.4;
	add(chatSend);

	sendText = new Text(0, 0, Multiplayer::username + ": ", 16, "Futura Bold");
	add(sendText);
	sendText->alpha = 0.85;
	sendText->color = Multiplayer::nameColor;

	chatNotif = new AvgRect(0, Game::gameHeight - 32, w, 32);
	chatNotif->alpha = 0;

	notifRank = new Text(4, 0, Multiplayer::tag, 16, "Futura Bold");
	notifRank->alpha = 0;

	notifText = new Text(0, 0, "bn", 16, "Futura Bold");
	notifText->alpha = 0;

	rank = new Text(4, 0, Multiplayer::tag, 16, "Futura Bold");

	if (Multiplayer::tag == "NO_TAG")
		rank->alpha = 0;

	rank->color = Multiplayer::tagColor;
	add(rank);



	texts = new AvgGroup(0, 0, w, chatBody->h);
	add(texts);
	add(chatNotif);
	add(notifText);
	add(notifRank);
	MUTATE_END
}

ChatObject::~ChatObject()
{
	clearMessages();
}

void ChatObject::open()
{
	MUTATE_START
	Tweening::TweenManager::removeTween("chat_notif");
	chatNotif->alpha = 0;

	notifRank->alpha = 0;
	notifText->alpha = 0;
	wait = 0;
	startTween = false;
	shouldNotif = false;
	opened = !opened;
	Tweening::TweenManager::createNewTween("chat_openBody", chatBody, Tweening::tt_Y, 500, Game::gameHeight + h, Game::gameHeight - h, NULL, Easing::EaseInSine, false);
	MUTATE_END
}

void ChatObject::close()
{
	MUTATE_START
	shouldNotif = false;
	opened = !opened;
	Tweening::TweenManager::createNewTween("chat_closeBody", chatBody, Tweening::tt_Y, 500, Game::gameHeight - h, Game::gameHeight + h, NULL, Easing::EaseInSine, false);
	MUTATE_END
}

void ChatObject::clearMessages()
{
	Tweening::TweenManager::removeTween("chat_openBody");
	Tweening::TweenManager::removeTween("chat_closeBody");
	Tweening::TweenManager::removeTween("chat_notif");
	for (message& mess : messages)
	{
		texts->removeObj(mess.tagT);
		texts->removeObj(mess.text);
	}
	messages.clear();
}

void ChatObject::addMessage(SPacketOnChat packetChat)
{
	MUTATE_START
	Text* tag;
	Text* text;

	message msg;

	msg.name = packetChat.sender.Name + ": " + packetChat.message;
	msg.tag = packetChat.tagText;

	text = new Text(0, 0, msg.name, 16, "Futura Bold");
	
	tag = new Text(0, 0, msg.tag, 16, "Futura Bold");


	text->border = true;
	text->borderSize = 2;
	tag->border = true;
	tag->borderSize = 2;

	text->borderColor = { 255,255,255 };
	tag->borderColor = { 255,255,255 };

	tag->color = {(int)(packetChat.tagColor.red * 255),(int)(packetChat.tagColor.green * 255),(int)(packetChat.tagColor.blue * 255) };
	text->color = { (int)(packetChat.color.red * 255),(int)(packetChat.color.green * 255),(int)(packetChat.color.blue * 255) };
	msg.text = text;
	msg.tagT = tag;

	msg.ind = 0;

	for (message& m : messages)
		m.ind += 1;
	if (packetChat.tagText != "NO_TAG")
		texts->add(tag);
	texts->add(text);

	messages.push_back(msg);
	
	if (!opened && showNotifs)
	{
		chatNotif->alpha = 0.8;

		notifRank->alpha = 1;
		notifText->alpha = 1;

		shouldNotif = true;

		notifRank->borderColor = { 255,255,255 };
		notifText->borderColor = { 255,255,255 };

		notifRank->color = { (int)(packetChat.tagColor.red * 255),(int)(packetChat.tagColor.green * 255),(int)(packetChat.tagColor.blue * 255) };
		notifText->color = { (int)(packetChat.color.red * 255),(int)(packetChat.color.green * 255),(int)(packetChat.color.blue * 255) };
		startTween = false;
		wait = 0;
		if (msg.tag == "NO_TAG")
			notifRank->alpha = 0;
		notifRank->setText(msg.tag);
		notifText->setText(msg.name);
	}
	MUTATE_END
}

void ChatObject::onPacket(PacketType pt, char* data, int32_t length)
{
	if (pt != eSPacketOnChat)
		return;
	VM_START
	SPacketOnChat chat;
	msgpack::unpacked result;

	msgpack::object obj;
	msgpack::unpack(result, data, length);

	obj = msgpack::object(result.get());

	obj.convert(chat);

	addMessage(chat);
	VM_END
}

void ChatObject::draw()
{
	MUTATE_START
	chatSend->h = sendText->surfH + 8;
	chatSend->y = (chatBody->y + chatBody->h) - chatSend->h;

	// text stuff!!!

	rank->y = ((chatBody->y + chatBody->h) - sendText->h) - 4;

	sendText->y = rank->y + 1;

	if (rank->text != "NO_TAG")
		sendText->x = rank->surfW + 8;
	else
		sendText->x = 0;


	texts->y = chatBody->y;

		notifRank->y = chatNotif->y + (notifRank->surfH / 2);
		notifText->y = notifRank->y;

		if (notifRank->text != "NO_TAG")
			notifText->x = notifRank->surfW + 8;
		else
			notifText->x = 0;

		if (startTween)
		{
			if (notifRank->text != "NO_TAG")
				notifRank->alpha = chatNotif->alpha;
			notifText->alpha = chatNotif->alpha;
		}

		if (wait >= 1000)
		{
			if (!startTween && shouldNotif)
			{
				startTween = true;
				Tweening::TweenManager::removeTween("chat_notif");
				Tweening::TweenManager::createNewTween("chat_notif", chatNotif, Tweening::tt_Alpha, 3500, 1, 0, NULL, Easing::EaseInSine, false);
			}
		}
		else if (wait < 1000 && !startTween)
			wait += Game::deltaTime;


	for (message& msg : messages)
	{
		msg.tagT->y = ((((texts->h + texts->y) - 16)) - (16 * msg.ind)) - (chatSend->h + 4);
		msg.text->y = msg.tagT->y + 1;
		if (msg.tagT->text != "NO_TAG")
			msg.text->x = msg.tagT->surfW + 8;
		else
			msg.text->x = 0;
	}
	MUTATE_END
}

void ChatObject::keyDown(SDL_KeyboardEvent ev)
{
	if (!opened)
		return;
	MUTATE_START
	switch (ev.keysym.sym)
	{
	case SDLK_BACKSPACE:
		if (fuckin.size() > 0 && typing)
		{
			fuckin.pop_back();
			sendText->setText(Multiplayer::username + ": " + fuckin + "_");
		}
		break;
	case SDLK_RETURN:
		if (typing)
		{
			if (fuckin.size() != 0)
			{
				CPacketSendMessage msg;
				msg.message = fuckin;
				msg.Order = 0;
				msg.PacketType = eCPacketSendMessage;

				fuckin = "";

				Multiplayer::sendMessage<CPacketSendMessage>(msg);
			}
			sendText->setText(Multiplayer::username + ": " + fuckin);
		}
		else
			sendText->setText(Multiplayer::username + ": " + fuckin + "_");
		typing = !typing;
		break;
	}
	MUTATE_END
}

void ChatObject::textInput(SDL_TextInputEvent ev)
{
	if (typing)
	{
		MUTATE_START
		if (fuckin.size() + 1 > 128)
			return;
		fuckin += ev.text;
		sendText->setText(Multiplayer::username + ": " + fuckin + "_");
		MUTATE_END
	}
}
