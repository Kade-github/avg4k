#pragma once
#define STATIC_LINK

#ifdef STATIC_LINK
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define ZIP_STATIC
#define CURL_STATICLIB
#endif

#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include "FONT/SDL_ttf.h"
#include <vector>
#include <string>
#include <filesystem>
#include <limits.h>

#ifdef STATIC_LINK
#include "include/static/zip.h"
#else
#include <zip.h>
#endif
#include <fstream>
#include <stdio.h>
#include "steam/steam_api.h"
#include <Themida/ThemidaSDK.h>
#include <msgpack.hpp>
#include <openssl/opensslconf.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <stdlib.h>
#include <SDL_image.h>
#include <regex>
#include <SDL_sound.h>
#include <bass.h>


#ifdef STATIC_LINK
#include <curl_static/curl.h>
#else
#include <curl/curl.h>
#endif

#ifdef STATIC_LINK
#include "include/boost_static/algorithm/clamp.hpp"
#else
#include <boost/algorithm/clamp.hpp>
#endif

#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>

#include "Packet.h"
#include "SPacketUpdateLeaderboard.h"
#include "SPacketFinalizeChart.h"
#include "CPacketSongFinished.h"
#include "CPacketNoteHit.h"
#include "SPacketUpdateLobbyData.h"
#include "SPacketWtfAmInReply.h"
#include "CPacketWtfAmIn.h"
#include "CPacketLeave.h"
#include "SPacketStartLobbyGame.h"
#include "SPacketUpdateLobbyChart.h"
#include "CPacketChartAcquired.h"
#include "CPacketHostChangeChart.h"
#include "CPacketHostStartGame.h"
#include <io.h>