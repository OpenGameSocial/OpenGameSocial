#include "NetAddress.h"

#include <cstring>
#include <arpa/inet.h>


OGS::Net::CNetAddress::CNetAddress()
{
    memset(&SockAddr4, 0, sizeof(SockAddr4));
    memset(&SockAddr6, 0, sizeof(SockAddr6));

    SockAddrLen = 0;
    AddressFamily = EAddressFamily::Unknown;
}

OGS::Net::CNetAddress::CNetAddress(const std::string& InAddress, uint16_t InPort)
{
    memset(&SockAddr4, 0, sizeof(SockAddr4));
    memset(&SockAddr6, 0, sizeof(SockAddr6));

    if (inet_pton(AF_INET, InAddress.c_str(), &SockAddr4.sin_addr) == 1)
    {
        AddressFamily = EAddressFamily::IpV4;
        SockAddr4.sin_family = AF_INET;
        SockAddr4.sin_port = htons(InPort);
        SockAddrLen = sizeof(SockAddr4);
    }
    else if (inet_pton(AF_INET6, InAddress.c_str(), &SockAddr6.sin6_addr) == 1)
    {
        AddressFamily = EAddressFamily::IpV6;
        SockAddr6.sin6_family = AF_INET6;
        SockAddr6.sin6_port = htons(InPort);
        SockAddrLen = sizeof(SockAddr6);
    }
}

std::string OGS::Net::CNetAddress::GetAddressImpl() const
{
    if (AddressFamily == EAddressFamily::IpV6)
    {
        char Buffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &SockAddr6.sin6_addr, Buffer, sizeof(Buffer));
        return Buffer;
    }

    char Buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &SockAddr4.sin_addr, Buffer, sizeof(Buffer));
    return Buffer;
}

uint16_t OGS::Net::CNetAddress::GetPortImpl() const
{
    if (AddressFamily == EAddressFamily::IpV6)
    {
        return ntohs(SockAddr6.sin6_port);
    }

    return ntohs(SockAddr4.sin_port);
}

void OGS::Net::CNetAddress::UpdatePortImpl(uint16_t InPort)
{
    if (AddressFamily == EAddressFamily::IpV6)
    {
        SockAddr6.sin6_port = htons(InPort);
    }
    else
    {
        SockAddr4.sin_port = htons(InPort);
    }
}

const sockaddr* OGS::Net::CNetAddress::GetSockAddr() const
{
    if (AddressFamily == EAddressFamily::IpV4)
    {
        return reinterpret_cast<const sockaddr*>(&SockAddr4);
    }

    return reinterpret_cast<const sockaddr*>(&SockAddr6);
}

sockaddr* OGS::Net::CNetAddress::GetSockAddr()
{
    if (AddressFamily == EAddressFamily::IpV4)
    {
        return reinterpret_cast<sockaddr*>(&SockAddr4);
    }

    return reinterpret_cast<sockaddr*>(&SockAddr6);
}

socklen_t OGS::Net::CNetAddress::GetSockAddrLen() const
{
    return SockAddrLen;
}

void OGS::Net::CNetAddress::LoadFromSockAddr(const sockaddr* InSockAddr)
{
    memset(&SockAddr4, 0, sizeof(SockAddr4));
    memset(&SockAddr6, 0, sizeof(SockAddr6));

    SockAddrLen = 0;
    AddressFamily = EAddressFamily::Unknown;

    if (InSockAddr->sa_family == AF_INET)
    {
        AddressFamily = EAddressFamily::IpV4;
        SockAddr4 = *reinterpret_cast<const sockaddr_in*>(InSockAddr);
        SockAddrLen = sizeof(SockAddr4);
    }
    else if (InSockAddr->sa_family == AF_INET6)
    {
        AddressFamily = EAddressFamily::IpV6;
        SockAddr6 = *reinterpret_cast<const sockaddr_in6*>(InSockAddr);
        SockAddrLen = sizeof(SockAddr6);
    }
}

void OGS::Net::CNetAddress::SetFamily(EAddressFamily InFamily)
{
    memset(&SockAddr4, 0, sizeof(SockAddr4));
    memset(&SockAddr6, 0, sizeof(SockAddr6));

    AddressFamily = InFamily;

    if (AddressFamily == EAddressFamily::IpV4)
    {
        SockAddrLen = sizeof(SockAddr4);
    }
    else if (AddressFamily == EAddressFamily::IpV6)
    {
        SockAddrLen = sizeof(SockAddr6);
    }
}
