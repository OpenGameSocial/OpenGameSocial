#pragma once


namespace OGS::WebSocket
{
    enum class EConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };
}