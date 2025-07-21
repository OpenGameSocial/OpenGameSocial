#pragma once

#include <memory>


namespace OGS::WebSocket
{
    class CWebSocket final
    {
    public:

    };

    class CWebSocketManager
    {
    public:
        static CWebSocketManager& Get();

        std::shared_ptr<CWebSocket> CreateSocket();

    private:
        CWebSocketManager() = default;
    };
}
