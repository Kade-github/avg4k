#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "steam/steam_api.h"
#include <Themida/ThemidaSDK.h>
#include <msgpack.hpp>
#include <openssl/opensslconf.h>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include "Packet.h"
#include "SPacketUpdateLobbyData.h"
#include "SPacketWtfAmInReply.h"
#include "CPacketWtfAmIn.h"
#include "CPacketLeave.h"
#include "SPacketStartLobbyGame.h"
#include "SPacketUpdateLobbyChart.h"
#include "CPacketClientMissingChart.h"
#include "CPacketHostChangeChart.h"
