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
