#pragma once

#include <string>


namespace OGS
{
    constexpr uint32_t JoaatHash(const char* Str, size_t Len, uint32_t Seed = 0)
    {
        for (size_t i = 0; i < Len; ++i)
        {
            Seed += static_cast<uint8_t>(Str[i]);
            Seed += Seed << 10;
            Seed ^= Seed >> 6;
        }

        Seed += Seed << 3;
        Seed ^= Seed >> 11;
        Seed += Seed << 15;

        return Seed;
    }

    constexpr size_t ConstexprStrlen(const char* Str)
    {
        size_t len = 0;
        
        while (Str[len] != '\0')
        {
            ++len;
        }

        return len;
    }

    constexpr uint32_t JoaatHash(const char* Str)
    {
        return JoaatHash(Str, ConstexprStrlen(Str));
    }
}
