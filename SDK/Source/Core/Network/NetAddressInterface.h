#pragma once

#include <string>


namespace OGS::Net
{
    enum class EAddressFamily
    {
        Unknown,

        IpV4,
        IpV6
    };

    template <typename TImpl>
    class TNetAddressInterface
    {
    public:
        virtual ~TNetAddressInterface() = default;

        [[nodiscard]] bool IsValid() const
        {
            return AddressFamily != EAddressFamily::Unknown;
        }

        [[nodiscard]] std::string GetAddress() const
        {
            return static_cast<const TImpl*>(this)->GetAddressImpl();
        }

        [[nodiscard]] uint16_t GetPort() const
        {
            return static_cast<const TImpl*>(this)->GetPortImpl();
        }

        void UpdatePort(uint16_t InPort)
        {
            static_cast<TImpl*>(this)->UpdatePortImpl(InPort);
        }

        [[nodiscard]] EAddressFamily GetAddressFamily() const
        {
            return AddressFamily;
        }

        [[nodiscard]] std::string ToString() const
        {
            return GetAddress() + ":" + std::to_string(GetPort());
        }

    protected:
        EAddressFamily AddressFamily = EAddressFamily::Unknown;
    };
}
