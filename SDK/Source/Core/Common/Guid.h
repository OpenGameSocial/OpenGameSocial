#pragma once

#include <array>
#include <random>
#include <optional>


namespace OGS
{
    class CGuid final
    {
    public:
        CGuid();

        static std::optional<CGuid> FromString(const std::string& Str);

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] const std::array<uint8_t, 16>& GetData() const;

        bool operator==(const CGuid& Other) const;

        bool operator!=(const CGuid& Other) const;

    private:
        std::array<uint8_t, 16> Data{};
    };
}

template <>
struct std::hash<OGS::CGuid>
{
    std::size_t operator()(const OGS::CGuid& guid) const noexcept
    {
        const auto& Data = guid.GetData();
        std::size_t Hash = 0;

        for (size_t i = 0; i < 16; i += 4)
        {
            const std::size_t Part =
                (static_cast<std::size_t>(Data[i]) << 24) |
                (static_cast<std::size_t>(Data[i + 1]) << 16) |
                (static_cast<std::size_t>(Data[i + 2]) << 8) |
                static_cast<std::size_t>(Data[i + 3]);

            Hash ^= Part + 0x9e3779b9 + (Hash << 6) + (Hash >> 2);
        }

        return Hash;
    }
};
