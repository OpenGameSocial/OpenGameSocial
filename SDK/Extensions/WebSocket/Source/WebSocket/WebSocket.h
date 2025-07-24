#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "WebSocketManager.h"
#include "Core/Common/MulticastDelegate.h"


struct lws;
struct lws_context;

namespace OGS::WebSocket
{
    enum class EConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };

    class CWebSocket final : public std::enable_shared_from_this<CWebSocket>
    {
    public:
        ~CWebSocket();

        bool Connect(std::string_view Url);
        void Close();

        void SetHeader(const std::string& Key, const std::string& Value);

        bool Send(const std::string& Message) const;

        [[nodiscard]] EConnectionState GetState() const
        {
            return ConnectionState;
        }

    private:
        CWebSocket();

        int32_t HandleCallback(lws_callback_reasons Reason, void* User, void* In, size_t Len);

        void CloseInternal(bool bDestroyed = false);

    public:
        TMulticastDelegate<EConnectionState> OnConnectionStateChanged;
        TMulticastDelegate<const std::string&> OnMessageReceived;

    private:
        EConnectionState ConnectionState = EConnectionState::Disconnected;

        lws_context* Context = nullptr;
        lws* Connection = nullptr;

        std::unordered_map<std::string, std::string> Headers;

        friend class CWebSocketManager;
    };
}
