#include <iostream>
#include <thread>
#include <string>
#include <curl/curl.h>
#include <libwebsockets.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/error.h>
#include <mbedtls/ssl.h>

#include "cacert.h"
#include "OpenGameSocial.h"
#include "mbedtls/debug.h"


static void OnLoginWithOpenId(OGS_Result Result, void* UserObject, const OGS_Account_LoginWithOpenId_CallbackData* Data)
{
    // DEBUG_BREAK();
}

static void DoLogging(OGS_ELogLevel Level, const char* Message)
{
    printf("%s\n", Message);
}

// Callback для записи полученных данных в std::string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto totalSize = size * nmemb;
    auto* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

static CURLcode sslctx_function(CURL *curl, void *sslctx, void *userptr)
{
    auto* config = static_cast<mbedtls_ssl_config*>(sslctx);

    auto* ca_chain = new mbedtls_x509_crt;
    mbedtls_x509_crt_init(ca_chain);

    int ret = mbedtls_x509_crt_parse(ca_chain, CRootCertificate::Data, CRootCertificate::Size);
    if (ret < 0) {
        char errbuf[128];
        mbedtls_strerror(ret, errbuf, sizeof(errbuf));
        fprintf(stderr, "Failed to parse cert: %s\n", errbuf);
        return CURLE_SSL_CERTPROBLEM;
    }

    mbedtls_ssl_conf_ca_chain(config, ca_chain, nullptr);

    // Хранить указатель в userptr, чтобы потом освободить (если нужно)
    curl_easy_setopt(curl, CURLOPT_PRIVATE, ca_chain);
    return CURLE_OK;
}

static int interrupted = 0;

static int
callback_ws(struct lws *wsi, enum lws_callback_reasons reason,
            void *user, void *in, size_t len)
{
    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lwsl_user("Connection established\n");
        // Отправляем первое сообщение
        {
        const char *msg = "Hello from lws WSS client";
        unsigned char buf[LWS_PRE + 256];
        size_t msg_len = strlen(msg);
        memcpy(&buf[LWS_PRE], msg, msg_len);
        lws_write(wsi, &buf[LWS_PRE], msg_len, LWS_WRITE_TEXT);
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        printf("Received: %.*s\n", (int)len, (char *)in);
        {
        const char *msg = "Hello from lws WSS client";
        unsigned char buf[LWS_PRE + 256];
        size_t msg_len = strlen(msg);
        memcpy(&buf[LWS_PRE], msg, msg_len);
        lws_write(wsi, &buf[LWS_PRE], msg_len, LWS_WRITE_TEXT);
        }
        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        printf("Connection error\n");
        interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        printf("Connection closed\n");
        interrupted = 1;
        break;

    default:
        break;
    }

    return 0;
}

static const struct lws_protocols protocols[] = {
    {
        "wss",
        callback_ws,
        0,
        512,
    },
    { NULL, NULL, 0, 0 } /* terminator */
};

static void my_lws_logger(int level, const char *line) {
    // Можно фильтровать по уровню, либо выводить всё
    fprintf(stderr, "[LWS %d] %s\n", level, line);
}

int main()
{
    // lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_LATENCY, my_lws_logger);

    // mbedtls_debug_set_threshold(4);

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации libcurl\n";
        return 1;
    }

    struct lws_context_creation_info info;
    struct lws_client_connect_info ccinfo;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN; // клиент, не слушает
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context = lws_create_context(&info);
    if (!context) {
        printf("lws init failed\n");
        return -1;
    }

    memset(&ccinfo, 0, sizeof(ccinfo));
    ccinfo.context = context;
    ccinfo.address = "echo.websocket.org"; // или любой wss сервер
    ccinfo.port = 443;
    ccinfo.path = "/";
    ccinfo.host = ccinfo.address;
    ccinfo.origin = ccinfo.address;
    ccinfo.protocol = protocols[0].name;
    ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED; // SSL + разрешить self-signed (если надо)

    struct lws *wsi = lws_client_connect_via_info(&ccinfo);
    if (!wsi) {
        printf("Client connect failed\n");
        lws_context_destroy(context);
        return -1;
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, sslctx_function);

    // Включаем валидацию SSL-сертификата
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Указываем callback и буфер
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Выполняем запрос
    CURLcode res = curl_easy_perform(curl);

    // if (res != CURLE_OK) {
    //     std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
    // } else {
    //     std::cout << "Ответ от сервера:\n" << response << "\n";
    // }

    OGS_SetLogger(OGS_Verbose, true, DoLogging);

    {
        OGS_Init_Options Options{};
        Options.ThreadPoolSize = 4;
        OGS_Init(&Options);
    }

    {
        OGS_Account_LoginWithOpenId_Options Options{};
        Options.IdentityService = "Test";
        Options.Token = "testtoken";

        OGS_Account_LoginWithOpenId(&Options, nullptr, OnLoginWithOpenId);
    }

    while (true)
    {
        OGS_Platform_Tick(0.1);
        lws_service(context, 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    curl_easy_cleanup(curl);
    lws_context_destroy(context);

    return 0;
}
