#include "LWSWebSocket.h"

#include <libwebsockets.h>

#include "Core/Http/Uri.h"
#include "Core/Logging/Logger.h"


static OGS::TLogCategory LogWebSocket("LogWebSocket");

namespace OGS::LWS
{
    CLWSWebSocket::~CLWSWebSocket()
    {}

    bool CLWSWebSocket::Connect(std::string_view Url)
    {
        if (Context == nullptr)
        {
            return false;
        }

        const Http::Uri Uri(Url);

        if (!Uri.IsValid())
        {
            LogWebSocket.Error("Failed to parse WebSocket URL: %s", Url.data());
            return false;
        }

        lws_client_connect_info connect_info = {};
        connect_info.context = Context;
        connect_info.address = Uri.GetHost().c_str();
        connect_info.port = Uri.GetPort() == 0 ? 80 : Uri.GetPort();
        connect_info.path = Uri.GetPath().c_str();
        connect_info.host = connect_info.address;
        connect_info.origin = connect_info.address;
        connect_info.protocol = Uri.GetProtocol().c_str();

        if (Uri.GetProtocol() == "wss")
        {
            connect_info.ssl_connection = LCCSCF_USE_SSL;

            if (connect_info.port == 0)
            {
                connect_info.port = 443;
            }
        }

        lws* Client = lws_client_connect_via_info(&connect_info);
        if (!Client)
        {
            LogWebSocket.Error("Failed to initiate WebSocket connection to %s", Url.data());
            return false;
        }

        Connection = Client;
        ConnectionState = WebSocket::EConnectionState::Connecting;
        CLWSManager::Get().RegisterSocket(shared_from_this());

        LogWebSocket.Info("WebSocket connection initiated to %s", Url.data());
        return true;
    }

    void CLWSWebSocket::Close()
    {
        CloseInternal();
    }

    void CLWSWebSocket::SetHeader(const std::string& Key, const std::string& Value)
    {
        auto KeyCopy = Key;

        if (!KeyCopy.ends_with(':'))
        {
            KeyCopy += ':';
        }

        Headers.erase(KeyCopy);
        Headers.emplace(std::move(KeyCopy), Value);
    }

    bool CLWSWebSocket::Send(const std::string& Message) const
    {
        if (Connection == nullptr ||
            ConnectionState != WebSocket::EConnectionState::Connected)
        {
            return false;
        }

        const size_t MessageLen = Message.length();
        std::vector<unsigned char> buffer(LWS_PRE + MessageLen);

        memcpy(&buffer[LWS_PRE], Message.c_str(), MessageLen);

        return lws_write(Connection, &buffer[LWS_PRE], MessageLen, LWS_WRITE_TEXT) >= 0;
    }

    CLWSWebSocket::CLWSWebSocket()
    {
        Context = CLWSManager::Get().Context;
    }

    int32_t CLWSWebSocket::HandleCallback(lws_callback_reasons Reason, void* User, void* In, size_t Len)
    {
        switch (Reason)
        {
        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CLOSED:
        case LWS_CALLBACK_WSI_DESTROY:
            CloseInternal();
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        {
            CloseInternal(true);

            std::string Error;

            if (In && Len > 0)
            {
                Error = std::string(static_cast<char*>(In), Len);
            }

            LogWebSocket.Error("WebSocket connection error: [%s]", Error.c_str());
        }
        break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            ConnectionState = WebSocket::EConnectionState::Connected;
            OnConnectionStateChanged.Broadcast(WebSocket::EConnectionState::Connected);
            LogWebSocket.Info("WebSocket connection established");
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (In && Len > 0)
            {
                const std::string Message(static_cast<const char*>(In), Len);
                OnMessageReceived.Broadcast(Message);
            }
            break;

        case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
        {
            auto** Data = static_cast<unsigned char**>(In);
            auto* DataEnd = *Data + Len;


            for (const auto& [Key, Value] : Headers)
            {
                const auto* KeyPtr = reinterpret_cast<const uint8_t*>(Key.c_str());
                const auto* ValuePtr = reinterpret_cast<const uint8_t*>(Value.c_str());

                if (lws_add_http_header_by_name(Connection,
                                                KeyPtr,
                                                ValuePtr,
                                                static_cast<int32_t>(Value.length()),
                                                Data, DataEnd))
                {
                    CloseInternal();
                    LogWebSocket.Error("Failed to add [%s] header", Key.c_str());
                    return -1;
                }
            }

            break;
        }

        default:
            LogWebSocket.Verbose("Unhandled WebSocket callback: %d", Reason);
        }

        return 0;
    }

    void CLWSWebSocket::CloseInternal(bool bDestroyed)
    {
        if (Connection == nullptr)
        {
            return;
        }

        CLWSManager::Get().UnRegisterSocket(shared_from_this());

        if (!bDestroyed)
        {
            lws_close_reason(Connection, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
        }

        Connection = nullptr;
        ConnectionState = WebSocket::EConnectionState::Disconnected;

        OnConnectionStateChanged.Broadcast(ConnectionState);
    }
}
