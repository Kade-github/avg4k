#pragma once
#include "Menu.h"
#include "Lobby.h"
#include "Text.h"
#include "Game.h"
#include "MainMenu.h"
#include "MultiplayerLobbies.h"
#include "Gameplay.h"
#include "ChatObject.h"

struct person {
    Text* display;
    AvgSprite* avatar;
};


class MultiplayerLobby :
    public Menu
{
    public:
        static bool inLobby;
        bool waitingForStart;
        static lobby CurrentLobby;
        static bool isHost;

        bool downloading = true;

        int diff = 0;

        ChatObject* chat;

        Text* helpDisplay;

        Text* warningDisplay;

        std::vector<person> people;

        virtual void create();
        void refreshLobby(lobby l);
        void onPacket(PacketType pt, char* data, int32_t length);
        void onSteam(std::string s) override;
        MultiplayerLobby(lobby l, bool hosted, bool backFromSelect);
        void keyDown(SDL_KeyboardEvent event) override;
        void update(Events::updateEvent event) override;
};

