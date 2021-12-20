#include "Multiplayer.h"
#include "Game.h"
#include "Gameplay.h"



bool Multiplayer::connectedToServer = false;

bool Multiplayer::loggedIn = false;

ConnectionData* connectionData;

std::string username;

std::string* reauth;

void Multiplayer::SendPacket(std::string data, PacketType packet) {

    websocketpp::lib::error_code ec;

    std::string dataStr = data;

    char* sendData = (char*)malloc(dataStr.length() + 8);

    char* writeTo = (char*)(((__int64)sendData) + 8);

    __int32* dataPtrs = (__int32*)sendData;
    dataPtrs[0] = packet;
    dataPtrs[1] = (__int32)dataStr.length();

    memcpy(writeTo, dataStr.c_str(), dataStr.length());

    connectionData->c.send(connectionData->connectionHdl, std::string(sendData, dataStr.length() + 8), websocketpp::frame::opcode::BINARY, ec);

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

            reauth = new std::string(helloBack.reauth);

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



bool verify_subject_alternative_name(const char* hostname, X509* cert) {
    STACK_OF(GENERAL_NAME)* san_names = NULL;

    san_names = (STACK_OF(GENERAL_NAME)*) X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
    if (san_names == NULL) {
        return false;
    }

    int san_names_count = sk_GENERAL_NAME_num(san_names);

    bool result = false;

    for (int i = 0; i < san_names_count; i++) {
        const GENERAL_NAME* current_name = sk_GENERAL_NAME_value(san_names, i);

        if (current_name->type != GEN_DNS) {
            continue;
        }

        char const* dns_name = (char const*)ASN1_STRING_get0_data(current_name->d.dNSName);

        // Make sure there isn't an embedded NUL character in the DNS name
        if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
            break;
        }
        // Compare expected hostname with the CN
        result = (strcmp(hostname, dns_name) == 0);
    }
    sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

    return result;
}

/// Verify that the certificate common name matches the given hostname
bool verify_common_name(char const* hostname, X509* cert) {
    // Find the position of the CN field in the Subject field of the certificate
    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
    if (common_name_loc < 0) {
        return false;
    }

    // Extract the CN field
    X509_NAME_ENTRY* common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
    if (common_name_entry == NULL) {
        return false;
    }

    // Convert the CN field to a C string
    ASN1_STRING* common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        return false;
    }

    char const* common_name_str = (char const*)ASN1_STRING_get0_data(common_name_asn1);

    // Make sure there isn't an embedded NUL character in the CN
    if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
        return false;
    }

    // Compare expected hostname with the CN
    return (strcmp(hostname, common_name_str) == 0);
}

/**
 * This code is derived from examples and documentation found ato00po
 * http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/example/cpp03/ssl/client.cpp
 * and
 * https://github.com/iSECPartners/ssl-conservatory
 */
bool verify_certificate(const char* hostname, bool preverified, boost::asio::ssl::verify_context& ctx) {
    //should prob do something wit this
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
           
            connectionData = new ConnectionData();

            if (!firstConnection)
            {
          
                firstConnection = true;
                
            }
            else {
                //please dont ddos my webserver thanks
                Sleep(5000);
            }

            connectionData->c.set_access_channels(websocketpp::log::alevel::none);
            connectionData->c.clear_access_channels(websocketpp::log::alevel::all);
            connectionData->c.set_error_channels(websocketpp::log::elevel::none);

            // Initialize ASIO
            connectionData->c.init_asio();

            // Register our message handler
            std::cout << "Setting handlers" << std::endl;

            // Register our message handler
            std::cout << "Setting handlers" << std::endl;




            std::cout << "handlers set" << std::endl;

            connectionData->c.set_message_handler(bind(&on_message, &connectionData->c, ::_1, ::_2));

            connectionData->c.set_tls_init_handler(bind(&on_tls_init, "titnoas.xyz", ::_1));

            websocketpp::lib::error_code ec;
            client::connection_ptr con = connectionData->c.get_connection(url, ec);
            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                return 0;
            }
            std::cout << "got connection" << std::endl;

            if(reauth)
                con->append_header("Reauth", *reauth);
            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            connectionData->connectionHdl = connectionData->c.connect(con);
            
            std::cout << "connected" << std::endl;
            // Start the ASIO io_service run loop
            // this will cause a single connection to be made to the server. c.run()
            // will exit when this connection is closed.
            

            std::cout << "Calling run" << std::endl;
            connectedToServer = true;
            connectionData->c.run();
            std::cout << "done run" << std::endl;
            Multiplayer::loggedIn = false;
            connectedToServer = false;

            delete connectionData;
        }
    }
    catch (websocketpp::exception const& e) {
        std::cout << "exc: " << e.what() << std::endl;
        Game::startConnect = true;
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
    connectionData->c.close(connectionData->connectionHdl, 1000, "no you");
}
