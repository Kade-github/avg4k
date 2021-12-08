#pragma once
#include "Menu.h"
#include <list>
#include "Lobby.h"
#include "Text.h"
#include "Game.h"

struct bruh {
    player p;
    SDL_Texture* avatar;
};

class MultiplayerLobbies :
    public Menu
{
public:
    MultiplayerLobbies();
    std::vector<Text*> lobbyTexts;
    std::vector<lobby> Lobbies;

    std::vector<bruh> avatars;



    Text* helpText;

    float refreshTimer = 3000;

    bool joiningServer = false;

    int selectedIndex;
    void refreshLobbies();
    void updateList(std::vector<lobby> lobs);
    void onPacket(PacketType pt, char* data, int32_t length);

    void update(Events::updateEvent event) override;
    void keyDown(SDL_KeyboardEvent event) override;
    void postUpdate(Events::updateEvent event) override;
};

