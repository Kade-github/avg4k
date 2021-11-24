#pragma once
#include "Menu.h"
class MultiplayerLobby :
    public Menu
{
    public:
        MultiplayerLobby();
        void update(Events::updateEvent event) override;
};

