#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include "Core/Network/NetAddressInterface.h"


namespace OGS::Net
{
    class CNetAddress final : public TNetAddressInterface<CNetAddress>
    {
    public:
        CNetAddress();
        CNetAddress(const std::string& InAddress, uint16_t InPort);

        [[nodiscard]] std::string GetAddressImpl() const;
        [[nodiscard]] uint16_t GetPortImpl() const;

        void UpdatePortImpl(uint16_t InPort);

        [[nodiscard]] const sockaddr* GetSockAddr() const;
        [[nodiscard]] sockaddr* GetSockAddr();

        [[nodiscard]] socklen_t GetSockAddrLen() const;

        void LoadFromSockAddr(const sockaddr* InSockAddr);

        void SetFamily(EAddressFamily InFamily);

    private:
        sockaddr_in SockAddr4{};
        sockaddr_in6 SockAddr6{};
        socklen_t SockAddrLen = 0;
    };
}
