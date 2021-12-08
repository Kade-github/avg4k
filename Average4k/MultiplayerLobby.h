#pragma once
#include "Menu.h"
#include "Lobby.h"
#include "Text.h"
#include "Game.h"
#include "MainMenu.h"
#include "MultiplayerLobbies.h"
#include "Gameplay.h"

struct person {
    Text* display;
    SDL_Texture* avatar;
};

class MultiplayerLobby :
    public Menu
{
    public:
        static bool inLobby;
        bool waitingForStart;
        static lobby CurrentLobby;
        static bool isHost;

        Text* helpDisplay;

        Text* warningDisplay;

        std::vector<person> people;

        void refreshLobby(lobby l);
        void onPacket(PacketType pt, char* data, int32_t length);

        MultiplayerLobby(lobby l, bool hosted, bool backFromSelect);
        void keyDown(SDL_KeyboardEvent event) override;
        void update(Events::updateEvent event) override;
        void postUpdate(Events::updateEvent event) override;
};

