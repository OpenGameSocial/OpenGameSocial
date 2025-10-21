#pragma once

#include <vector>

#include "Core/Network/NetAddress.h"


namespace OGS::Net
{
    enum class ESocketType
    {
        Unknown,

        Tcp,
        Udp
    };

    template <typename TImpl>
    class TSocketInterface
    {
    public:
        explicit TSocketInterface(ESocketType, EAddressFamily)
        {}

        virtual ~TSocketInterface() = default;

        [[nodiscard]] bool IsValid() const
        {
            return static_cast<const TImpl*>(this)->IsValidImpl();
        }

        [[nodiscard]] bool SetNonBlocking() const
        {
            return static_cast<const TImpl*>(this)->SetNonBlockingImpl();
        }

        [[nodiscard]] bool Bind(const CNetAddress& Address)
        {
            return static_cast<TImpl*>(this)->BindImpl(Address);
        }

        [[nodiscard]] bool ReceiveFrom(std::vector<uint8_t>& Buffer, CNetAddress& Address)
        {
            return static_cast<TImpl*>(this)->ReceiveFromImpl(Buffer, Address);
        }

        bool SendTo(const std::vector<uint8_t>& Buffer, const CNetAddress& Address)
        {
            return static_cast<TImpl*>(this)->SendToImpl(Buffer, Address);
        }

        void Tick()
        {
            static_cast<TImpl*>(this)->TickImpl();
        }

        [[nodiscard]] bool HasData()
        {
            return static_cast<TImpl*>(this)->HasDataImpl();
        }
    };
}
