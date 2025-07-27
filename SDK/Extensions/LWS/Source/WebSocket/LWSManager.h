#pragma once

#include <memory>
#include <unordered_map>

#include "Core/Common/Ticker.h"


struct lws;
struct lws_context;
enum lws_callback_reasons : int32_t;

namespace OGS::LWS
{
    class CLWSWebSocket;
    using CLWSWebSocketPtr = std::shared_ptr<CLWSWebSocket>;

    class CLWSManager : public CTicker
    {
    public:
        static CLWSManager& Get();

        void Init();
        void DeInit();

        CLWSWebSocketPtr CreateSocket();

    private:
        CLWSManager();

        void Tick(double ElapsedSeconds) override;
        void CleanupDeadSockets();

        void RegisterSocket(const CLWSWebSocketPtr& Socket);
        void UnRegisterSocket(const CLWSWebSocketPtr& Socket);

    private:
        static int32_t HandleCallback(lws* Client, lws_callback_reasons Reason,
                                      void* User, void* In, size_t Len);

    private:
        lws_context* Context{nullptr};
        std::unordered_map<lws*, std::weak_ptr<CLWSWebSocket>> Sockets;

        friend class CLWSWebSocket;
        friend class CProtocolProvider;
    };
}
