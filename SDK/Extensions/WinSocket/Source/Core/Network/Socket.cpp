#include "Socket.h"

#include "OGS/Core.h"

#pragma comment(lib, "Ws2_32.lib")


class WSA
{
public:
    ~WSA()
    {
        if (bWsaStartupCalled)
        {
            WSACleanup();
        }
    }

    int CallWSAStartup()
    {
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
        {
            bWsaStartupCalled = true;
        }

        return 0;
    }

private:
    bool bWsaStartupCalled = false;
};

static WSA WSAInstance;

static int GetAddressFamily(OGS::Net::EAddressFamily InAddressFamily)
{
    switch (InAddressFamily)
    {
    case OGS::Net::EAddressFamily::IpV4:
        return AF_INET;
    case OGS::Net::EAddressFamily::IpV6:
        return AF_INET6;
    default:
        DEBUG_BREAK();
        return AF_UNSPEC;
    }
}

static int GetSocketType(OGS::Net::ESocketType InType)
{
    switch (InType)
    {
    case OGS::Net::ESocketType::Tcp:
        return SOCK_STREAM;
    case OGS::Net::ESocketType::Udp:
        return SOCK_DGRAM;
    default:
        DEBUG_BREAK();
        return 0;
    }
}

static int GetSocketProtocol(OGS::Net::ESocketType InType)
{
    switch (InType)
    {
    case OGS::Net::ESocketType::Tcp:
        return IPPROTO_TCP;
    case OGS::Net::ESocketType::Udp:
        return IPPROTO_UDP;
    default:
        DEBUG_BREAK();
        return IPPROTO_RESERVED_MAX;
    }
}

OGS::Net::CSocket::CSocket(ESocketType InType, EAddressFamily InAddressFamily) :
TSocketInterface(InType, InAddressFamily)
{
    static auto _ = WSAInstance.CallWSAStartup();
    Socket = socket(GetAddressFamily(InAddressFamily), GetSocketType(InType), GetSocketProtocol(InType));

    if (Socket == INVALID_SOCKET)
    {
        DEBUG_BREAK();
        return;
    }
}

OGS::Net::CSocket::~CSocket()
{
    if (IsValid())
    {
        closesocket(Socket);
        Socket = INVALID_SOCKET;
    }
}

void OGS::Net::CSocket::TickImpl()
{
    static timeval Timeout{};

    if (!IsValid())
    {
        return;
    }

    FD_ZERO(&SocketReadDescriptor);
    FD_SET(Socket, &SocketReadDescriptor);

    select(0, &SocketReadDescriptor, nullptr, nullptr, &Timeout);
}

bool OGS::Net::CSocket::ReceiveFromImpl(std::vector<uint8_t>& InBuffer, CNetAddress& InAddress) const
{
    if (!IsValid())
    {
        return false;
    }

    // Assume buffer has enough reserved capacity
    const int BufferSize = static_cast<int>(InBuffer.capacity());
    InBuffer.resize(BufferSize);

    InAddress.SetFamily(SocketAddress.GetAddressFamily());

    // Should be explicitly int since recvfrom expects int* instead of socklen_t*
    int AddressSize = InAddress.GetSockAddrLen();
    const auto ReceivedLength = recvfrom(Socket,
                                  reinterpret_cast<char*>(InBuffer.data()), BufferSize,
                                  0,
                                  InAddress.GetSockAddr(), &AddressSize);

    if (ReceivedLength == SOCKET_ERROR)
    {
        LastError = WSAGetLastError();

        if (LastError == WSAEWOULDBLOCK)
        {
            LastError = 0;
        }

        return false;
    }

    InBuffer.resize(ReceivedLength);

    LastError = 0;
    return true;
}

bool OGS::Net::CSocket::SendToImpl(const std::vector<uint8_t>& InBuffer, const CNetAddress& InAddress) const
{
    if (!IsValid() || InBuffer.empty())
    {
        return false;
    }

    const auto Result = sendto(Socket, reinterpret_cast<const char*>(InBuffer.data()), static_cast<int>(InBuffer.size()), 0,
                  InAddress.GetSockAddr(), InAddress.GetSockAddrLen());

    if (Result == SOCKET_ERROR)
    {
        LastError = WSAGetLastError();

        if (LastError == WSAEWOULDBLOCK)
        {
            LastError = 0;
        }

        return false;
    }

    LastError = 0;
    return true;
}

bool OGS::Net::CSocket::HasDataImpl() const
{
    if (!IsValid())
    {
        return false;
    }

    return FD_ISSET(Socket, &SocketReadDescriptor);
}

bool OGS::Net::CSocket::IsValidImpl() const
{
    return Socket != INVALID_SOCKET;
}

bool OGS::Net::CSocket::SetNonBlockingImpl() const
{
    if (Socket == INVALID_SOCKET)
    {
        return false;
    }

    u_long mode = 1;
    return ioctlsocket(Socket, FIONBIO, &mode) == 0;
}

bool OGS::Net::CSocket::BindImpl(const CNetAddress& Address)
{
    if (!IsValid())
    {
        return false;
    }

    SocketAddress.LoadFromSockAddr(Address.GetSockAddr());
    return bind(Socket, Address.GetSockAddr(), Address.GetSockAddrLen()) != SOCKET_ERROR;
}
