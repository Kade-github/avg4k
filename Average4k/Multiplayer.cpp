#include "Multiplayer.h"
#include "Game.h"
#include "Gameplay.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

bool Multiplayer::connectedToServer = false;

bool Multiplayer::loggedIn = false;

client c;
websocketpp::connection_hdl connectionHdl;

std::string username;

void Multiplayer::SendPacket(std::string data, PacketType packet) {

    websocketpp::lib::error_code ec;

    std::string dataStr = data;

    char* sendData = (char*)malloc(dataStr.length() + 8);

    char* writeTo = (char*)(((__int64)sendData) + 8);

    __int32* dataPtrs = (__int32*)sendData;
    dataPtrs[0] = packet;
    dataPtrs[1] = (__int32)dataStr.length();

    memcpy(writeTo, dataStr.c_str(), dataStr.length());

    c.send(connectionHdl, std::string(sendData, dataStr.length() + 8), websocketpp::frame::opcode::BINARY, ec);

    free(sendData);
}

DWORD WINAPI NewThread(LPVOID param) {
    for (;;) {
        VM_START
        Sleep(1000);

        CPacketStatus heartbeat;
        heartbeat.PacketType = eCPacketStatus;
        heartbeat.Order = 0;
        heartbeat.code = 200;

        if (Game::patched)
            heartbeat.code = 7393;

        heartbeat.Status = "hello";

        Multiplayer::sendMessage<CPacketStatus>(heartbeat);
        VM_END
    }
}

DWORD WINAPI pleaseLogin(LPVOID agh)
{
    Game::multi->login();

    return 0;
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {

    if (msg->get_opcode() != websocketpp::frame::opcode::BINARY)
        return;

    std::string string = msg->get_raw_payload();

    char* ptr = (char*)malloc(string.length());

    memcpy(ptr, string.c_str(), string.length());

    int32_t* ints = (int32_t*)ptr;

    int32_t packetType = ints[0];
    int32_t length = ints[1];

    size_t size = (size_t)length;

    char* data = (char*)((__int64)(ptr)+8);

    PacketType type = (PacketType)packetType;

    SPacketStatus status;
    SPacketHello helloBack;
    Events::packetEvent p;

    msgpack::unpacked result;

    msgpack::object obj;

    switch (type)
    {
        case eSPacketStatus:
            unpack(result, data, length);

            obj = msgpack::object(result.get());

            obj.convert(status);

            switch (status.code)
            {
                case 403:
                    if (!Multiplayer::loggedIn)
                    {
                        std::cout << "trying to login cuz unauthorized" << std::endl;
                        CreateThread(NULL, NULL, pleaseLogin, NULL, NULL, NULL);
                    }
                    break;
                case 409:
                    std::cout << "conflict" << std::endl;
                    break;
                case 404:
                    std::cout << "not found" << std::endl;
                    break;
            }
            p.data = data;
            p.length = length;
            p.type = type;
            p.ogPtr = ptr;
            Game::instance->weGotPacket(p);
        break;
        case eSPacketHello:
            unpack(result, data, length);

            obj = msgpack::object(result.get());

            obj.convert(helloBack);


            CreateThread(NULL, NULL, NewThread, NULL, NULL, NULL);
            Multiplayer::loggedIn = true;

            std::cout << helloBack.Message << ". hello server, fuck you too! " << std::endl;
        break;
        default:
            p.data = data;
            p.length = length;
            p.type = type;
            p.ogPtr = ptr;
            Game::instance->weGotPacket(p);
            break;
    }
}



bool verify_certificate(const char* hostname, bool preverified, boost::asio::ssl::verify_context& ctx) {
    return true;
}


context_ptr on_tls_init(const char* hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::single_dh_use);


        ctx->set_verify_mode(boost::asio::ssl::verify_peer);
        ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}

bool firstConnection = false;

DWORD WINAPI Multiplayer::connect(LPVOID agh)
{
    connectedToServer = true;

   
    std::string url = "wss://titnoas.xyz/ballsandsex/";
   
    std::cout << "Creating things" << std::endl;

    try {

        while (true)
        {
           
            if (!firstConnection)
            {
                c.set_access_channels(websocketpp::log::alevel::none);
                c.clear_access_channels(websocketpp::log::alevel::all);
                c.set_error_channels(websocketpp::log::elevel::none);

                // Initialize ASIO
                c.init_asio();

                // Register our message handler
                std::cout << "Setting handlers" << std::endl;
                c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

                c.set_tls_init_handler(bind(&on_tls_init, "titnoas.xyz", ::_1));

                std::cout << "handlers set" << std::endl;
                firstConnection = true;
            }
            websocketpp::lib::error_code ec;
            client::connection_ptr con = c.get_connection(url, ec);
            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                return 0;
            }
            std::cout << "got connection" << std::endl;

            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            connectionHdl = c.connect(con);
            std::cout << "connected" << std::endl;
            // Start the ASIO io_service run loop
            // this will cause a single connection to be made to the server. c.run()
            // will exit when this connection is closed.


            std::cout << "Calling run" << std::endl;

            c.run();
            std::cout << "done run" << std::endl;
        }
    }
    catch (websocketpp::exception const& e) {
        std::cout << "exc: " << e.what() << std::endl;
        loggedIn = false;
        connectedToServer = false;
    }
    
}

void Multiplayer::login()
{
    if (!connectedToServer)
    {
        std::cout << "not connected!" << std::endl;
        return;
    }

    char c[5] = "dang";

    SteamAPICall_t call = SteamUser()->RequestEncryptedAppTicket(&c, 5);
    steamEncryptedAppTicketCall.Set(call, this, &Multiplayer::OnSteamAuthTicket);
    std::cout << "awaiting auth ticket from steam" << std::endl;
}

void Multiplayer::OnSteamAuthTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure)
{
    CPacketHello hello;

    hello.PacketType = eCPacketHello;

    std::cout << "got auth ticket" << std::endl;

    char* buf = new char[4096];

    uint32 ticketLength = 4096;

    bool result = SteamUser()->GetEncryptedAppTicket(buf, 4096, &ticketLength);

    if (!result)
    {
        std::cout << "bruh bruh" << std::endl;
        return;
    }

    char* ticket = new char[ticketLength];

    std::string bruh = std::string(buf,ticketLength);

    std::string encoded = macaron::Base64::Encode(bruh);

    hello.SteamTicket = encoded;

    sendMessage<CPacketHello>(hello);
}

void Multiplayer::inQuotesGracefullyDisconnect()
{
    c.close(connectionHdl, 1000, "no you");
}
