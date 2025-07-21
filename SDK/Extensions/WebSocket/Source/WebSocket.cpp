#include "WebSocket.h"

namespace OGS::WebSocket
{
    CWebSocketManager& CWebSocketManager::Get()
    {
        static CWebSocketManager Instance;
        return Instance;
    }
}
