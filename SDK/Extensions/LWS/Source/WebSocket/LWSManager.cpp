#include "LWSManager.h"

#include <libwebsockets.h>
#include <ranges>

#include "LWSWebSocket.h"
#include "Core/Common/Codegen.h"
#include "Core/Logging/Logger.h"


static OGS::TLogCategory LogWebSocketManager("LogWebSocketManager");
static OGS::TLogCategory LogLWS("LogLWS");

namespace OGS::LWS
{
    class CProtocolProvider final
    {
    private:
        static int32_t HandleCallback(lws* Client, lws_callback_reasons Reason, void* User, void* In,
                                              size_t Len)
        {
            return CLWSManager::Get().HandleCallback(Client, Reason, User, In, Len);
        }

    public:
        static inline lws_protocols Protocols[] = {
            {
                "ws",
                HandleCallback,
                0,
                65536,
            },
            {
                "wss",
                HandleCallback,
                0,
                65536,
            },
            {nullptr, nullptr, 0, 0}
        };
    };

    static void LwsLogCallback(int level, const char* line)
    {
        std::string_view LineView(line);

        while (LineView.ends_with('\n') || LineView.ends_with('\r'))
        {
            LineView.remove_suffix(1);
        }

        switch (level)
        {
        case LLL_ERR:
            LogLWS.Error(LineView);
            break;
        case LLL_WARN:
            LogLWS.Warning(LineView);
            break;
        case LLL_NOTICE:
        case LLL_DEBUG:
            LogLWS.Verbose(LineView);
            break;
        case LLL_INFO:
            LogLWS.Info(LineView);
            break;
        }
    }

    CLWSManager& CLWSManager::Get()
    {
        static CLWSManager Instance;
        return Instance;
    }

    void CLWSManager::Init()
    {
        if (Context != nullptr)
        {
            LogWebSocketManager.Warning("WebSocketManager already initialized");
            return;
        }

        struct lws_context_creation_info info = {};
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = CProtocolProvider::Protocols;
        info.gid = -1;
        info.uid = -1;
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

        Context = lws_create_context(&info);
        if (!Context)
        {
            LogWebSocketManager.Error("Failed to create libwebsockets context");
            return;
        }

        LogWebSocketManager.Info("WebSocketManager initialized successfully");
    }

    void CLWSManager::DeInit()
    {
        if (Context == nullptr)
        {
            return;
        }

        // Close all active sockets
        std::vector<CLWSWebSocketPtr> ActiveSockets;

        for (auto& WeakSocket : Sockets | std::views::values)
        {
            if (auto Socket = WeakSocket.lock())
            {
                ActiveSockets.push_back(Socket);
            }
        }

        for (auto& Socket : ActiveSockets)
        {
            Socket->Close();
        }

        // Clean up context
        lws_context_destroy(Context);
        Context = nullptr;
        Sockets.clear();

        LogWebSocketManager.Info("WebSocketManager deinitialized");
    }

    CLWSWebSocketPtr CLWSManager::CreateSocket()
    {
        if (Context == nullptr)
        {
            LogWebSocketManager.Error("WebSocketManager not initialized");
            return nullptr;
        }

        return CLWSWebSocketPtr(new CLWSWebSocket());
    }

    CLWSManager::CLWSManager()
    {
        lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_HEADER | LLL_EXT | LLL_CLIENT | LLL_LATENCY | LLL_USER, LwsLogCallback);
    }

    void CLWSManager::Tick(double ElapsedSeconds)
    {
        if (Context == nullptr)
        {
            return;
        }

        lws_service(Context, -1);

        CleanupDeadSockets();
    }

    void CLWSManager::CleanupDeadSockets()
    {
        auto Iterator = Sockets.begin();

        while (Iterator != Sockets.end())
        {
            if (Iterator->second.expired())
            {
                Iterator = Sockets.erase(Iterator);
                continue;
            }

            ++Iterator;
        }
    }

    void CLWSManager::RegisterSocket(const CLWSWebSocketPtr& Socket)
    {
        if (Socket->Connection == nullptr)
        {
            LogWebSocketManager.Error("Socket connection is null");
        }

        Sockets[Socket->Connection] = Socket;
    }

    void CLWSManager::UnRegisterSocket(const CLWSWebSocketPtr& Socket)
    {
        Sockets.erase(Socket->Connection);
    }

    int32_t CLWSManager::HandleCallback(lws* Client, int32_t Reason, void* User, void* In,
                                              size_t Len)
    {
        const auto Iterator = Sockets.find(Client);

        if (Iterator == Sockets.end())
        {
            return 0;
        }

        const auto Socket = Iterator->second.lock();
        if (!Socket)
        {
            Sockets.erase(Iterator);
            return 0;
        }

        return Socket->HandleCallback(Reason, User, In, Len);
    }
}

CAutoInit WebSocketManagerAutoInit(new CFunctionAutoInitable([]
{
    OGS::LWS::CLWSManager::Get().Init();
}));
