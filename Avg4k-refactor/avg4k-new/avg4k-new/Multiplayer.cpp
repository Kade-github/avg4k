#include "Multiplayer.h"

#include "CPacketHello.h"
#include "CPacketStatus.h"
#include "SPacketHello.h"
#include "SPacketStatus.h"
#include "SPacketUpdateEncryptionParameters.h"


bool Multiplayer::connectedToServer = false;

bool Multiplayer::loggedIn = false;

Multiplayer* Multiplayer::MInstance = NULL;

std::string Multiplayer::tag = "";
std::string Multiplayer::username = "";

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

bool Multiplayer::integ = false;
bool Multiplayer::prot = false;
bool Multiplayer::debugger = false;
bool Multiplayer::vm = false;
bool Multiplayer::speed = false;

boost::lockfree::queue<PacketData*> Multiplayer::sendQueue(0);

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

bool IsPointerBad(void* p)
{
    VM_START
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    if (::VirtualQuery(p, &mbi, sizeof(mbi)))
    {
        DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
        bool b = !(mbi.Protect & mask);
        // check the page is not a guard page
        if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

        return b;
    }
    VM_END
    return true;
}

typedef BOOL WINAPI RtlQueryPerformanceCounter_t(LARGE_INTEGER* ticks);
int Multiplayer::DetectSpeedhack(std::vector<char>* function) {

    VM_START
    RtlQueryPerformanceCounter_t* perfCounter = (RtlQueryPerformanceCounter_t*)GetProcAddress(GetModuleHandleA("ntdll"), "RtlQueryPerformanceCounter");
        
    if (!perfCounter)
        return -1;

    int iptr = 0;

    bool flag = false;

    do {

        uint8_t* byte = (uint8_t*)perfCounter + iptr;

        unsigned char b = *byte;

        if (b == (unsigned char)0xE9 && iptr == 0) {

            int32_t* followThrough = (int32_t*)((uint8_t*)perfCounter + 1);

            int32_t offsetOfHook = *followThrough;

            if (IsPointerBad((void*)(offsetOfHook + (uint64_t)followThrough))) {
                return 0;
            }

            __try {

                unsigned char* addr = (unsigned char*)offsetOfHook + (uint64_t)followThrough;
                addr += 4;

                while (addr[0] == (unsigned char)0xFF && addr[1] == (unsigned char)0x25) {
                    //Trampoline 14byte jump
                   
                    //FF25 is the opcode for the 14 byte jump followed by 4 bytes of 0 followed by the address to jmp to
                    addr = (unsigned char*)(*(uint64_t*)(addr + 6));
               }

               
                for (int i = 0; i < 5000; i++) {

                    
                    unsigned char byte = addr[i];

                    
                    function->push_back(byte);
                    
                    if ((unsigned char)byte == (unsigned char)0xC9)
                        break;
                }

            }
            __except (1) {

            }

            if (function->size() > 0)
                return 1;

        }

        iptr++;
    } while (iptr < 50);

    VM_END
    return 0;
}


