#include "Multiplayer.h"
#include "Game.h"
#include "Gameplay.h"



bool Multiplayer::connectedToServer = false;

bool Multiplayer::loggedIn = false;

std::string Multiplayer::tag = "";
std::string Multiplayer::username = "";
Color Multiplayer::tagColor;
Color Multiplayer::nameColor;

ConnectionData* connectionData;
std::string Multiplayer::currentUserAvatar = "";

std::string username;

std::string* reauth;

RSA* rsa;
EVP_PKEY_CTX* cryptoCtx;
ENGINE* engine;

EVP_CIPHER_CTX* aesCtx;

unsigned char* key;
unsigned char* keykey;

unsigned char iv[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

std::queue<PacketData> Multiplayer::sendQueue;
std::mutex Multiplayer::sendQueueLock;

std::mutex ConnectionLock;

std::mutex unfuckPlease;

void Multiplayer::InitCrypto() {
    

    VM_START
        //  STR_ENCRYPT_START
           // STR_ENCRYPT_START
        const char* publicKeyBase64 = "LS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlJQklqQU5CZ2txaGtpRzl3MEJBUUVGQUFPQ0FROEFNSUlCQ2dLQ0FRRUEzd3J2di9iYTl4SXZTZWd1b1lIagpvTGRrWHIyKys1K1ZzOHRBMEhvQnhGaWV6VnZSc3RyK1NDOXRGVDVSdzN6ZmpwWnVwd3N1MWp4dDFzVjFWQmdICm5EcjdUV2JydTI2Nm1kZ2d2RHZxVk82cTAxbDBsQkthYldmV2dlSnJvUE1NOVZJTkIvNHVRZEJ6TnQ0eFZnUk4Ka0lxaUJISFZNM3lVNlJsaVhFYkFzeURjL0lkc1RYTWxvdE45VjNvNDlpSmYrQmFiSldRNVNGUHZzbnIyanZoQQpoM1huQ3hxb016ajAzaG16cUdKK1VwSnhGbTdBYVE3WXBWZjdnYjJxcVREdUpYQkJ6cUQ3anBJTk5PcktNV0NmCnFxWVFWc3dDakp4WWhRMlNOaWozNkJ3Z29URUE5Ung3MmZNZU9rVFVvWjhrNENrRkV5MElSeHVFTVZveUtiVWgKd1FJREFRQUIKLS0tLS1FTkQgUFVCTElDIEtFWS0tLS0t";
       
    std::string publicKeyStr;

    std::string error = macaron::Base64::Decode(publicKeyBase64, publicKeyStr);

    const char* publicKey = publicKeyStr.c_str();

        BIO* bo = BIO_new(BIO_s_mem());
    BIO_write(bo, publicKey, strlen(publicKey));
    PEM_read_bio_RSA_PUBKEY(bo, &rsa, 0, 0);
    BIO_free(bo);


    EVP_PKEY* pKey = EVP_PKEY_new();

    EVP_PKEY_assign_RSA(pKey, rsa);

    engine = NULL;

    cryptoCtx = EVP_PKEY_CTX_new(pKey, engine);

    EVP_PKEY_encrypt_init(cryptoCtx);

    EVP_PKEY_CTX_set_rsa_padding(cryptoCtx, RSA_PKCS1_OAEP_PADDING);

    key = (unsigned char*)malloc(32);

    RAND_bytes(key, 32);

    keykey = (unsigned char*)malloc(32);

    RAND_bytes(keykey, 32);

    for (int i = 0; i < 32; i++) {
        key[i] = key[i] ^ keykey[i];
    }
        VM_END
    //STR_ENCRYPT_END
    
}


DWORD WINAPI SendPacketT(LPVOID param) {

    
        while (true) {
        
          
        if (!connectionData) {
            Sleep(1);
            continue;
        }
        

        Multiplayer::sendQueueLock.lock();

            if (Multiplayer::sendQueue.empty()) {
                Multiplayer::sendQueueLock.unlock();
                Sleep(50);
                continue;
            }
           
            PacketData packetData = Multiplayer::sendQueue.front();

            Multiplayer::sendQueueLock.unlock();

            VM_START

            websocketpp::lib::error_code ec;

            std::string dataStr = packetData.data;

            char* sendData = (char*)malloc(dataStr.length() + 8);

            if (!sendData) {
                std::cout << "Alloc error 1 (Packet Dropped)" << std::endl;
                return 0;
            }

            char* writeTo = (char*)(((__int64)sendData) + 8);

            __int32* dataPtrs = (__int32*)sendData;
            dataPtrs[0] = packetData.packetType;
            dataPtrs[1] = (__int32)dataStr.length();

            memcpy(writeTo, dataStr.c_str(), dataStr.length());

            unsigned char* cipherplusIV = (unsigned char*)malloc(dataStr.length() + 256);

            if (!cipherplusIV) {
                std::cout << "Alloc error 2 (Packet Dropped)" << std::endl;
                return 0;
            }

            unsigned char* ciphertext = cipherplusIV + 16;


            int len;
            int ciphertext_len;

            unfuckPlease.lock();

            aesCtx = EVP_CIPHER_CTX_new();

            if (!aesCtx) {
                std::cout << "Crypto error 1 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }

            if (EVP_EncryptInit_ex(aesCtx, EVP_aes_256_cbc(), NULL, NULL, NULL) != 1) {
                std::cout << "Crypto error 2 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }


            if (EVP_CIPHER_CTX_set_padding(aesCtx, EVP_PADDING_PKCS7) != 1) {
                std::cout << "Crypto error 4 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }



            if (RAND_bytes(cipherplusIV, 16) != 1) {
                std::cout << "Crypto error 5 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }

            unsigned char buf[32];

            for (int i = 0; i < 32; i++) {
                buf[i] = key[i] ^ keykey[i];
            }

            if (EVP_EncryptInit_ex(aesCtx, NULL, NULL, buf, cipherplusIV) != 1) {
                std::cout << "Crypto error 6 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }

            if (EVP_EncryptUpdate(aesCtx, ciphertext, &len, (unsigned char*)sendData, dataStr.length() + 8) != 1) {
                std::cout << "Crypto error 7 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }

            ciphertext_len = len;

            if (EVP_EncryptFinal_ex(aesCtx, ciphertext + len, &len) != 1) {
                std::cout << "Crypto error 8 (Packet Dropped)" << std::endl;
                unfuckPlease.unlock();
                return 0;
            }

            memset(buf, 0, 32);

            ciphertext_len += len;

            EVP_CIPHER_CTX_free(aesCtx);

            bool success = false;

            try {
                if (connectionData) {
                    connectionData->c.send(connectionData->connectionHdl, std::string((char*)cipherplusIV, ciphertext_len + 16), websocketpp::frame::opcode::BINARY, ec);
                    success = true;
                }
            }
            catch (std::exception ex) {
                std::cout << "something something problem send: " << ex.what() << std::endl;
            }
            if (success) {
                Multiplayer::sendQueueLock.lock();
                Multiplayer::sendQueue.pop();
                Multiplayer::sendQueueLock.unlock();
            }
            unfuckPlease.unlock();
            free(cipherplusIV);
            free(sendData);

            VM_END
        }
    return 0;
   
}
void Multiplayer::SendPacket(std::string data, PacketType packet) {

     MUTATE_START
    PacketData packetData = PacketData();
    packetData.data = data;
    packetData.packetType = packet;

    Multiplayer::sendQueueLock.lock();
    Multiplayer::sendQueue.push(packetData);
    Multiplayer::sendQueueLock.unlock();
    MUTATE_END
}

bool integ = false;
bool prot = false;
bool debugger = false;
bool vm = false;

DWORD WINAPI NewThread(LPVOID param) {
    for (;;) {
        VM_LION_BLACK_START
        Sleep(1000);

        CPacketStatus heartbeat;
        heartbeat.PacketType = eCPacketStatus;
        heartbeat.Order = 0;
        heartbeat.code = 200;

        if (Game::patched)
            heartbeat.code = 7393;

        heartbeat.Status = "hello";

        Multiplayer::sendMessage<CPacketStatus>(heartbeat);

        CPacketStatus send;
        int monkey = 32;
        int cock = 46;
        int cockmonkey = 567;
        int monkeysexCock = 7372;

        if (integ) {
            CHECK_CODE_INTEGRITY(monkey, 64);

            if (monkey != 64) {

                send.code = 6743;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            integ = false;
        }
        if (prot) {
            CHECK_PROTECTION(cock, 76);

            if (cock != 76) {
                send.code = 6744;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            prot = false;
        }

        if (debugger) {
            CHECK_DEBUGGER(cockmonkey, 878);

            if (cockmonkey != 878) {
                send.code = 6745;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            debugger = false;
        }

        if (vm) {
            CHECK_VIRTUAL_PC(monkeysexCock, 4234);
            if (cockmonkey != 4234) {
                send.code = 6746;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            vm = false;
        }

        VM_LION_BLACK_END
    }
}

DWORD WINAPI pleaseLogin(LPVOID agh)
{
    Game::multi->login();

    return 0;
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    VM_TIGER_WHITE_START
    PacketType type;
    if (msg->get_opcode() != websocketpp::frame::opcode::BINARY)
        return;
    try {
       
        std::string strData = msg->get_raw_payload();


        if (strData.length() <= 16)
            return;
        size_t payloadLength = strData.length();

        char* ciphertext = (char*)malloc(payloadLength);
        char iv[16];


        char* plaintext = (char*)malloc(payloadLength + 256);

        memcpy(ciphertext, strData.c_str() + 16, payloadLength - 16);

        memcpy(iv, strData.c_str(), 16);

        int len;
        int plaintext_len;

        //std::cout << "buffers allocated" << std::endl;

        EVP_CIPHER_CTX* ctx = NULL;

        ctx = EVP_CIPHER_CTX_new();



        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL);

        //std::cout << "cxt created " << std::endl;

        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 16, NULL);
        EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);

       // std::cout << "padding shit set" << std::endl;

        unsigned char keyDec[32];

        for (int i = 0; i < 32; i++) {
            keyDec[i] = key[i] ^ keykey[i];
        }


        EVP_DecryptInit_ex(ctx, NULL, NULL, keyDec, (unsigned char*)iv);

        memset(keyDec, 0, 32);
       // std::cout << "decrypt inited" << std::endl;

        EVP_DecryptUpdate(ctx, (unsigned char*)plaintext, &len, (unsigned char*)ciphertext, payloadLength);
       // std::cout << "first update done" << std::endl;

        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext + len, &len);
       // std::cout << "done decrypt" << std::endl;
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);


       // std::cout << "freed cxt" << std::endl;
        memset(ciphertext, 0, payloadLength);
        free(ciphertext);
        //std::cout << "freed ciphertext" << std::endl;

        std::string plaintex = std::string(plaintext, plaintext_len);

       // std::string encoded = macaron::Base64::Encode(plaintex);

        //std::cout << "Decrypted message: " << encoded << std::endl;


        int32_t* ints = (int32_t*)plaintext;

        int32_t packetType = ints[0];
        int32_t length = ints[1];

        //std::cout << "Packet length: " << length << std::endl;
        //std::cout << "packet type: " << packetType << std::endl;

        size_t size = (size_t)length;

        char* data = (char*)((__int64)(plaintext)+8);

        type = (PacketType)packetType;

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

                std::cout << "trying to login cuz unauthorized" << std::endl;
                CreateThread(NULL, NULL, pleaseLogin, NULL, NULL, NULL);

                break;
            case 409:
                std::cout << "conflict" << std::endl;
                break;
            case 404:
                std::cout << "not found" << std::endl;
                break;
            case 3301:             
                integ = true;
                break;
            case 3302:
                prot = true;
                break;
            case 3303:
                debugger = true;
                break;
            case 3304:
                vm = true;
                break;
            case 3305:
                Game::asyncShowErrorWindow("Message", status.Status, true);
                break;
            case 3306:
                Game::asyncShowErrorWindow("Message", status.Status, false);
                break;
            }
            p.data = data;
            p.length = length;
            p.type = type;
            p.ogPtr = plaintext;
            Game::instance->weGotPacket(p);
            break;
        case eSPacketHello:
            //std::cout << "bruh moment -1" << std::endl;
            unpack(result, data, length);

            //std::cout << "bruh moment 0" << std::endl;

            obj = msgpack::object(result.get());

            //std::cout << "bruh moment 0.5" << std::endl;

            obj.convert(helloBack);

           // std::cout << "bruh moment 1" << std::endl;

            CreateThread(NULL, NULL, NewThread, NULL, NULL, NULL);
            Multiplayer::loggedIn = true;

            //std::cout << "bruh moment 2" << std::endl;

            if (reauth)
                delete reauth;

            reauth = new std::string(helloBack.reauth);

            Multiplayer::username = SteamFriends()->GetPersonaName();

            Multiplayer::tag = helloBack.chatTag;
            Multiplayer::tagColor = { (int)(helloBack.tagColor.red * 255),(int)(helloBack.tagColor.green * 255),(int)(helloBack.tagColor.blue * 255) };
            Multiplayer::nameColor = { (int)(helloBack.nameColor.red * 255),(int)(helloBack.nameColor.green * 255),(int)(helloBack.nameColor.blue * 255) };

            Multiplayer::currentUserAvatar = helloBack.avatarURL;

            std::cout << helloBack.Message << ". hello server, fuck you too! " << std::endl;
            break;
        default:
            p.data = data;
            p.length = length;
            p.type = type;
            p.ogPtr = plaintext;
            Game::instance->weGotPacket(p);
            break;
        }
    }
    catch (std::exception e) {
        std::cout << "shit " << e.what() << " - THINGY CASTING " << type << std::endl;
    }
    VM_TIGER_WHITE_END
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

    std::string build = Game::version;
    bool debug = false;
    UNPROTECTED_START
    build = "(DEBUG) " + build;
    debug = true;
    UNPROTECTED_END

    VM_START
    connectedToServer = false;

   
    std::string url = "wss://titnoas.xyz/ballsandsex/";
   
    std::cout << "Creating things" << std::endl;
    CreateThread(NULL, NULL, SendPacketT, NULL, NULL, NULL);

    try {

        while (true)
        {
           
         
            if (!firstConnection)
            {
                connectionData = new ConnectionData();
                connectionData->c.set_access_channels(websocketpp::log::alevel::none);
                connectionData->c.clear_access_channels(websocketpp::log::alevel::all);
                connectionData->c.set_error_channels(websocketpp::log::elevel::none);

                // Initialize ASIO
                connectionData->c.init_asio();

                // Register our message handler
                std::cout << "Setting handlers" << std::endl;

                // Register our message handler
                std::cout << "Setting handlers" << std::endl;


                firstConnection = true;
                
            }
            else {
                //please dont ddos my webserver thanks
                Sleep(1000);
            }


            std::cout << "handlers set" << std::endl;

            connectionData->c.set_message_handler(bind(&on_message, &connectionData->c, ::_1, ::_2));

            connectionData->c.set_tls_init_handler(bind(&on_tls_init, "titnoas.xyz", ::_1));
           

            

            websocketpp::lib::error_code ec;
            client::connection_ptr con = connectionData->c.get_connection(url, ec);
            if (ec) {
                Game::asyncShowErrorWindow("Connection failure!", "Couldn't connect to Average4K Servers.", true);
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                return 0;
            }
            std::cout << "got connection" << std::endl;

            if (reauth) {
                con->append_header("Reauth", *reauth);
                Multiplayer::loggedIn = true;
            }
            con->append_header("Encrypted", "1");
            con->append_header("IV", "dynamic");
            con->append_header("Version", build);
            if (debug)
                con->append_header("Debug", "");

           // UNPROTECTED_START
           //     con->append_header("DebugBuild", "1");
            //UNPROTECTED_END

            size_t outLen;

            unsigned char buf[32];

            for (int i = 0; i < 32; i++) {
                buf[i] = key[i] ^ keykey[i];
            }

            EVP_PKEY_encrypt(cryptoCtx, NULL, &outLen, buf, 32);


            unsigned char* out = (unsigned char*)OPENSSL_malloc(outLen);

            EVP_PKEY_encrypt(cryptoCtx, out, &outLen, buf, 32);

            memset(buf, 0, 32);

            std::string encryptedData = std::string((char*)out, outLen);

            std::string base64Encoded = macaron::Base64::Encode(encryptedData);
            //std::cout << "Base64: " << base64Encoded << std::endl;
            con->append_header("Key", base64Encoded);
           

            OPENSSL_free(out);
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
            
            auto closeCode = con->get_remote_close_code();
            std::string closeReason = con->get_remote_close_reason();
            
            if (closeReason.size() != 0)
                Game::asyncShowErrorWindow("Disconnected",closeReason,true);


            connectionData->c.reset();
        }
    }
    catch (websocketpp::exception const& e) {
        std::cout << "exc: " << e.what() << std::endl;
        Game::startConnect = true;
        loggedIn = false;
        connectedToServer = false;
    }
    VM_END
}

void Multiplayer::login()
{
    VM_START
    if (!connectedToServer)
    {
        std::cout << "not connected!" << std::endl;
        return;
    }

    char c[5] = "dang";


    if (SteamUser() == nullptr) {
        Game::asyncShowErrorWindow("Steam not open!", "Steam is required to run this build.", true);
        std::cout << "User not logged into steam" << std::endl;
        Game::instance->isSteam = false;
        return;
    }

    SteamAPICall_t call = SteamUser()->RequestEncryptedAppTicket(&c, 5);
    steamEncryptedAppTicketCall.Set(call, this, &Multiplayer::OnSteamAuthTicket);
    std::cout << "awaiting auth ticket from steam" << std::endl;
    VM_END
}

void Multiplayer::OnSteamAuthTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure)
{
    VM_START
    CPacketHello hello;

    hello.PacketType = eCPacketHello;

    std::cout << "got auth ticket" << std::endl;

    char* buf = new char[4096];

    uint32 ticketLength = 4096;

    bool result = SteamUser()->GetEncryptedAppTicket(buf, 4096, &ticketLength);

    if (!result)
    {
        Game::asyncShowErrorWindow("Authorization Failure!", "Failed to authorize your reuqest.", true);
        std::cout << "bruh bruh" << std::endl;
        return;
    }

    char* ticket = new char[ticketLength];

    std::string bruh = std::string(buf,ticketLength);

    std::string encoded = macaron::Base64::Encode(bruh);

    hello.SteamTicket = encoded;

    sendMessage<CPacketHello>(hello);
    VM_END
}

void Multiplayer::inQuotesGracefullyDisconnect()
{
    connectionData->c.close(connectionData->connectionHdl, 1000, "no you");
}
