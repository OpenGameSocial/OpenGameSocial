#pragma once

#include "OGS/Core.h"
#include <string>


namespace OGS
{
    template <typename ... TArgs>
    std::string Printf(const char* Format, TArgs&& ... Args)
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

    template <typename ... TArgs>
    std::string Printf(const std::string& Format, TArgs&& ... Args)
    {
        return Printf(Format.c_str(), std::forward<TArgs>(Args) ...);
    }
}
