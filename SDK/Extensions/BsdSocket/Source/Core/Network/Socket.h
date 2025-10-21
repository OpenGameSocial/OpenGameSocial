#pragma once

#include "Core/Network/SocketInterface.h"


namespace OGS::Net
{
    class CSocket final : public TSocketInterface<CSocket>
    {
    public:
        CSocket(ESocketType InType, EAddressFamily InAddressFamily);\
        ~CSocket() override;

        void TickImpl();

        [[nodiscard]] bool ReceiveFromImpl(std::vector<uint8_t>& InBuffer, CNetAddress& InAddress) const;

        [[nodiscard]] bool SendToImpl(const std::vector<uint8_t>& InBuffer, const CNetAddress& InAddress) const;

        [[nodiscard]] bool HasDataImpl() const;

        [[nodiscard]] bool IsValidImpl() const;

        [[nodiscard]] bool SetNonBlockingImpl() const;

        [[nodiscard]] bool BindImpl(const CNetAddress& Address);

    private:
        // Either bound or connected address
        CNetAddress SocketAddress;
        int Socket = -1;
        fd_set SocketReadDescriptor{};
        mutable int32_t LastError = 0;
    };
}
