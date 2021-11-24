#pragma once
#include "Menu.h"
#include "Lobby.h"
#include "Text.h"
#include "Game.h"

struct person {
    Text* display;
    SDL_Texture* avatar;
};

class MultiplayerLobby :
    public Menu
{
    public:
        lobby CurrentLobby;

        Text* helpDisplay;

        std::vector<person*> people;

        void refreshLobby(lobby l);
        void onPacket(PacketType pt, char* data, int32_t length);

        MultiplayerLobby(lobby l);
        void keyDown(SDL_KeyboardEvent event) override;
        void update(Events::updateEvent event) override;
};

