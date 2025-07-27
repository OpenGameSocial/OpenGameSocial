#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "LWSManager.h"
#include "WebSocketEnums.h"
#include "Core/Common/MulticastDelegate.h"


struct lws;
struct lws_context;

namespace OGS::LWS
{
    class CLWSWebSocket final : public std::enable_shared_from_this<CLWSWebSocket>
    {
    public:
        ~CLWSWebSocket();

        bool Connect(std::string_view Url);
        void Close();

        void SetHeader(const std::string& Key, const std::string& Value);

        bool Send(const std::string& Message) const;

        [[nodiscard]] WebSocket::EConnectionState GetState() const
        {
            return ConnectionState;
        }

    private:
        CLWSWebSocket();

        int32_t HandleCallback(lws_callback_reasons Reason, void* User, void* In, size_t Len);

        void CloseInternal(bool bDestroyed = false);

    public:
        TMulticastDelegate<WebSocket::EConnectionState> OnConnectionStateChanged;
        TMulticastDelegate<const std::string&> OnMessageReceived;

    private:
        WebSocket::EConnectionState ConnectionState = WebSocket::EConnectionState::Disconnected;

        lws_context* Context = nullptr;
        lws* Connection = nullptr;

        std::unordered_map<std::string, std::string> Headers;

        friend class CLWSManager;
    };
}