DWORD WINAPI SendPacketT(LPVOID param) {

    
        while (true) {
        
          
        if (!connectionData) {
            Sleep(1);
            continue;
        }
        

        PacketData* packetData;

        if (!Multiplayer::sendQueue.pop(packetData)) {
            Sleep(1);
            continue;
         }

            VM_START

            websocketpp::lib::error_code ec;

            std::string dataStr = packetData->data;

            char* sendData = (char*)malloc(dataStr.length() + 8);

            if (!sendData) {
	            AvgEngine::Logging::writeLog("[Server] Alloc error 1 (Packet Dropped)");
                return 0;
            }

            char* writeTo = (char*)(((__int64)sendData) + 8);

            __int32* dataPtrs = (__int32*)sendData;
            dataPtrs[0] = packetData->packetType;
            dataPtrs[1] = (__int32)dataStr.length();

            memcpy(writeTo, dataStr.c_str(), dataStr.length());

            unsigned char* cipherplusIV = (unsigned char*)malloc(dataStr.length() + 256);

            if (!cipherplusIV) {
	            AvgEngine::Logging::writeLog("[Server] Alloc error 2 (Packet Dropped)");
                return 0;
            }

            unsigned char* ciphertext = cipherplusIV + 16;


            int len;
            int ciphertext_len;

            unfuckPlease.lock();

            aesCtx = EVP_CIPHER_CTX_new();

            if (!aesCtx) {
	            AvgEngine::Logging::writeLog("[Server] Crypto error 1 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }

            if (EVP_EncryptInit_ex(aesCtx, EVP_aes_256_cbc(), NULL, NULL, NULL) != 1) {
	            AvgEngine::Logging::writeLog("[Server] Crypto error 2 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }


            if (EVP_CIPHER_CTX_set_padding(aesCtx, EVP_PADDING_PKCS7) != 1) {
	            AvgEngine::Logging::writeLog("[Server] Crypto error 4 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }



            if (RAND_bytes(cipherplusIV, 16) != 1) {
	            AvgEngine::Logging::writeLog("[Server] Crypto error 5 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }

            unsigned char buf[32];

            for (int i = 0; i < 32; i++) {
                buf[i] = key[i] ^ keykey[i];
            }

            if (EVP_EncryptInit_ex(aesCtx, NULL, NULL, buf, cipherplusIV) != 1) {
	            AvgEngine::Logging::writeLog("[Server] Crypto error 6 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }

            if (EVP_EncryptUpdate(aesCtx, ciphertext, &len, (unsigned char*)sendData, dataStr.length() + 8) != 1) {
                AvgEngine::Logging::writeLog("[Server] Crypto error 7 (Packet Dropped)");
                unfuckPlease.unlock();
                return 0;
            }

            ciphertext_len = len;

            if (EVP_EncryptFinal_ex(aesCtx, ciphertext + len, &len) != 1) {
                AvgEngine::Logging::writeLog("[Server] Crypto error 8 (Packet Dropped)");
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
                AvgEngine::Logging::writeLog("[Server] Something something problem send: " + std::string(ex.what()));
            }

            if (!success && packetData->attempts < 5) {
                packetData->attempts++;
                Multiplayer::sendQueue.push(packetData);
            }
            else {
                delete packetData;
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
    PacketData* packetData = new PacketData();
    packetData->data = data;
    packetData->packetType = packet;

    Multiplayer::sendQueue.push(packetData);
    MUTATE_END
}



DWORD WINAPI NewThread(LPVOID param) {
    for (;;) {
        VM_START
        Sleep(1000);

        CPacketStatus heartbeat;
        heartbeat.PacketType = eCPacketStatus;
        heartbeat.Order = 0;
        heartbeat.code = 200;

        //if (Game::patched)
        //    heartbeat.code = 7393;

        heartbeat.Status = "hello";

        Multiplayer::sendMessage<CPacketStatus>(heartbeat);

        CPacketStatus send;
        int monkey = 32;
        int cock = 46;
        int cockmonkey = 567;
        int monkeysexCock = 7372;

        if (Multiplayer::integ) {
            CHECK_CODE_INTEGRITY(monkey, 64);

            if (monkey != 64) {

                send.code = 6743;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            Multiplayer::integ = false;
        }

        if (Multiplayer::speed) {
            Multiplayer::speed = false;
            std::vector<char> funcDump = std::vector<char>();

            int ret = Multiplayer::DetectSpeedhack(&funcDump);

            if (ret > 0 && funcDump.size() > 0) {

                CPacketStatus status;
                status.PacketType = eCPacketStatus;
                status.code = 3307;
                status.Status = macaron::Base64::Encode(std::string(funcDump.data(), funcDump.size()));
                Multiplayer::sendMessage<CPacketStatus>(status);
            }
            else if (ret > 0) {

                CPacketStatus status;
                status.PacketType = eCPacketStatus;
                status.code = 3307;
                status.Status = "Error with pointer result";
                Multiplayer::sendMessage<CPacketStatus>(status);
               
            }
            else if (ret == 0)
            {
                CPacketStatus status;
                status.PacketType = eCPacketStatus;
                status.code = 3307;
                status.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(status);
               
            }
            else if (ret < 0)
            {
                CPacketStatus status;
                status.PacketType = eCPacketStatus;
                status.code = 3307;
                status.Status = "Error occured: " + std::to_string(ret);
                Multiplayer::sendMessage<CPacketStatus>(status);   
            }
            
        }
        if (Multiplayer::prot) {
            CHECK_PROTECTION(cock, 76);

            if (cock != 76) {
                send.code = 6744;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            Multiplayer::prot = false;
        }

        if (Multiplayer::debugger) {
            CHECK_DEBUGGER(cockmonkey, 878);

            if (cockmonkey != 878) {
                send.code = 6745;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            Multiplayer::debugger = false;
        }

        if (Multiplayer::vm) {
            CHECK_VIRTUAL_PC(monkeysexCock, 4234);
            if (cockmonkey != 4234) {
                send.code = 6746;
                send.Order = 0;
                send.PacketType = eCPacketStatus;
                send.Status = "ok";
                Multiplayer::sendMessage<CPacketStatus>(send);
            }
            Multiplayer::vm = false;
        }

        VM_END
    }
}

DWORD WINAPI pleaseLogin(LPVOID agh)
{
    //Game::multi->login();

    return 0;
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    
    PacketType type;
    if (msg->get_opcode() != websocketpp::frame::opcode::BINARY)
        return;
    try {
       
        VM_START

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

        //Logging::writeLog("buffers allocated");

        EVP_CIPHER_CTX* ctx = NULL;

        ctx = EVP_CIPHER_CTX_new();



        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL);

        //Logging::writeLog("cxt created ");

        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, 16, NULL);
        EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);

       // Logging::writeLog("padding shit set");

        unsigned char keyDec[32];

        for (int i = 0; i < 32; i++) {
            keyDec[i] = key[i] ^ keykey[i];
        }


        EVP_DecryptInit_ex(ctx, NULL, NULL, keyDec, (unsigned char*)iv);

        memset(keyDec, 0, 32);
       // Logging::writeLog("decrypt inited");

        EVP_DecryptUpdate(ctx, (unsigned char*)plaintext, &len, (unsigned char*)ciphertext, payloadLength);
       // Logging::writeLog("first update done");

        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext + len, &len);
       // Logging::writeLog("done decrypt");
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);


       // Logging::writeLog("freed cxt");
        memset(ciphertext, 0, payloadLength);
        free(ciphertext);
        //Logging::writeLog("freed ciphertext");

        std::string plaintex = std::string(plaintext, plaintext_len);

       // std::string encoded = macaron::Base64::Encode(plaintex);

        //Logging::writeLog("Decrypted message: " << encoded);


        int32_t* ints = (int32_t*)plaintext;

        int32_t packetType = ints[0];
        int32_t length = ints[1];

        //Logging::writeLog("Packet length: " << length);
        //Logging::writeLog("packet type: " << packetType);

        size_t size = (size_t)length;

        char* data = (char*)((__int64)(plaintext)+8);

        type = (PacketType)packetType;

        SPacketStatus status;
        SPacketHello helloBack;
        SPacketUpdateEncryptionParameters enc;

        msgpack::unpacked result;

        msgpack::object obj;


        VM_END
        switch (type)
        {
        case eSPacketStatus: {

            unpack(result, data, length);

            obj = msgpack::object(result.get());

            obj.convert(status);

            switch (status.code)
            {
            case 403:
                VM_START
                AvgEngine::Logging::writeLog("[Server] Logging in...");
				Multiplayer::MInstance->login();
                VM_END
                break;
            case 409:
                VM_START
                AvgEngine::Logging::writeLog("[Server] Conflict status...");
                VM_END
                break;
            case 404:
                VM_START
                AvgEngine::Logging::writeLog("[Server] Not Found status");
                VM_END
                break;
            case 3301:
                VM_START
                Multiplayer::integ = true;
                VM_END
                break;
            case 3302:
                VM_START
                Multiplayer::prot = true;
                VM_END
                break;
            case 3303:
                VM_START
                Multiplayer::debugger = true;
                VM_END
                break;
            case 3304:
                VM_START
                Multiplayer::vm = true;
                VM_END
                break;
            case 3305:
                VM_START
                AvgEngine::Logging::writeLog("[Server] " + status.Status);
                VM_END
                break;
            case 3306:
                VM_START
                    AvgEngine::Logging::writeLog("[Server] " + status.Status);
                VM_END
                break;
            case 3307:
                VM_START
                Multiplayer::speed = true;
                VM_END
                break;
            }
            VM_START

            VM_END
            break;
        }
        case eSPacketHello: {
            VM_START
            //Logging::writeLog("bruh moment -1");
            unpack(result, data, length);

            //Logging::writeLog("bruh moment 0");

            obj = msgpack::object(result.get());

            //Logging::writeLog("bruh moment 0.5");

            obj.convert(helloBack);

            // Logging::writeLog("bruh moment 1");

            CreateThread(NULL, NULL, NewThread, NULL, NULL, NULL);
            Multiplayer::loggedIn = true;

            //Logging::writeLog("bruh moment 2");

            if (reauth)
                delete reauth;

            reauth = new std::string(helloBack.reauth);

            AvgEngine::Logging::writeLog("[Server] Server agreed to give me the kids on the weekends (you logged in successfully)");
            VM_END
            break;
        }

        case eSPacketUpdateEncryptionParameters: {

            VM_START

            unpack(result, data, length);

            obj = msgpack::object(result.get());

            obj.convert(enc);


            std::string out;
            std::string empty = macaron::Base64::Decode(enc.key, out);

            memcpy(key, out.c_str(), out.length());

            RAND_bytes(keykey, 32);

            for (int i = 0; i < 32; i++) {
                key[i] = key[i] ^ keykey[i];
            }
            VM_END
            break;
        }
        default:
            VM_START

            VM_END
            break;
        }
    }
    catch (std::exception e) {
        AvgEngine::Logging::writeLog("[Server] Shit " + std::string(e.what()) + " - THINGY CASTING " + std::to_string(type));
    }
    
    __nop();
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
        AvgEngine::Logging::writeLog(e.what());
    }
    return ctx;
}

bool firstConnection = false;


void Multiplayer::connect(AvgEngine::Game* g)
{
    if (!MInstance)
		MInstance = new Multiplayer();
    std::string build = g->Version;
    bool debug = false;
    UNPROTECTED_START
    build = "(DEBUG) " + build;
    debug = true;
    UNPROTECTED_END

    VM_START
    connectedToServer = false;

   
    std::string url = "wss://titnoas.xyz/ballsandsex/";
   
    AvgEngine::Logging::writeLog("[Server] Connecting...");
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


                firstConnection = true;

            }
            else {
                // please dont ddos my webserver thanks
                // ok <3
                Sleep(1000);
            }

            connectionData->c.set_message_handler(bind(&on_message, &connectionData->c, ::_1, ::_2));

            connectionData->c.set_tls_init_handler(bind(&on_tls_init, "titnoas.xyz", ::_1));




            websocketpp::lib::error_code ec;
            client::connection_ptr con = connectionData->c.get_connection(url, ec);
            if (ec) {
                AvgEngine::Logging::writeLog("[Server] Connection Failed: " + ec.message());
                return;
            }
            AvgEngine::Logging::writeLog("[Server] Connected successfully!");

            if (reauth) {
                con->append_header("Reauth", *reauth);
                Multiplayer::loggedIn = true;
            }
            con->append_header("Encrypted", "1");
            con->append_header("IV", "dynamic");
            con->append_header("Version", build);
            con->append_header("ProtoVersion", "2");
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
            //Logging::writeLog("Base64: " << base64Encoded);
            con->append_header("Key", base64Encoded);


            OPENSSL_free(out);
            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            connectionData->connectionHdl = connectionData->c.connect(con);

            // Start the ASIO io_service run loop
            // this will cause a single connection to be made to the server. c.run()
            // will exit when this connection is closed.


            connectedToServer = true;
            connectionData->c.run();
            AvgEngine::Logging::writeLog("[Server] Connection closed");
            Multiplayer::loggedIn = false;
            connectedToServer = false;

            auto closeCode = con->get_remote_close_code();
            std::string closeReason = con->get_remote_close_reason();

            if (closeReason.size() != 0)
            {
                AvgEngine::Logging::writeLog("[Server] Disconnected by server: " + closeReason);
            }


            connectionData->c.reset();
        }
    }
    catch (websocketpp::exception const& e) {
        AvgEngine::Logging::writeLog("[Server] Server connection failed: " + std::string(e.what()));
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
        AvgEngine::Logging::writeLog("not connected!");
        return;
    }

    char c[5] = "dang";


    if (SteamUser() == nullptr) {
        AvgEngine::Logging::writeLog("[Server] User not logged into steam");
        return;
    }

    SteamAPICall_t call = SteamUser()->RequestEncryptedAppTicket(&c, 5);
    steamEncryptedAppTicketCall.Set(call, this, &Multiplayer::OnSteamAuthTicket);
    AvgEngine::Logging::writeLog("[Server] Awaiting auth ticket from steam");
    VM_END
}

void Multiplayer::OnSteamAuthTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure)
{
    VM_START
    CPacketHello hello;

    hello.PacketType = eCPacketHello;

    AvgEngine::Logging::writeLog("[Server] Sending auth ticket...");

    char* buf = new char[4096];

    uint32 ticketLength = 4096;

    bool result = SteamUser()->GetEncryptedAppTicket(buf, 4096, &ticketLength);

    if (!result)
    {
        AvgEngine::Logging::writeLog("[STEAM] Auth ticket was bad");
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
