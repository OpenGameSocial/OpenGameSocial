#include "Socket.h"
#include "OGS/Core.h"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>


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
        return 0;
    }
}

OGS::Net::CSocket::CSocket(ESocketType InType, EAddressFamily InAddressFamily)
    : TSocketInterface(InType, InAddressFamily)
{
    Socket = socket(GetAddressFamily(InAddressFamily), GetSocketType(InType), GetSocketProtocol(InType));

    if (Socket < 0)
    {
        DEBUG_BREAK();
        return;
    }
}

OGS::Net::CSocket::~CSocket()
{
    if (IsValid())
    {
        close(Socket);
        Socket = -1;
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

    select(Socket + 1, &SocketReadDescriptor, nullptr, nullptr, &Timeout);
}

bool OGS::Net::CSocket::ReceiveFromImpl(std::vector<uint8_t>& InBuffer, CNetAddress& InAddress) const
{
    if (!IsValid())
    {
        return false;
    }

    const int BufferSize = static_cast<int>(InBuffer.capacity());
    InBuffer.resize(BufferSize);

    InAddress.SetFamily(SocketAddress.GetAddressFamily());

    socklen_t AddressSize = InAddress.GetSockAddrLen();

    const ssize_t ReceivedLength = recvfrom(
        Socket,
        InBuffer.data(),
        BufferSize,
        0,
        InAddress.GetSockAddr(),
        &AddressSize
    );

    if (ReceivedLength < 0)
    {
        LastError = errno;

        if (LastError == EWOULDBLOCK)
        {
            LastError = 0;
        }

        return false;
    }

    InBuffer.resize(static_cast<size_t>(ReceivedLength));

    LastError = 0;
    return true;
}

bool OGS::Net::CSocket::SendToImpl(const std::vector<uint8_t>& InBuffer, const CNetAddress& InAddress) const
{
    if (!IsValid() || InBuffer.empty())
    {
        return false;
    }

    const ssize_t Result = sendto(
        Socket,
        InBuffer.data(),
        InBuffer.size(),
        0,
        InAddress.GetSockAddr(),
        InAddress.GetSockAddrLen()
    );

    if (Result < 0)
    {
        LastError = errno;

        if (LastError == EWOULDBLOCK)
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
    return Socket >= 0;
}

bool OGS::Net::CSocket::SetNonBlockingImpl() const
{
    if (Socket < 0)
    {
        return false;
    }

    const int flags = fcntl(Socket, F_GETFL, 0);

    if (flags == -1)
        return false;

    return fcntl(Socket, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool OGS::Net::CSocket::BindImpl(const CNetAddress& Address)
{
    if (!IsValid())
    {
        return false;
    }

    SocketAddress.LoadFromSockAddr(Address.GetSockAddr());

    return bind(Socket, Address.GetSockAddr(), Address.GetSockAddrLen()) != -1;
}
