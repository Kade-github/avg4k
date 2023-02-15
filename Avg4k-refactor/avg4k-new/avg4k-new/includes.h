#pragma once
#define STATIC_LINK

#ifdef STATIC_LINK
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define ZIP_STATIC
#define CURL_STATICLIB
#endif

#include <iostream>
#include <algorithm>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
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
#include <regex>
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

#include "Logging.h"
#include "BASS.h"
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include <io.h>

#include "Collision.h"