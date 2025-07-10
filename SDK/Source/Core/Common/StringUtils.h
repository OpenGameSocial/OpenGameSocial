#pragma once

#include "OGS/Core.h"
#include <string>


namespace OGS
{
    template <typename... TArgs>
    std::string Printf(const char* Format, TArgs&&... Args)
    {
        thread_local std::string Buffer;

        auto Size = SCPRINTF(Format, std::forward<TArgs>(Args)...);

        if (Size + 1 > Buffer.capacity())
        {
            Buffer.reserve(Size + 1);
        }

        Buffer.resize(Size);

        SNPRINTF(Buffer.data(), Size + 1, Format, std::forward<TArgs>(Args)...);

        return Buffer;
    }

    template <typename... TArgs>
    std::string Printf(const std::string& Format, TArgs&&... Args)
    {
        return Printf(Format.c_str(), std::forward<TArgs>(Args)...);
    }

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
