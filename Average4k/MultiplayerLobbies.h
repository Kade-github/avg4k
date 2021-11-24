#pragma once
#include "Menu.h"
#include <list>
#include "Lobby.h"
#include "Text.h"
#include "Game.h"
class MultiplayerLobbies :
    public Menu
{
public:
    MultiplayerLobbies();
    std::vector<SDL_Texture*> peopleTextures;
    std::vector<Text*> lobbyTexts;
    std::vector<lobby> Lobbies;

    bool joiningServer = false;

    int selectedIndex;

    void updateList(std::vector<lobby> lobs);

    void onPacket(PacketType pt, char* data, int32_t length);

    void update(Events::updateEvent event) override;
    void keyDown(SDL_KeyboardEvent event) override;
};

