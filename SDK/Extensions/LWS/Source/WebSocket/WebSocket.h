#pragma once

#include "LWSManager.h"
#include "LWSWebSocket.h"


namespace OGS::WebSocket
{
    using CWebSocket = LWS::CLWSWebSocket;
    using CWebSocketManager = LWS::CLWSManager;
    using CWebSocketPtr = LWS::CLWSWebSocketPtr;
}
