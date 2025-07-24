#pragma once

#include <memory>
#include <unordered_map>

#include "Core/Common/Ticker.h"


struct lws;
struct lws_context;
enum lws_callback_reasons : int32_t;

namespace OGS::WebSocket
{
    class CWebSocket;
    using CWebSocketPtr = std::shared_ptr<CWebSocket>;

    class CWebSocketManager : public CTicker
    {
    public:
        static CWebSocketManager& Get();

        void Init();
        void DeInit();

        CWebSocketPtr CreateSocket();

    private:
        CWebSocketManager();

        void Tick(double ElapsedSeconds) override;
        void CleanupDeadSockets();

        void RegisterSocket(const CWebSocketPtr& Socket);
        void UnRegisterSocket(const CWebSocketPtr& Socket);

    private:
        static int32_t HandleCallback(lws* Client, lws_callback_reasons Reason,
                                      void* User, void* In, size_t Len);

    private:
        lws_context* Context{nullptr};
        std::unordered_map<lws*, std::weak_ptr<CWebSocket>> Sockets;

        friend class CWebSocket;
        friend class CProtocolProvider;
    };
}
